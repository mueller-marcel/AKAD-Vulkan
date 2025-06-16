#pragma once

#include <deque>
#include <functional>
#include <ranges>
#include <vector>
#include <vk_types.h>

#include "vk_mesh.h"

/**
 * A queue to manage the cleanup
 */
struct DeletionQueue {
	/**
	 * A queue of functions to be executed to clean up all resources
	 */
	std::deque<std::function<void()>> _deletors;

	/**
	 * Add a new cleanup function to the queue
	 * @param function The function used to clean up
	 */
	void push_function(std::function<void()>&& function) {
		_deletors.push_back(function);
	}

	/**
	 * Execute the functions in the queue to perform the cleanup
	 */
	void flush() {
		for (auto & _deletor : std::ranges::reverse_view(_deletors)) {
			_deletor();
		}

		_deletors.clear();
	}
};

/**
 * Class to construct the pipeline using the builder pattern
 */
class PipelineBuilder {
	public:

	/**
	 * The shader stages of the pipeline builder
	 */
	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;

	/**
	 * The vertex input of the pipeline builder
	 */
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;

	/**
	 * The input assembly of the pipeline builder
	 */
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;

	/**
	 * The viewport of the pipeline builder
	 */
	VkViewport _viewport;

	/**
	 * The scissors of the pipeline builder
	 */
	VkRect2D _scissor;

	/**
	 * The rasterizer of the pipeline builder
	 */
	VkPipelineRasterizationStateCreateInfo _rasterizer;

	/**
	 * The color blend attachment of the pipeline builder
	 */
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;

	/**
	 * The multisampling of the pipeline builder
	 */
	VkPipelineMultisampleStateCreateInfo _multisampling;

	/**
	 * The pipeline layout of the pipeline builder
	 */
	VkPipelineLayout _pipelineLayout;

	/**
	 * Build the pipeline
	 * @param device The device to build the pipeline for
	 * @param renderpass The render pass of the pipeline
	 * @return
	 */
	VkPipeline build_pipeline(VkDevice device, VkRenderPass renderpass) const;
};

/**
 * A class responsible for initializing and managing the Vulkan rendering engine,
 * including setup, resource allocation, and rendering operations.
 */
class VulkanEngine {
private :
	/**
	 * Initializes the Vulkan API and performs the necessary setup required for
	 * creating a Vulkan rendering context, including instance creation,
	 * validation layers setup, and other essential configurations.
	 */
	void init_vulkan();

	/**
	 * Initializes the swapchain for the Vulkan rendering engine.
	 * This setup involves creating and configuring the swapchain, which manages
	 * the series of renderable surfaces used to present images to the screen.
	 */
	void init_swapchain();

	/**
	 * Initializes the command buffers and command pool necessary for issuing rendering and
	 * resource management commands in the Vulkan engine.
	 */
	void init_commands();

	/**
	 * Initializes the default renderpass
	 */
	void init_default_renderpass();

	/**
	 * Initializes the framebuffers
	 */
	void init_framebuffers();

	/**
	 * Initializes the sync structures
	 */
	void init_sync_structures();

	/**
	 * Loads the shaders
	 * @param file_path The shader's file path
	 * @param out_shader_module The pointer to the loaded shader module
	 * @return Indicates whether the shader was loaded successfully
	 */
	bool load_shader_module(const char* file_path, VkShaderModule* out_shader_module) const;

	/**
	 * Initialize the render pipeline
	 */
	void init_pipelines();

	void load_meshes();

	void upload_mesh(Mesh& mesh);

public:
	/**
	 * Indicates whether the engine is initialized
	 */
	bool _isInitialized{ false };

	/**
	 * The number of the frame which is initialized as 0
	 */
	int _frameNumber {0};

	/**
	 * Stops the rendering and is initialized with false
	 */
	bool stop_rendering{ false };

	/**
	 * The window extent indicates the size of the window
	 */
	VkExtent2D _windowExtent{ 1700 , 900 };

	/**
	 * The Vulkan instance representing the connection between the application and the Vulkan library.
	 * It serves as the foundation for using Vulkan APIs and managing resources.
	 */
	VkInstance _instance;

	/**
	 * The debug messenger responsible for handling debug callbacks, such as validation layer warnings and errors during Vulkan runtime.
	 * It facilitates debugging by logging messages provided by the Vulkan validation layers.
	 */
	VkDebugUtilsMessengerEXT _debugMessenger;

	/**
	 * The physical device selected for rendering operations.
	 * Represents a GPU on the system that meets the required criteria for Vulkan operations.
	 */
	VkPhysicalDevice _chosenGPU;

	/**
	 * The logical Vulkan device used for interfacing with the selected physical device (GPU).
	 * It represents an abstraction of the physical GPU and provides methods to execute commands,
	 * manage resources, and perform operations within the Vulkan API.
	 */
	VkDevice _device;

	/**
	 * The Vulkan surface representing the connection between the Vulkan instance and the platform-specific window system.
	 * It is used as a target for rendering operations and facilitates presenting rendered images to the screen.
	 */
	VkSurfaceKHR _surface;

	/**
	 * A Vulkan handle representing a swapchain, which manages the presentation of rendered images
	 * to a surface and facilitates image buffering for rendering operations.
	 */
	VkSwapchainKHR _swapchain;

	/**
	 * Stores the image format used for the Vulkan swapchain.
	 * This format determines how the pixels are represented in the swapchain images
	 * used for rendering and presentation to the display.
	 */
	VkFormat _swapchainImageFormat;

	/**
	 * A container that stores the images used in the Vulkan swapchain,
	 * which are presented as part of the rendering process.
	 */
	std::vector<VkImage> _swapchainImages;

	/**
	 * A container that holds the image views corresponding to the images in the swapchain.
	 * These image views are used to interface with the swapchain images for rendering operations.
	 */
	std::vector<VkImageView> _swapchainImageViews;

	/**
	 * Represents the Vulkan queue used for submitting graphics commands to the GPU.
	 * It is responsible for handling operations related to rendering and graphics processing.
	 */
	VkQueue _graphicsQueue;

	/**
	 * Stores the index of the queue family within a Vulkan physical device
	 * that supports graphics operations. This index is used to assign
	 * and manage the graphics queue for rendering tasks.
	 */
	uint32_t _graphicsQueueFamilyIndex;

	/**
	 * A Vulkan command pool used for managing the allocation of command buffers,
	 * which are used for recording commands to be submitted to a GPU queue.
	 */
	VkCommandPool _commandPool;

	/**
	 * Represents a Vulkan command buffer used to record and submit rendering or
	 * compute commands to the Vulkan command queue.
	 */
	VkCommandBuffer _commandBuffer;

	/**
	 * The renderpass to define the layout of the render target
	 */
	VkRenderPass _renderPass;

	/**
	 * The buffers for the frames
	 */
	std::vector<VkFramebuffer> _framebuffers;

	/**
	 * The current semaphore for the parallel process
	 */
	VkSemaphore _presentSemaphore;

	/**
	 * The semaphore for the rendering
	 */
	VkSemaphore _renderSemaphore;

	/**
	 * The render fence used for the synchronization between the queue and the cpu
	 */
	VkFence _renderFence;

	/**
	 * The render pipeline for the triangle
	 */
	VkPipeline _trianglePipeline;

	/**
	 * The layout for the pipeline used to render the triangle
	 */
	VkPipelineLayout _trianglePipelineLayout;

	/**
	 * The allocator for the vulkan memory management
	 */
	VmaAllocator _allocator;

	/**
	 * The rendering pipeline for the meshes
	 */
	VkPipeline _meshPipeline;

	/**
	 * Contains the mesh of the triangle
	 */
	Mesh _triangleMesh;

	/**
	 * The cleanup queue to delete all vulkan resources
	 */
	DeletionQueue _mainDeletionQueue;

	/**
	 * The SDL window used as the main rendering target and interface for the Vulkan engine.
	 * Serves as the platform-specific window connected to the Vulkan surface for presenting rendered images.
	 */
	struct SDL_Window* _window{ nullptr };

	/**
	 * Get the reference to the vulkan engine
	 * @return The reference to the vulkan engine
	 */
	static VulkanEngine& Get();

	/**
	 * Initializes the Vulkan rendering engine.
	 *
	 * This method sets up the necessary resources and states required for the Vulkan engine,
	 * including the initialization of the SDL library, creation of a window with Vulkan support
	 * and registering the current engine instance to ensure single initialization.
	 *
	 * It ensures that only one Vulkan engine instance can be initialized per application session.
	 * Upon successful execution, the engine is marked as initialized and ready for rendering operations.
	 *
	 * Assertions:
	 * - Ensures that the engine is initialized only once by asserting that no other engine instance is loaded.
	 *
	 * Postconditions:
	 * - The `_window` is created and configured for Vulkan rendering.
	 * - The `_isInitialized` flag is set to `true` to indicate successful initialization.
	 */
	void init();

	/**
	 * Releases resources and performs cleanup operations for the Vulkan engine.
	 *
	 * This method is responsible for safely cleaning up and deallocating resources used by the Vulkan engine.
	 * If the engine is initialized, it destroys the SDL window and resets the global engine pointer to ensure
	 * that the engine state is properly cleared and no lingering references remain.
	 *
	 * Preconditions:
	 * - The engine must have been initialized before invoking this method.
	 *
	 * Postconditions:
	 * - The SDL window (_window) is destroyed if it was created.
	 * - The global engine pointer (loadedEngine) is set to nullptr.
	 * - The Vulkan engine is no longer operational after cleanup.
	 */
	void cleanup();

	/**
	 * Executes the rendering process for the Vulkan engine.
	 *
	 * This method is responsible for managing the drawing operations of the Vulkan engine,
	 * including issuing rendering commands, synchronizing GPU operations, and preparing
	 * the frame to be presented on the target surface. It is invoked as part of the main rendering loop.
	 *
	 * Preconditions:
	 * - The Vulkan engine must have been initialized and running.
	 *
	 * Postconditions:
	 * - A frame is rendered and prepared for presentation on the Vulkan surface, provided rendering is not stopped.
	 *
	 * Note:
	 * - Does not execute if the `stop_rendering` flag is set to true.
	 */
	void draw();

	/**
	 * Starts the main execution loop of the Vulkan rendering engine,
	 * handling window events, maintaining application state, and performing rendering operations.
	 *
	 * The method listens for window events such as close requests or window state changes (e.g., minimization).
	 * In minimized states, rendering is paused to conserve resources, while a throttling mechanism
	 * ensures the application does not consume excessive CPU cycles.
	 *
	 * Within the loop, the rendering process is initiated unless the application is in a minimized state.
	 */
	void run();
};
