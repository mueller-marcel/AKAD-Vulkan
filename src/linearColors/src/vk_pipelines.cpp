﻿#include "../include/vk_pipelines.h"
#include "../include/vk_initializers.h"
#include <vulkan/vulkan_core.h>

void PipelineBuilder::clear() {
    _input_assembly = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    _rasterizer = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    _color_blend_attachment = {};
    _multisampling = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    pipeline_layout = {};
    _depth_stencil = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    _render_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    _shader_stages.clear();
}

VkPipeline PipelineBuilder::build_pipeline(VkDevice device) {
    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineColorBlendStateCreateInfo color_blending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = nullptr, // TODO: Should be color_blend_attachment
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    VkDynamicState state[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = &state[0],
    };

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        // connect the renderInfo to the pNext extension mechanism
        .pNext = &_render_info,
        .stageCount = (uint32_t) _shader_stages.size(),
        .pStages = _shader_stages.data(),
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &_input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &_rasterizer,
        .pMultisampleState = &_multisampling,
        .pDepthStencilState = &_depth_stencil,
        .pColorBlendState = &color_blending,
        .pDynamicState = &dynamic_info,
        .layout = pipeline_layout,
    };

    VkPipeline new_pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info,
                                  nullptr, &new_pipeline) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }

    return new_pipeline;
}

void PipelineBuilder::set_shaders(VkShaderModule vertex_shader, VkShaderModule fragment_shader) {
    _shader_stages.clear();
    _shader_stages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertex_shader));
    _shader_stages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader));
}

void PipelineBuilder::set_input_topology(VkPrimitiveTopology topology) {
    _input_assembly.topology = topology;
    _input_assembly.primitiveRestartEnable = false;
}

void PipelineBuilder::set_polygon_mode(VkPolygonMode mode) {
    _rasterizer.polygonMode = mode;
    _rasterizer.lineWidth = 1.0f;
}

void PipelineBuilder::set_cull_mode(VkCullModeFlags cull_mode, VkFrontFace front_face) {
    _rasterizer.cullMode = cull_mode;
    _rasterizer.frontFace = front_face;
}

void PipelineBuilder::set_multisampling_none() {
    _multisampling.sampleShadingEnable = VK_FALSE;
    _multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _multisampling.minSampleShading = 1.0f;
    _multisampling.pSampleMask = nullptr;
    _multisampling.alphaToCoverageEnable = VK_FALSE;
    _multisampling.alphaToOneEnable = VK_FALSE;
}

void PipelineBuilder::disable_blending() {
    _color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                             VK_COLOR_COMPONENT_G_BIT |
                                             VK_COLOR_COMPONENT_B_BIT |
                                             VK_COLOR_COMPONENT_A_BIT;

    _color_blend_attachment.blendEnable = VK_FALSE;
}

void PipelineBuilder::enable_blending_additive() {
    _color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                             VK_COLOR_COMPONENT_G_BIT |
                                             VK_COLOR_COMPONENT_B_BIT |
                                             VK_COLOR_COMPONENT_A_BIT;

    _color_blend_attachment.blendEnable = VK_TRUE;
    _color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    _color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    _color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    _color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    _color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    _color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void PipelineBuilder::enable_blending_alpha() {
    _color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                             VK_COLOR_COMPONENT_G_BIT |
                                             VK_COLOR_COMPONENT_B_BIT |
                                             VK_COLOR_COMPONENT_A_BIT;

    _color_blend_attachment.blendEnable = VK_TRUE;
    _color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    _color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    _color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    _color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    _color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    _color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void PipelineBuilder::set_color_attachment_format(VkFormat format) {
    _color_attachment_format = format;
    _render_info.colorAttachmentCount = 1;
    _render_info.pColorAttachmentFormats = &_color_attachment_format;
}

void PipelineBuilder::set_depth_format(VkFormat format) {
    _render_info.depthAttachmentFormat = format;
}

void PipelineBuilder::enable_depth_test(bool depth_write_enable, VkCompareOp op) {
    _depth_stencil.depthTestEnable = VK_TRUE;
    _depth_stencil.depthWriteEnable = depth_write_enable;
    _depth_stencil.depthCompareOp = op;
    _depth_stencil.depthBoundsTestEnable = VK_FALSE;
    _depth_stencil.stencilTestEnable = VK_FALSE;
    _depth_stencil.front = {};
    _depth_stencil.back = {};
    _depth_stencil.minDepthBounds = 0.0f;
    _depth_stencil.maxDepthBounds = 1.0f;
}

void PipelineBuilder::disable_depth_test() {
    _depth_stencil.depthTestEnable = VK_FALSE;
    _depth_stencil.depthWriteEnable = VK_FALSE;
    _depth_stencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    _depth_stencil.depthBoundsTestEnable = VK_FALSE;
    _depth_stencil.stencilTestEnable = VK_FALSE;
    _depth_stencil.front = {};
    _depth_stencil.back = {};
    _depth_stencil.minDepthBounds = 0.0f;
    _depth_stencil.maxDepthBounds = 1.0f;
}

bool vkutil::load_shader_module(const char* file_path, VkDevice device,
        VkShaderModule* out_shader_module) {
    BundleFileData shader_data{};
    bool shader_found = false;

    for (int i = 0; i < BUNDLE_FILE_COUNT; i++) {
        BundleFileData data = BUNDLE_FILES[i];
        if (data.path == file_path) {
            shader_data = data;
            shader_found = true;
            break;
        }
    }

    if (!shader_found) {
        return false;
    }

    // create a new shader module, using the buffer we loaded
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext = nullptr;

    // codeSize has to be in bytes, so multiply the ints in the buffer by size
    // of int to know the real size of the buffer
    create_info.codeSize = shader_data.size;
    create_info.pCode = (uint32_t*)&BUNDLE_DATA[shader_data.start_idx];

    // check that the creation goes well.
    VkShaderModule shader_module;
    if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) !=
            VK_SUCCESS) {
        return false;
            }

    *out_shader_module = shader_module;
    return true;
}