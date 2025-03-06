#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class DrawTriangle
{
public:
	void run();

private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();

	void createInstance();

	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffer();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	std::vector<const char*> getRequiredExtension() const;

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
	GLFWwindow* window;

	friend static bool checkValidationLayerSupport();
	friend bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	inline const static std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	inline const static std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
};