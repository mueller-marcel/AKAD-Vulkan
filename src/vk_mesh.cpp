#include <iostream>
#include <vk_mesh.h>

#include "tiny_obj_loader.h"

VertexInputDescription Vertex::get_vertex_description() {
    VertexInputDescription description;
    // Use one vertex buffer binding with per-vertex rate
    VkVertexInputBindingDescription main_binding = {};
    main_binding.binding = 0;
    main_binding.stride = sizeof(Vertex);
    main_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    description.bindings.push_back(main_binding);

    // The position will be stored at location 0
    VkVertexInputAttributeDescription position_attribute = {};
    position_attribute.binding = 0;
    position_attribute.location = 0;
    position_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    position_attribute.offset = offsetof(Vertex, position);

    // The normal will be stored at location 1
    VkVertexInputAttributeDescription normal_attribute = {};
    normal_attribute.binding = 0;
    normal_attribute.location = 1;
    normal_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    normal_attribute.offset = offsetof(Vertex, normal);

    // The position will be stored at Location 2
    VkVertexInputAttributeDescription color_attribute = {};
    color_attribute.binding = 0;
    color_attribute.location = 2;
    color_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    color_attribute.offset = offsetof(Vertex, color);

    description.attributes.push_back(position_attribute);
    description.attributes.push_back(normal_attribute);
    description.attributes.push_back(color_attribute);
    
    return description;
}

bool Mesh::load_from_obj(const char *file_name) {

    // Declaration of variables
    tinyobj::attrib_t vertex_attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    std::string warn;

    // Load the OBJ file
    tinyobj::LoadObj(&vertex_attributes, &shapes, &materials, &warn, &err, file_name, nullptr);

    // Check for warnings
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }

    // Check for errors
    if (!err.empty()) {
        std::cerr << "ERROR" << err << std::endl;
        return false;
    }

    // Loop over shapes
    for (auto & shape : shapes) {

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

            int face_vertices = 3;

            // Loop over vertices in the face.
            for (size_t v = 0; v < face_vertices; v++) {

                // Access the vertex
                tinyobj::index_t index = shape.mesh.indices[index_offset + v];

                // Get the vertices and normals
                tinyobj::real_t vertex_x = vertex_attributes.vertices[3 * index.vertex_index + 0];
                tinyobj::real_t vertex_y = vertex_attributes.vertices[3 * index.vertex_index + 1];
                tinyobj::real_t vertex_z = vertex_attributes.vertices[3 * index.vertex_index + 2];
                tinyobj::real_t normal_x = vertex_attributes.normals[3 * index.normal_index + 0];
                tinyobj::real_t normal_y = vertex_attributes.normals[3 * index.normal_index + 1];
                tinyobj::real_t normal_z = vertex_attributes.normals[3 * index.normal_index + 2];

                // Copy to the vertex struct
                Vertex new_vert;
                new_vert.position.x = vertex_x;
                new_vert.position.y = vertex_y;
                new_vert.position.z = vertex_z;
                new_vert.normal.x = normal_x;
                new_vert.normal.y = normal_y;
                new_vert.normal.z = normal_z;

                // Set color as vertex normal
                new_vert.color = new_vert.normal;

                _vertices.push_back(new_vert);
            }

            index_offset += face_vertices;
        }
    }

    return true;
}
