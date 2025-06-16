#pragma once

#include <vector>
#include <vk_types.h>
#include <glm/vec3.hpp>

/**
 * Represents the vertex input description
 */
struct VertexInputDescription {
    /**
     * The vertex input binding descriptions
     */
    std::vector<VkVertexInputBindingDescription> bindings;

    /**
     * The vertex input attribute description
     */
    std::vector<VkVertexInputAttributeDescription> attributes;

    /**
     * The vertex input state create flags
     */
    VkPipelineVertexInputStateCreateFlags flags = 0;
};

/**
 * Represents a vertex
 */
struct Vertex {
    /**
     * The position of the vertex
     */
    glm::vec3 position;

    /**
     * The normal of the vertex
     */
    glm::vec3 normal;

    /**
     * The color of the vertex
     */
    glm::vec3 color;

    /**
     * Gets the vertex description containing bindings and attributes
     * @return The vertex input description
     */
    static VertexInputDescription get_vertex_description();
};

/**
 * Represents a mesh
 */
struct Mesh {
    /**
     * The vertices of the mesh
     */
    std::vector<Vertex> _vertices;

    /**
     * The allocated buffer
     */
    AllocatedBuffer _vertexBuffer;
};