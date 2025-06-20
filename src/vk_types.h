#pragma once

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>

/**
 * Represents allocated buffer in the memory
 */
struct AllocatedBuffer {
    /**
     * The buffer
     */
    VkBuffer _buffer;

    /**
     * The data of the allocation
     */
    VmaAllocation _allocation;
};

/**
 * Represents allocated image
 */
struct AllocatedImage {
    /**
     * The image
     */
    VkImage _image;

    /**
     * The data of the allocation
     */
    VmaAllocation _allocation;
};