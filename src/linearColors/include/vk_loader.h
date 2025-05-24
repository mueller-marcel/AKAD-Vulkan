#pragma once

#include <filesystem>
#include "optional"
#include "vk_loader.h"
#include "vk_types.h"

struct GLTFMaterial {
    MaterialInstance instance;
};

struct GeoSurface {
    uint32_t start_index;
    uint32_t count;
    std::shared_ptr<GLTFMaterial> material;
};

struct MeshAsset {
    std::string name;
    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers mesh_buffers;
};

class VulkanEngine;

std::optional<std::vector<std::shared_ptr<MeshAsset> > > load_meshes(VulkanEngine *engine,
                                                                     std::filesystem::path file_path);
