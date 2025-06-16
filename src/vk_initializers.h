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

    /**
     * Defines the buffer begin info
     * @param flags The flags for the buffer begin info
     * @return The parameters for the instantiation of the command buffer begin
     */
    VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0);

    /**
     * Initializes the fence for the synchronization structures
     * @param flags The flags for the fence
     * @return The parameters for the instantiation of the fence
     */
    VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);

    /**
     * Initializes semaphore as a synchronization structure
     * @param flags The flags for the semaphore
     * @return the parameters for the instantiation of the semaphore
     */
    VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0);

    /**
     * Initializes the submit-queue for the command buffer
     * @param command_buffer The command buffer of the submit-queue
     * @return The parameters for the instantiation of the submit-queue
     */
    VkSubmitInfo submit_info(VkCommandBuffer* command_buffer);

    /**
     * Initializes the presentation of the images
     * @return The parameters for the instantiation of the presentation of the images
     */
    VkPresentInfoKHR present_info();

    /**
     * Initializes a framebuffer
     * @param renderPass The renderpass
     * @param vk_extent_2d The extent of the framebuffer
     * @return The parameters for the instantiation of the framebuffers
     */
    VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass renderPass, VkExtent2D vk_extent_2d);

    /**
     * Initializes the start of the render pass
     * @param render_pass The renderpass to be started
     * @param windowExtent The extent of the window
     * @param framebuffer The framebuffer
     * @return The parameters for the instantiation of the start of the renderpass
     */
    VkRenderPassBeginInfo render_pass_begin_info(VkRenderPass render_pass, VkExtent2D windowExtent, VkFramebuffer framebuffer);
}
