// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_descriptors.h>
#include "VkBootstrap.h"
#include "vk_pipelines.h"


struct DeletionQueue {
	std::deque<std::function<void()>> _deletors;
	void push_function(std::function<void()> &&function) {
		_deletors.push_back(function);
	}

	void flush() {
		for (auto it = _deletors.rbegin(); it != _deletors.rend(); it++) {
			(*it)();
		}
		_deletors.clear();
	}
};

struct FrameData {
	VkCommandPool _commandPool { VK_NULL_HANDLE };
	VkCommandBuffer _mainCommandBuffer { VK_NULL_HANDLE };
	VkSemaphore _swapChainSemahore { VK_NULL_HANDLE }, _renderSemaphore { VK_NULL_HANDLE };
	VkFence _renderFence { VK_NULL_HANDLE };
	DeletionQueue _deletionQueue;
};

struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

struct ComputeEffect {
	const char *name;
	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {
public:
	std::vector<FrameData> _frames;
	FrameData &get_current_frame() { return _frames[_frameNumber % _frames.size()]; };
	VkQueue _graphicsQueue { VK_NULL_HANDLE };
	uint32_t _graphicsQueueFamily { 0 };
	bool _isInitialized { false };
	int _frameNumber { 0 };
	bool stop_rendering { false };
	VkExtent2D _windowExtent { 1700 , 900 };
	VkInstance _instance { VK_NULL_HANDLE };
	VkDebugUtilsMessengerEXT _debug_messenger { VK_NULL_HANDLE };
	VkPhysicalDevice _chosenGPU { VK_NULL_HANDLE };
	VkDevice _device { VK_NULL_HANDLE };
	VkSurfaceKHR _surface { VK_NULL_HANDLE };
	VkSwapchainKHR _swapchain { VK_NULL_HANDLE };
	VkFormat _swapchainImageFormat { VK_FORMAT_UNDEFINED };
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
	VkExtent2D _swapchainExtent;
	DeletionQueue _mainDeletionQueue;
	VmaAllocator _allocator { VK_NULL_HANDLE };
	AllocatedImage _drawImage;
	VkExtent2D _drawExtent;
	DescriptorAllocator globalDescriptorAllocator;
	VkDescriptorSet _drawImageDescriptors { VK_NULL_HANDLE };
	VkDescriptorSetLayout _drawImageDescriptorLayout { VK_NULL_HANDLE };
	VkPipeline _gradientPipeline { VK_NULL_HANDLE };
	VkPipelineLayout _gradientPipelineLayout { VK_NULL_HANDLE };
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;
	std::vector<ComputeEffect> backgroundEffects;
	int currentBackgroundEffect { 0 };

	struct SDL_Window *_window { nullptr };

	static VulkanEngine &Get();

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();
	void immediate_submit(std::function<void(VkCommandBuffer cmd)> &&function);
private:
	void init_imgui();
	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
	void draw_background(VkCommandBuffer cmd);
	void init_descriptors();
	void init_pipelines();
	void init_background_pipelines();
	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
};
