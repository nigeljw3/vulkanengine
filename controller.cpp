#include "controller.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <limits>

Controller::Controller()
{
	
}

Controller::~Controller()
{
	
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{
    std::cerr << msg << std::endl;

    return VK_FALSE;
}

bool Controller::Init()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
		
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	VkExtensionProperties* extensions = static_cast<VkExtensionProperties*>(malloc(sizeof(VkExtensionProperties)*extensionCount));
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);
	
	for(uint32_t i = 0; i < extensionCount; ++i)
	{
		std::cout << extensions[i].extensionName << std::endl;
	}
	
	uint32_t requestedExtensionCount = 3;
	
	const char* requestedExtensions[] =
	{
		"VK_KHR_surface",
		"VK_KHR_win32_surface",
		"VK_EXT_debug_report"
	};
	//requestedExtensions = static_cast<char**>(malloc(sizeof(char*)*extensionCount));
	
	bool supported = false;
	
	for(uint32_t i = 0; i < requestedExtensionCount; ++i)
	{
		//std::cout << extensions[i].extensionName << std::endl;
		
		//requestedExtensions[i] = static_cast<char*>(malloc(sizeof(char)*VK_MAX_EXTENSION_NAME_SIZE));
		
		//strncpy(requestedExtensions[i], extensions[i].extensionName, VK_MAX_EXTENSION_NAME_SIZE);
	
		supported = false;
	
		for(uint32_t j = 0; j < extensionCount; ++j)
		{
			if (strncmp(requestedExtensions[i], extensions[j].extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0)
			{
				supported = true;
			}
		}
		
		if (supported == false)
		{
			std::cout << "Extension " << requestedExtensions[i] << " not supported" << std::endl;
			
			break;
		}
	}

	//strncpy(requestedExtensions[0], "VK_KHR_surface", VK_MAX_EXTENSION_NAME_SIZE);
	//strncpy(requestedExtensions[1], "VK_KHR_win32_surface", VK_MAX_EXTENSION_NAME_SIZE);
	//strncpy(requestedExtensions[2], "VK_EXT_debug_report", VK_MAX_EXTENSION_NAME_SIZE);
	
	//bool supported = windowView.CheckExtensionsSupport(extensionCount, extensions);
			
	createInfo.enabledExtensionCount = requestedExtensionCount;
	createInfo.ppEnabledExtensionNames = requestedExtensions;
	
	uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    VkLayerProperties* availableLayers = static_cast<VkLayerProperties*>(malloc(sizeof(VkLayerProperties)*layerCount));; 
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
		
	supported = false;
	
	for(uint32_t i = 0; i < layerCount; ++i)
	{
		if (strcmp(layer, availableLayers[i].layerName) == 0)
		{
			supported = true;
			break;
		}
		
		//std::cout << availableLayers[i].layerName << std::endl;
	}
	
	if (supported == false)
	{
		std::cout << "Layer not supported:" << layer << std::endl;
	}
	
	createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = &layer;
	
	
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Failed to create instance" << std::endl;
	}	
	
	VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
	callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	callbackCreateInfo.pfnCallback = debugCallback;
	
	auto vkCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	
	VkResult callbackResult = vkCreateDebugReportCallback(instance, &callbackCreateInfo, nullptr, &callback);
	
	if (callbackResult != VK_SUCCESS)
	{
		std::cout << "error: did not create callback succesfuly" << std::endl;
	}
	
	physicalDevice = VK_NULL_HANDLE;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	
	if (deviceCount == 0)
	{
		std::cout << "failed to find a device that supports Vulkan" << std::endl;	
	}
	
	VkPhysicalDevice* devices = static_cast<VkPhysicalDevice*>(malloc(sizeof(VkPhysicalDevice)*deviceCount));
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
	VkPhysicalDeviceProperties deviceProperties;
	
	bool foundDiscreteGPU = false;
	
	for (uint32_t i = 0; i < deviceCount; ++i)
	{
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		
		//std::cout << deviceProperties.deviceType << std::endl;
		
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			physicalDevice = devices[i];
			foundDiscreteGPU = true;
			break;
		}
	}
	
	if (foundDiscreteGPU == false)
	{
		std::cout << "failed to find a discrete GPU" << std::endl;
	}

	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
	
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	free(devices);
	free(availableLayers);
	
	/*for(uint32_t i = 0; i < extensionCount; ++i)
	{
		free(requestedExtensions[i]);
	}*/

	//free(requestedExtensions);
	free(extensions);
	
	return true;
}

bool Controller::SetupQueue()
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	
	if (queueFamilyCount == 0)
	{
		std::cout << "failed to find a device that supports Vulkan" << std::endl;	
	}
	
	VkQueueFamilyProperties* queueFamilies = static_cast<VkQueueFamilyProperties*>(malloc(sizeof(VkQueueFamilyProperties)*queueFamilyCount));
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);
	
	for (uint32_t i = 0; i < queueFamilyCount; ++i)
	{
		std::cout << queueFamilies[i].queueCount << ", " << std::hex << queueFamilies[i].queueFlags << std::endl;
		
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queueFamilyId = i;
			break;
		}
	}
	
	if (queueFamilyId == InvalidIndex)
	{
		std::cout << "failed to get all indicies" << std::endl;
	}
	
	free(queueFamilies);
	
	return true;
}

bool Controller::SetupDevice(VkSurfaceKHR& surface)
{
	VkBool32 presentSupport = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyId, surface, &presentSupport);
	
	if (presentSupport != true)
	{
		std::cout << "surface presetation not supported" << std::endl;
	}
	
	uint32_t queueCount = 2;
	
	float* queuePriorities = static_cast<float*>(malloc(sizeof(float)*queueCount));
	
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	
	queuePriorities[graphicsQueueIndex] = 1.0f;
	queuePriorities[presentQueueIndex] = 1.0f; 
	
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = queueFamilyId;
	queueCreateInfo.queueCount = queueCount;
	queueCreateInfo.pQueuePriorities = queuePriorities;
	
	uint32_t deviceExtensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);

    VkExtensionProperties* availableDeviceExtensions = static_cast<VkExtensionProperties*>(malloc(sizeof(VkExtensionProperties)*deviceExtensionCount));
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, availableDeviceExtensions);
	
	uint32_t requestedDeviceExtensionCount = 1;
	
	//char** requestedDeviceExtensions;
	//requestedDeviceExtensions = static_cast<char**>(malloc(sizeof(char*)*deviceExtensionCount));
	
	const char* requestedDeviceExtension = "VK_KHR_swapchain";
	
	bool supported = false;
	
	for(uint32_t i = 0; i < deviceExtensionCount; ++i)
	{
		std::cout << availableDeviceExtensions[i].extensionName << std::endl;
		
		//requestedDeviceExtensions[i] = static_cast<char*>(malloc(sizeof(char)*VK_MAX_EXTENSION_NAME_SIZE));
		//strncpy(requestedDeviceExtensions[i], availableDeviceExtensions[i].extensionName, VK_MAX_EXTENSION_NAME_SIZE);
		
		if (strncmp(requestedDeviceExtension, availableDeviceExtensions[i].extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0)
		{
			supported = true;
		}
	}
	
	if (supported == false)
	{
		std::cout << "VK_KHR_swapchain device extension not supported" << std::endl;
	}
	
	//strncpy(requestedDeviceExtensions[0], "VK_KHR_swapchain", VK_MAX_EXTENSION_NAME_SIZE);
	
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledLayerCount = 1;
    deviceCreateInfo.ppEnabledLayerNames = &layer;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.enabledExtensionCount = requestedDeviceExtensionCount;
	deviceCreateInfo.ppEnabledExtensionNames = &requestedDeviceExtension;
	
	VkResult deviceResult = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
	
	if (deviceResult != VK_SUCCESS)
	{
		std::cout << "failed to create device" << std::endl;
	}
	
	//for(uint32_t i = 0; i < deviceExtensionCount; ++i)
	//{
	//	free(requestedDeviceExtensions[i]);
	//}

	//free(requestedDeviceExtensions);
	free(availableDeviceExtensions);
	
	return true;
}

bool Controller::Configure(VkSurfaceKHR& surface)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
	
	PrintCapabilities();
	
	VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	
	if ((capabilities.supportedTransforms & transform) == 0)
	{
		std::cout << "Transform not supported" << std::endl;
	}
	
	return true;
}

void Controller::PrintCapabilities()
{
	/*uint32_t minImageCount;
    uint32_t maxImageCount;
    VkExtent2D currentExtent;
    VkExtent2D minImageExtent;
    VkExtent2D maxImageExtent;
    uint32_t maxImageArrayLayers;
    VkSurfaceTransformFlagsKHR supportedTransforms;
    VkSurfaceTransformFlagBitsKHR currentTransform;
    VkCompositeAlphaFlagsKHR supportedCompositeAlpha;
    VkImageUsageFlags supportedUsageFlags;*/
	
	std::cout << capabilities.minImageCount << std::endl;
	std::cout << capabilities.maxImageCount << std::endl;
	std::cout << capabilities.currentExtent.width << std::endl;
	std::cout << capabilities.currentExtent.height << std::endl;
	std::cout << capabilities.minImageExtent.width << std::endl;
	std::cout << capabilities.minImageExtent.height << std::endl;
	std::cout << capabilities.maxImageExtent.width << std::endl;
	std::cout << capabilities.maxImageExtent.height << std::endl;
	std::cout << capabilities.maxImageArrayLayers << std::endl;
	std::cout << capabilities.supportedTransforms << std::endl;
	std::cout << capabilities.currentTransform << std::endl;
	std::cout << capabilities.supportedCompositeAlpha << std::endl;
	std::cout << capabilities.supportedUsageFlags << std::endl;
}

bool Controller::PresentModeSupported(VkSurfaceKHR& surface, VkPresentModeKHR presentMode)
{
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	VkPresentModeKHR* supportedPresentModes = static_cast<VkPresentModeKHR*>(malloc(sizeof(VkPresentModeKHR)*presentModeCount));
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, supportedPresentModes);
	
	bool supported = false;	
	
	for(uint32_t i = 0; i < presentModeCount; ++i)
	{
		std::cout << std::hex << supportedPresentModes[i] << std::endl;
		
		if (supportedPresentModes[i] == presentMode)
		{
			supported = true;
		}
	}
	
	if (!supported)
	{
		std::cout << "Present mode not supported" << std::endl;
	}
	
	free(supportedPresentModes);
	
	return supported;
}

bool Controller::SurfaceFormatSupported(VkSurfaceKHR& surface, VkFormat surfaceFormat)
{
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	VkSurfaceFormatKHR* surfaceFormats = static_cast<VkSurfaceFormatKHR*>(malloc(sizeof(VkSurfaceFormatKHR)*formatCount));
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats);
		
	bool supported = false;
	
	std::cout << std::endl;
	for(uint32_t i = 0; i < formatCount; ++i)
	{
		std::cout << std::hex << surfaceFormats[i].format << ", " << surfaceFormats[i].colorSpace << std::endl;
	
		if (surfaceFormats[i].format == surfaceFormat)
		{
			supported = true;
		}
	}
	
	if (!supported)
	{
		std::cout << "Format not supported" << std::endl;
	}
	
	free(surfaceFormats);
	
	return supported;
}

bool Controller::Destroy()
{
	auto vkDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    vkDestroyDebugReportCallback(instance, callback, nullptr);
	
	vkDestroyInstance(instance, nullptr);
	
	return true;
}
