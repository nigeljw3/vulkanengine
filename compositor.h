#ifndef compositor_h
#define compositor_h

#include "renderer.h"

#include <vulkan/vulkan.h>

class Compositor
{
public:
	Compositor(VkPhysicalDeviceMemoryProperties& memProperties);
	~Compositor();
	
	bool Init(VkDevice& device,
			  VkSurfaceKHR& surface,
			  uint32_t queueFamilyId,
			  uint32_t graphicsQueueIndex,
			  uint32_t presentQueueIndex);
	//bool CreateSurface(VkInstance& instance, VkSurfaceKHR* surface);
	void Loop();
	bool Destroy(VkDevice& device);
	bool CheckExtensionsSupport(uint32_t extensionCount, VkExtensionProperties* extensions);
	VkFormat GetSurfaceFormat() { return surfaceFormat; }
	VkPresentModeKHR GetPresentMode() { return presentMode; }
	
private:
	void PrintCapabilities();

	Renderer* graphicsEngine;
	VkSurfaceCapabilitiesKHR capabilities;
	VkImage* images;
	VkSemaphore waitSemaphore;
	VkSemaphore signalSemaphore;
	VkSwapchainKHR swapChain;
	VkImageView imageView;
	VkPhysicalDeviceMemoryProperties memProperties;
	
	const VkFormat surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
	const VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
};

#endif