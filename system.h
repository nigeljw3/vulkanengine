#ifndef system_h
#define system_h

#include "compositor.h"

#include <vulkan/vulkan.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

class System
{
public:
	System();
	~System();
	
	bool Init();
	bool CreateSurface(VkInstance& instance, VkSurfaceKHR* surface);
	void Loop(Compositor& composer, VkDevice& device);
	bool DestroySurface(VkInstance& instance, VkSurfaceKHR& surface);
	bool Destroy();
	bool CheckExtensionsSupport(uint32_t extensionCount, VkExtensionProperties* extensions);

	
private:
	GLFWwindow* window;
	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
};

#endif