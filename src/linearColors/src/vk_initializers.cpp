﻿#include "../include/vk_initializers.h"

VkCommandPoolCreateInfo vkinit::command_pool_create_info(uint32_t queue_family_index, VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.queueFamilyIndex = queue_family_index;
    info.flags = flags;

    return info;
}

VkCommandBufferAllocateInfo vkinit::command_buffer_allocate_info(VkCommandPool pool, uint32_t count) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = pool;
    info.commandBufferCount = count;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    return info;
}

VkFenceCreateInfo vkinit::fence_create_info(VkFenceCreateFlags flags) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;

    return info;
}

VkSemaphoreCreateInfo vkinit::semaphore_create_info(VkSemaphoreCreateFlags flags) {
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;

    return info;
}

VkCommandBufferBeginInfo vkinit::command_buffer_begin_info(VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.pInheritanceInfo = nullptr;
    info.flags = flags;

    return info;
}

VkImageSubresourceRange vkinit::image_subresource_range(VkImageAspectFlags aspect_mask) {
    VkImageSubresourceRange range = {};
    range.aspectMask = aspect_mask;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return range;
}

VkSemaphoreSubmitInfo vkinit::semaphore_submit_info(VkPipelineStageFlags2 stage_mask, VkSemaphore semaphore) {
    VkSemaphoreSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    info.pNext = nullptr;
    info.semaphore = semaphore;
    info.stageMask = stage_mask;
    info.deviceIndex = 0;
    info.value = 1;

    return info;
}

VkCommandBufferSubmitInfo vkinit::command_buffer_submit_info(VkCommandBuffer cmd) {
    VkCommandBufferSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.pNext = nullptr;
    info.commandBuffer = cmd;
    info.deviceMask = 0;

    return info;
}

VkSubmitInfo2 vkinit::submit_info(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signal_semaphore_info, VkSemaphoreSubmitInfo *wait_semaphore_info) {
    VkSubmitInfo2 info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    info.pNext = nullptr;
    info.waitSemaphoreInfoCount = wait_semaphore_info == nullptr ? 0 : 1;
    info.pWaitSemaphoreInfos = wait_semaphore_info;
    info.signalSemaphoreInfoCount = signal_semaphore_info == nullptr ? 0 : 1;
    info.pSignalSemaphoreInfos = signal_semaphore_info;
    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;

    return info;
}

VkImageCreateInfo vkinit::image_create_info(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent) {
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = format;
    info.extent = extent;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usage_flags;

    return info;
}

VkImageViewCreateInfo vkinit::imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags) {
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = aspect_flags;

    return info;
}

VkRenderingAttachmentInfo vkinit::depth_attachment_info(VkImageView view, VkImageLayout layout) {
    VkRenderingAttachmentInfo depth_attachment = {};
    depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_attachment.pNext = nullptr;
    depth_attachment.imageView = view;
    depth_attachment.imageLayout = layout;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.clearValue.depthStencil.depth = 0.f;

    return depth_attachment;
}

VkRenderingAttachmentInfo vkinit::attachment_info(VkImageView view, VkClearValue* clear, VkImageLayout layout) {
    VkRenderingAttachmentInfo color_attachment = {};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.pNext = nullptr;
    color_attachment.imageView = view;
    color_attachment.imageLayout = layout;
    color_attachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    if (clear) {
        color_attachment.clearValue = *clear;
    }

    return color_attachment;
}

VkRenderingInfo vkinit::rendering_info(VkExtent2D render_extent, VkRenderingAttachmentInfo* color_attachment, VkRenderingAttachmentInfo* depth_attachment) {
    VkRenderingInfo render_info = {};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.pNext = nullptr;
    render_info.renderArea = VkRect2D{ VkOffset2D{ 0, 0 }, render_extent };
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = color_attachment;
    render_info.pDepthAttachment = depth_attachment;
    render_info.pStencilAttachment = nullptr;

    return render_info;
}

VkPipelineLayoutCreateInfo vkinit::pipeline_layout_create_info() {
    VkPipelineLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.setLayoutCount = 0;
    info.pSetLayouts = nullptr;
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges = nullptr;

    return info;
}

VkPipelineShaderStageCreateInfo vkinit::pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shader_module, const char* entry) {
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.stage = stage;
    info.module = shader_module;
    info.pName = entry;

    return info;
}