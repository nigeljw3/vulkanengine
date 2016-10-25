#ifndef compositor_h
#define compositor_h

#include "renderer.h"

#include <vulkan/vulkan.h>

class Compositor
{
public:
	Compositor();
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

private:
	void PrintCapabilities();

	Renderer* graphicsEngine;
	VkSurfaceCapabilitiesKHR capabilities;
	VkImage* images;
	VkSemaphore waitSemaphore;
	VkSemaphore signalSemaphore;
	VkSwapchainKHR swapChain;
	VkImageView imageView;
};

#endif