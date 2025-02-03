#include <DrawTriangle.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <unordered_set>

static bool areAllExtensionsSupported(const std::vector<const char*>&, uint32_t, const std::vector<VkExtensionProperties>&);


void DrawTriangle::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void DrawTriangle::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void DrawTriangle::initVulkan()
{
	createInstance();
}

void DrawTriangle::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
}

void DrawTriangle::cleanup()
{
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();
}

void DrawTriangle::createInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	std::vector<const char*> requiredExtensions;
	requiredExtensions.reserve(glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
	{
		requiredExtensions.emplace_back(glfwExtensions[i]);
	}
	requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "available extensions:\n";
	for (const auto& extension : extensions)
	{
		std::cout << '\t' << extension.extensionName << '\n';
	}

	if (!areAllExtensionsSupported(requiredExtensions, glfwExtensionCount, extensions))
	{
		throw std::runtime_error("Missing required extensions!");
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

static bool areAllExtensionsSupported(const std::vector<const char*>& glfwExtensions, uint32_t glfwExtensionCount, const std::vector<VkExtensionProperties>& extensions)
{
	std::unordered_set<std::string> supportedExtensions;
	for (const auto& extension : extensions)
	{
		supportedExtensions.insert(extension.extensionName);
	}

	for (const char* requiredExtension : glfwExtensions)
	{
		if (supportedExtensions.find(requiredExtension) == supportedExtensions.end())
		{
			std::cerr << "Missing required extension: " << requiredExtension << std::endl;
			return false;
		}
	}

	return true;
}
