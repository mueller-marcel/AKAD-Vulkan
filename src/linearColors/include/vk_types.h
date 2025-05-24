#pragma once

#include <memory>
#include <vector>
#include <vk_mem_alloc.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>

struct AllocatedImage {
    VkImage image;
    VkImageView image_view;
    VmaAllocation allocation;
    VkExtent3D image_extent;
    VkFormat image_format;
};

struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};

struct Vertex {
    glm::vec3 position;
    float uv_x;
    glm::vec3 normal;
    float uv_y;
    glm::vec4 color;
};

// holds the resources needed for a mesh
struct GPUMeshBuffers {
    AllocatedBuffer index_buffer;
    AllocatedBuffer vertex_buffer;
    VkDeviceAddress vertex_buffer_address;
};

// push constants for our mesh object draws
struct GPUDrawPushConstants {
    glm::mat4 world_matrix;
    VkDeviceAddress vertex_buffer;
};

enum class MaterialPass : uint8_t {
    MainColor,
    Transparent,
    Other,
};

struct MaterialPipeline {
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

struct MaterialInstance {
    MaterialPipeline *pipeline;
    VkDescriptorSet material_set;
    MaterialPass pass_type;
};

struct DrawContext;

class IRenderable {
public:
    virtual ~IRenderable() = default;

    virtual void draw(const glm::mat4 &top_matrix, DrawContext &ctx) = 0;
};

struct Node : IRenderable {
    // parent pointer must be a weak pointer to avoid circular dependencies
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node> > children;

    glm::mat4 local_transform;
    glm::mat4 world_transform;

    void refresh_transform(const glm::mat4 &parent_matrix) {
        world_transform = parent_matrix * local_transform;
        for (auto c: children) {
            c->refresh_transform(world_transform);
        }
    }

    void draw(const glm::mat4 &top_matrix, DrawContext &ctx) override {
        // draw children
        for (auto &c: children) {
            c->draw(top_matrix, ctx);
        }
    }
};
