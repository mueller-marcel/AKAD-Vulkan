#pragma once

namespace vkinit {

    /**
     * Create the command pool to manage the memory for the command buffers
     * @param queueFamilyIndex Assigns the command queue for the command pool
     * @param flags Controls the behavior of the command pool
     * @return The parameters to create a command pool
     */
    VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);

    /**
     * Creates the command buffer to send commands to the gpu. The commands are stored in a queue to be executed.
     * @param pool The command pool the buffer is assigned to
     * @param count The number of command buffers. Default to 1 because we need a command buffer for every thread.
     * Since this application is single-threaded, we just need one command buffer
     * @return The parameters that store all the parameters for the command buffer allocation
     */
    VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1);

    /**
     * Creates the pipeline shader stage to control the graphics and compute pipeline to define
     * when the shaders (fragment-shader or vertex-shader) are executed.
     * @param stage These flags define the stages that are used in the pipeline
     * @param shaderModule The compiled shaders to be processed from the pipeline
     * @param entry The VK instance to be used
     * @return The parameters for the shader stage
     */
    VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shaderModule, const char * entry = "main");

    /**
     * Defines how Vertex data is used in the pipeline
     * @return The parameters for the pipeline vertex state
     */
    VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info();

    /**
     * Defines how primitives from the vertex data are assembled
     * @param topology Defines whether points, lines or triangles are rendered
     * @return The parameters for the pipeline assembly state
     */
    VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info(VkPrimitiveTopology topology);

    /**
     * Defines how primitives are converted to fragments
     * @return The parameters for the instantiation
     */
    VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info(VkPolygonMode polygon_mode);

    /**
     * Controls the multisampling settings.
     * @return The parameters for the instantiation
     */
    VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info();

    /**
     * Controls the mixture of the colors in the pipeline
     * @return The parameters for the instantiation
     */
    VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state();

    /**
     * Defines the pipeline layout for the descriptor sets and the push constants
     * @return The parameters for the instantiation
     */
    VkPipelineLayoutCreateInfo pipeline_layout_create_info();


VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0);
VkCommandBufferSubmitInfo command_buffer_submit_info(VkCommandBuffer cmd);

VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);

VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0);

    VkSubmitInfo submit_info(VkCommandBuffer* command_buffer);
VkPresentInfoKHR present_info();

VkRenderingAttachmentInfo attachment_info(VkImageView view, const VkClearValue* clear, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

VkRenderingAttachmentInfo depth_attachment_info(VkImageView view,
    VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

VkRenderingInfo rendering_info(VkExtent2D renderExtent, const VkRenderingAttachmentInfo* colorAttachment,
    const VkRenderingAttachmentInfo* depthAttachment);

VkImageSubresourceRange image_subresource_range(VkImageAspectFlags aspectMask);

VkSemaphoreSubmitInfo semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
VkDescriptorSetLayoutBinding descriptor_set_layout_binding(VkDescriptorType type, VkShaderStageFlags stageFlags,
    uint32_t binding);
VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(const VkDescriptorSetLayoutBinding* bindings,
    uint32_t bindingCount);
VkWriteDescriptorSet write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet,
    const VkDescriptorImageInfo* imageInfo, uint32_t binding);
VkWriteDescriptorSet write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet,
    const VkDescriptorBufferInfo* bufferInfo, uint32_t binding);
VkDescriptorBufferInfo buffer_info(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);

VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
VkImageViewCreateInfo imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);

VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass renderPass, VkExtent2D vk_extent_2d);

VkRenderPassBeginInfo render_pass_begin_info(VkRenderPass render_pass, VkExtent2D windowExtent, VkFramebuffer framebuffer);
}
