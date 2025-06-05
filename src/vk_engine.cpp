#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_initializers.h>
#include <vk_types.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <VkBootstrap.h>

using namespace std;
#define VK_CHECK(x)                                                                 \
do {                                                                                \
    VkResult err = x;                                                               \
    if (err != VK_SUCCESS) {                                                        \
        std::cout << "Detected Vulkan error: " << err << std::endl;                 \
        abort();                                                                    \
    }                                                                               \
} while (0)

VulkanEngine *loadedEngine = nullptr;

VulkanEngine &VulkanEngine::Get() { return *loadedEngine; }

void VulkanEngine::init() {
    // only one engine initialization is allowed with the application.
    assert(loadedEngine == nullptr);
    loadedEngine = this;

    // We initialize SDL and create a window with it.
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_VULKAN);

    _window = SDL_CreateWindow(
        "Vulkan Engine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _windowExtent.width,
        _windowExtent.height,
        window_flags);

    // Initialize the vulkan engine
    init_vulkan();

    // Initialize the swapchain
    init_swapchain();

    // Initialize the commands
    init_commands();

    // Initialize the default render pass
    init_default_renderpass();

    // Initialize the framebuffers
    init_framebuffers();

    // Initialize the sync structures
    init_sync_structures();

    // everything went fine
    _isInitialized = true;
}

void VulkanEngine::init_vulkan() {
    vkb::InstanceBuilder builder;

    // Create a vulkan instance with basic debug features
    auto inst_ret = builder
            .set_app_name("Colored Triangle")
            .request_validation_layers(true)
            .require_api_version(1, 1, 0)
            .use_default_debug_messenger()
            .build();

    // Store the instance and the debug messenger
    vkb::Instance vkb_inst = inst_ret.value();
    _instance = vkb_inst.instance;
    _debugMessenger = vkb_inst.debug_messenger;

    // Create a surface
    SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

    // Select a gpu using vk bootstrap
    vkb::PhysicalDeviceSelector selector(vkb_inst);
    vkb::PhysicalDevice physical_device = selector
            .set_minimum_version(1, 1)
            .set_surface(_surface)
            .select()
            .value();

    // Create the vulkan device representation
    vkb::DeviceBuilder device_builder{physical_device};
    vkb::Device vkb_device = device_builder.build().value();

    // Choose the gpu
    _device = vkb_device.device;
    _chosenGPU = physical_device.physical_device;

    // Get the graphics queue
    _graphicsQueue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    _graphicsQueueFamilyIndex = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
}

void VulkanEngine::init_swapchain() {
    vkb::SwapchainBuilder swapchain_builder{_chosenGPU, _device, _surface};

    // Build the swap chain
    vkb::Swapchain vkb_swapchain = swapchain_builder
            .use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(_windowExtent.width, _windowExtent.height)
            .build()
            .value();

    // Assign the swapchain and the images
    _swapchain = vkb_swapchain.swapchain;
    _swapchainImages = vkb_swapchain.get_images().value();
    _swapchainImageViews = vkb_swapchain.get_image_views().value();
    _swapchainImageFormat = vkb_swapchain.image_format;
}

void VulkanEngine::init_commands() {
    // Create a command pool for the commands enqueued in the graphics queue
    VkCommandPoolCreateInfo command_pool_create_info = vkinit::command_pool_create_info(
        _graphicsQueueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(_device, &command_pool_create_info, nullptr, &_commandPool));

    // Allocate the default command buffer
    VkCommandBufferAllocateInfo command_buffer_allocate_info = vkinit::command_buffer_allocate_info(_commandPool, 1);

    VK_CHECK(vkAllocateCommandBuffers(_device, &command_buffer_allocate_info, &_commandBuffer));
}

void VulkanEngine::init_default_renderpass() {
    VkAttachmentDescription color_attachment = {};

    // Use the format needed by the swapchain
    color_attachment.format = _swapchainImageFormat;

    // Initialize samples with one bit
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // Clear when the attachment is loaded
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

    // Keep the attachment when the renderpass ends
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    // Don't care about stencils
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // Starting layout is undefined
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // After the renderpass ends, the layout has to be ready to display
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Attachment number will index into the pAttachments array
    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Create one subpass, which is the minimum
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    // Connect the color attachment to the info
    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;

    VK_CHECK(vkCreateRenderPass(_device, &render_pass_create_info, nullptr, &_renderPass));
}

void VulkanEngine::init_framebuffers() {

    // Create the framebuffers for the swapchain images
    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.pNext = nullptr;
    framebuffer_create_info.renderPass = _renderPass;
    framebuffer_create_info.attachmentCount = 1;
    framebuffer_create_info.width = _windowExtent.width;
    framebuffer_create_info.height = _windowExtent.height;
    framebuffer_create_info.layers = 1;

    // Grab the images for the swapchain
    const uint32_t swapchain_image_count = _swapchainImageViews.size();
    _framebuffers = std::vector<VkFramebuffer>(swapchain_image_count);

    // Create framebuffers for each of the swapchain image views
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        framebuffer_create_info.pAttachments = &_swapchainImageViews[i];
        VK_CHECK(vkCreateFramebuffer(_device, &framebuffer_create_info, nullptr, &_framebuffers[i]));
    }
}

void VulkanEngine::init_sync_structures() {

    // Create the synchronization structures
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateFence(_device, &fence_create_info, nullptr, &_renderFence));

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = nullptr;
    semaphore_create_info.flags = 0;

    VK_CHECK(vkCreateSemaphore(_device, &semaphore_create_info, nullptr, &_presentSemaphore));
    VK_CHECK(vkCreateSemaphore(_device, &semaphore_create_info, nullptr, &_renderSemaphore));
}


void VulkanEngine::cleanup() {

    if (_isInitialized) {
        // Destroy the command pool
        vkDestroyCommandPool(_device, _commandPool, nullptr);

        // Destroy the swapchain and its images
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);

        // Destroy the renderpass
        vkDestroyRenderPass(_device, _renderPass, nullptr);

        // Destroy framebuffers and swapchain images
        for (int i = 0; i < _framebuffers.size(); i++) {
            vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
            vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
        }

        vkDestroyDevice(_device, nullptr);
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        vkb::destroy_debug_utils_messenger(_instance, _debugMessenger);
        vkDestroyInstance(_instance, nullptr);
        SDL_DestroyWindow(_window);
    }

    // clear engine pointer
    loadedEngine = nullptr;
}

void VulkanEngine::draw() {

    // Wait for the GPU to render the last frame with a timeout of 1 second
    VK_CHECK(vkWaitForFences(_device, 1, &_renderFence, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(_device, 1, &_renderFence));

    // Request image from the swapchain
    uint32_t swapchain_image_index;
    VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, UINT64_MAX, _presentSemaphore, nullptr, &swapchain_image_index));
    VK_CHECK(vkResetCommandBuffer(_commandBuffer, 0));

    // Begin the command buffer recording
    VkCommandBuffer command_buffer = _commandBuffer;
    VkCommandBufferBeginInfo command_buffer_create_info = {};
    command_buffer_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_create_info.pNext = nullptr;
    command_buffer_create_info.pInheritanceInfo = nullptr;
    command_buffer_create_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_create_info));

    // Create a clear collor from the frame number. It will flash
    VkClearValue clear_value = {};
    float flash = abs(sin(_frameNumber / 120.0f));
    clear_value.color = {{0.0f, 0.0f, flash, 1.0f}};

    // Start the main renderpass
    VkRenderPassBeginInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_create_info.pNext = nullptr;
    render_pass_create_info.renderPass = _renderPass;
    render_pass_create_info.renderArea.offset.x = 0;
    render_pass_create_info.renderArea.offset.y = 0;
    render_pass_create_info.renderArea.extent = _windowExtent;
    render_pass_create_info.framebuffer = _framebuffers[swapchain_image_index];
    render_pass_create_info.clearValueCount = 1;
    render_pass_create_info.pClearValues = &clear_value;

    vkCmdBeginRenderPass(command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdEndRenderPass(command_buffer);

    VK_CHECK(vkEndCommandBuffer(command_buffer));

    // Prepare the submission to the queue
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;

    VkPipelineStageFlags wait_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit_info.pWaitDstStageMask = &wait_stage_flags;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &_presentSemaphore;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &_renderSemaphore;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit_info, _renderFence));

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.pSwapchains = &_swapchain;
    present_info.swapchainCount = 1;
    present_info.pWaitSemaphores = &_renderSemaphore;
    present_info.waitSemaphoreCount = 1;
    present_info.pImageIndices = &swapchain_image_index;

    VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &present_info));

    // Increase the number of frames
    _frameNumber++;
}

void VulkanEngine::run() {
    SDL_Event e;
    bool bQuit = false;

    // main loop
    while (!bQuit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // close the window when user alt-f4s or clicks the X button
            if (e.type == SDL_QUIT)
                bQuit = true;

            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    stop_rendering = true;
                }
                if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                    stop_rendering = false;
                }
            }
        }

        // do not draw if we are minimized
        if (stop_rendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw();
    }
}
