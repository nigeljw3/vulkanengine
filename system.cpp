#include "system.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <cstdio>

System::System()
{
	
}

System::~System()
{
	
}

bool System::Init()
{
	bool result = true;
	
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	return result;
}

bool System::Destroy()
{
    glfwDestroyWindow(window);

    glfwTerminate();
	
	return true;
}

void System::Loop()
{
	while(!glfwWindowShouldClose(window))
	{
        glfwPollEvents();
    }
}

bool System::CreateSurface(VkInstance& instance, VkSurfaceKHR* surface)
{
	bool result = true;
	
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = glfwGetWin32Window(window);
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	
	auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
	
	VkResult surfaceResult = CreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, surface);
	//glfwCreateWindowSurface(instance, window, nullptr, &surface)
	
	if (surfaceResult != VK_SUCCESS)
	{
		std::cout << "failed to create surface" << std::endl;
		result = false;
	}
	
	return result;
}

bool System::DestroySurface(VkInstance& instance, VkSurfaceKHR& surface)
{
	auto DestroySurfaceKHR = (PFN_vkDestroySurfaceKHR) vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR");
	DestroySurfaceKHR(instance, surface, nullptr);
}

bool System::CheckExtensionsSupport(uint32_t extensionCount, VkExtensionProperties* extensions)
{
	bool fullySupported = true;
	
	for(unsigned int i = 0; i < glfwExtensionCount; ++i)
	{
		std::cout << glfwExtensions[i] << std::endl;
		
		bool extensionSupported = false;
	
		for(unsigned int j = 0; j < extensionCount; ++j)
		{
			if (strcmp(glfwExtensions[i], extensions[j].extensionName) == 0)
			{
				extensionSupported = true;
				break;
			}
		}
		
		if (extensionSupported == false)
		{
			std::cout << "Extension not supported:" << glfwExtensions[i] << std::endl;
			fullySupported = false;
		}
	}
	
	return fullySupported;
}