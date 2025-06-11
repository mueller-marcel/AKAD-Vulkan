#pragma once

#include <vk_types.h>
#include <vector>

class PipelineBuilder {
	public:
	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;

	VkPipeline build_pipeline(VkDevice device, VkRenderPass renderpass);
};

/**
 * A class responsible for initializing and managing the Vulkan rendering engine,
 * including setup, resource allocation, and rendering operations.
 */
class VulkanEngine {
private :
	/**
	 * Initializes the Vulkan API and performs necessary setup required for
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
	 * @param file_path The shaders file path
	 * @param out_shader_module The pointer to the loaded shader module
	 * @return Indicates whether the shader was loaded successfully
	 */
	bool load_shader_module(const char* file_path, VkShaderModule* out_shader_module);

	/**
	 * Initialize the render pipeline
	 */
	void init_pipelines();

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
	 * that supports graphics operations. This index is utilized to assign
	 * and manage the graphics queue for rendering tasks.
	 */
	uint32_t _graphicsQueueFamilyIndex;

	/**
	 * A Vulkan command pool used for managing the allocation of command buffers,
	 * which are utilized for recording commands to be submitted to a GPU queue.
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

	VkSemaphore _presentSemaphore;

	VkSemaphore _renderSemaphore;

	VkFence _renderFence;

	VkPipeline _trianglePipeline;

	VkPipelineLayout _trianglePipelineLayout;

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
	 * including the initialization of the SDL library, creation of a window with Vulkan support,
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
