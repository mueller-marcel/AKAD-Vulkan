#include "vk_engine.h"
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vk_initializers.h>
#include <vk_types.h>
#include <chrono>
#include <fstream>
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
    // We initialize SDL and create a window with it.
    SDL_Init(SDL_INIT_VIDEO);

    auto window_flags = SDL_WINDOW_VULKAN;

    // Cast the window extent to int
    const int width = static_cast<int>(_windowExtent.width);
    const int height = static_cast<int>(_windowExtent.height);

    // Create the window
    _window = SDL_CreateWindow(
        "Vulkan Triangle with color gradient",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
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

    // Initialize the pipelines
    init_pipelines();

    // everything went fine
    _isInitialized = true;
}

void VulkanEngine::init_vulkan() {
    vkb::InstanceBuilder builder;

    // Create a vulkan instance with basic debug features
    auto inst_ret = builder
            .set_app_name("Colored Triangle")
            .request_validation_layers()
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

    // Add the swapchain to the queue to be deleted
    _mainDeletionQueue.push_function([=, this]() {
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    });
}

void VulkanEngine::init_commands() {
    // Create a command pool for the commands enqueued in the graphics queue
    VkCommandPoolCreateInfo command_pool_create_info = vkinit::command_pool_create_info(
        _graphicsQueueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(_device, &command_pool_create_info, nullptr, &_commandPool));

    // Allocate the default command buffer
    VkCommandBufferAllocateInfo command_buffer_allocate_info = vkinit::command_buffer_allocate_info(_commandPool, 1);

    VK_CHECK(vkAllocateCommandBuffers(_device, &command_buffer_allocate_info, &_commandBuffer));

    // Add the command pools to the deletion queue
    _mainDeletionQueue.push_function([=, this]() {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
    });
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

    // Create a dependency to the subpass
    VkSubpassDependency subpass_dependency = {};
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Connect the color attachment to the info
    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &subpass_dependency;

    VK_CHECK(vkCreateRenderPass(_device, &render_pass_create_info, nullptr, &_renderPass));

    // Add the renderpass to deletion queue for later deletion
    _mainDeletionQueue.push_function([=, this]() {
        vkDestroyRenderPass(_device, _renderPass, nullptr);
    });
}

void VulkanEngine::init_framebuffers() {
    // Create the framebuffers for the swapchain images
    VkFramebufferCreateInfo framebuffer_create_info = vkinit::framebuffer_create_info(_renderPass, _windowExtent);

    // Grab the images for the swapchain
    const uint32_t swapchain_image_count = _swapchainImages.size();
    _framebuffers = std::vector<VkFramebuffer>(swapchain_image_count);

    // Create framebuffers for each of the swapchain image views
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        framebuffer_create_info.pAttachments = &_swapchainImageViews[i];
        VK_CHECK(vkCreateFramebuffer(_device, &framebuffer_create_info, nullptr, &_framebuffers[i]));

        // Add each framebuffer to the deletion queue
        _mainDeletionQueue.push_function([=, this]() {
            vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
            vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
        });
    }
}

void VulkanEngine::init_sync_structures() {
    // Create the synchronization structures
    VkFenceCreateInfo fence_create_info = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

    VK_CHECK(vkCreateFence(_device, &fence_create_info, nullptr, &_renderFence));

    // Queue the fence for the deletion queue
    _mainDeletionQueue.push_function([=, this]() {
        vkDestroyFence(_device, _renderFence, nullptr);
    });

    // The parameters for the semaphore instantiation
    VkSemaphoreCreateInfo semaphore_create_info = vkinit::semaphore_create_info(0);

    VK_CHECK(vkCreateSemaphore(_device, &semaphore_create_info, nullptr, &_presentSemaphore));
    VK_CHECK(vkCreateSemaphore(_device, &semaphore_create_info, nullptr, &_renderSemaphore));

    // Add the sync structures to the deletion queue
    _mainDeletionQueue.push_function([=, this]() {
        vkDestroySemaphore(_device, _presentSemaphore, nullptr);
        vkDestroySemaphore(_device, _renderSemaphore, nullptr);
    });
}

bool VulkanEngine::load_shader_module(const char *file_path, VkShaderModule *out_shader_module) const {
    // Load the shader from the file
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    // Get the size of the file
    size_t file_size = file.tellg();
    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));
    file.seekg(0);
    file.read(reinterpret_cast<char *>(buffer.data()), file_size);
    file.close();

    // Create a new shader module using the loaded buffer
    VkShaderModuleCreateInfo shader_module_create_info = {};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.pNext = nullptr;
    shader_module_create_info.codeSize = buffer.size() * sizeof(uint32_t);
    shader_module_create_info.pCode = buffer.data();

    // Load the file into the shader module
    VkShaderModule shader_module;
    if (vkCreateShaderModule(_device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS) {
        return false;
    }

    // Point to the shader module
    *out_shader_module = shader_module;

    return true;
}

void VulkanEngine::init_pipelines() {
    VkShaderModule triangle_vertex_shader;

    // Load the vertex shader
    if (!load_shader_module("../shaders/triangle.vert.spv", &triangle_vertex_shader)) {
        std::cout << "Failed to load triangle shader" << std::endl;
    } else {
        std::cout << "Loaded triangle shader" << std::endl;
    }

    VkShaderModule triangle_fragment_shader;

    // Load the fragment shader
    if (!load_shader_module("../shaders/triangle.frag.spv", &triangle_fragment_shader)) {
        std::cout << "Failed to load triangle shader" << std::endl;
    } else {
        std::cout << "Loaded triangle shader" << std::endl;
    }

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = vkinit::pipeline_layout_create_info();
    VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_create_info, nullptr, &_trianglePipelineLayout));

    PipelineBuilder pipeline_builder;

    pipeline_builder._shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, triangle_vertex_shader));

    pipeline_builder._shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangle_fragment_shader));

    pipeline_builder._vertexInputInfo = vkinit::pipeline_vertex_input_state_create_info();

    pipeline_builder._inputAssembly = vkinit::pipeline_input_assembly_state_create_info(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipeline_builder._viewport.x = 0.0f;
    pipeline_builder._viewport.y = 0.0f;
    pipeline_builder._viewport.width = static_cast<float>(_windowExtent.width);
    pipeline_builder._viewport.height = static_cast<float>(_windowExtent.height);
    pipeline_builder._viewport.minDepth = 0.0f;
    pipeline_builder._viewport.maxDepth = 1.0f;
    pipeline_builder._scissor.offset = {0, 0,};
    pipeline_builder._scissor.extent = _windowExtent;
    pipeline_builder._rasterizer = vkinit::pipeline_rasterization_state_create_info(VK_POLYGON_MODE_FILL);
    pipeline_builder._multisampling = vkinit::pipeline_multisample_state_create_info();
    pipeline_builder._colorBlendAttachment = vkinit::pipeline_color_blend_attachment_state();
    pipeline_builder._pipelineLayout = _trianglePipelineLayout;
    _trianglePipeline = pipeline_builder.build_pipeline(_device, _renderPass);

    // Destroy all shader modules, outside the queue
    vkDestroyShaderModule(_device, triangle_vertex_shader, nullptr);
    vkDestroyShaderModule(_device, triangle_fragment_shader, nullptr);

    // Destroy the pipeline
    _mainDeletionQueue.push_function([=, this]() {
        vkDestroyPipeline(_device, _trianglePipeline, nullptr);
        vkDestroyPipelineLayout(_device, _trianglePipelineLayout, nullptr);
    });
}

VkPipeline PipelineBuilder::build_pipeline(VkDevice device, VkRenderPass renderpass) const {
    // The parameters for the viewport and scissor settings, which defines how the scene is projected
    VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.pNext = nullptr;
    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.pViewports = &_viewport;
    viewport_state_create_info.scissorCount = 1;
    viewport_state_create_info.pScissors = &_scissor;

    // The parameters for the color mixture settings
    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
    color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_create_info.pNext = nullptr;
    color_blend_state_create_info.logicOpEnable = VK_FALSE;
    color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state_create_info.attachmentCount = 1;
    color_blend_state_create_info.pAttachments = &_colorBlendAttachment;

    // The parameters for the graphics pipeline
    VkGraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.pNext = nullptr;
    pipeline_create_info.stageCount = _shaderStages.size();
    pipeline_create_info.pStages = _shaderStages.data();
    pipeline_create_info.pVertexInputState = &_vertexInputInfo;
    pipeline_create_info.pInputAssemblyState = &_inputAssembly;
    pipeline_create_info.pViewportState = &viewport_state_create_info;
    pipeline_create_info.pRasterizationState = &_rasterizer;
    pipeline_create_info.pMultisampleState = &_multisampling;
    pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
    pipeline_create_info.layout = _pipelineLayout;
    pipeline_create_info.renderPass = renderpass;
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

    // Assure the instantiation of the graphics pipeline instantiation
    VkPipeline new_pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &new_pipeline) !=
        VK_SUCCESS) {
        std::cout << "Failed to create graphics pipeline" << std::endl;
        return VK_NULL_HANDLE;
    }

    return new_pipeline;
}


void VulkanEngine::cleanup() {
    if (_isInitialized) {
        // Make sure the GPU has stopped
        vkDeviceWaitIdle(_device);
        // Flush the deletion queue to remove all resources
        _mainDeletionQueue.flush();

        // Destroy other resources that are not captured by the deletion queue
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
    VK_CHECK(vkWaitForFences(_device, 1, &_renderFence, VK_TRUE, 1000000000));
    VK_CHECK(vkResetFences(_device, 1, &_renderFence));

    // Reset the command buffer
    VK_CHECK(vkResetCommandBuffer(_commandBuffer, 0));

    // Request image from the swapchain
    uint32_t swapchain_image_index;
    VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, UINT64_MAX, _presentSemaphore, nullptr, &swapchain_image_index));

    // Begin the command buffer recording
    VkCommandBuffer command_buffer = _commandBuffer;
    VkCommandBufferBeginInfo command_buffer_create_info = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_create_info));

    // Create a clear color from the frame number. It will flash
    VkClearValue clear_value = {};
    // float flash = abs(sin(_frameNumber / 120.0f));
    float flash = 0.0f;
    clear_value.color = {{0.0f, 0.0f, flash, 0.0f}};

    // Start the main renderpass
    VkRenderPassBeginInfo render_pass_create_info = vkinit::render_pass_begin_info(_renderPass, _windowExtent, _framebuffers[swapchain_image_index]);

    // Connect clear values
    render_pass_create_info.clearValueCount = 1;
    render_pass_create_info.pClearValues = &clear_value;

    // Start the render pass
    vkCmdBeginRenderPass(command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

    // Initialize the pipeline and bind it to the command buffer
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _trianglePipeline);
    vkCmdDraw(command_buffer, 3, 1, 0, 0);

    // End the render pass
    vkCmdEndRenderPass(command_buffer);

    VK_CHECK(vkEndCommandBuffer(command_buffer));

    // Prepare the submission to the queue
    VkSubmitInfo submit_info = vkinit::submit_info(&command_buffer);
    VkPipelineStageFlags pipeline_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit_info.pWaitDstStageMask = &pipeline_stage_flags;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &_presentSemaphore;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &_renderSemaphore;

    VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit_info, _renderFence));

    // Define how images from the swapchain are projected on the monitor
    VkPresentInfoKHR present_info = vkinit::present_info();
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
