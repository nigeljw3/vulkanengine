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
	
	bool Draw(VkDevice& device);
	
private:
	void PrintCapabilities();
	
	uint32_t imageCount;
	uint32_t drawIndex;

	Renderer* graphicsEngine;
	VkSurfaceCapabilitiesKHR capabilities;
	VkImage* images;
	VkSemaphore waitSemaphore;
	VkSemaphore signalSemaphore;
	VkSwapchainKHR swapChain;
	VkImageView* imageViews;
	VkPhysicalDeviceMemoryProperties memProperties;
	
	VkQueue presentQueue;
	VkQueue graphicsQueue;
	
	VkCommandBuffer* drawCommandBuffer;
	VkCommandBuffer* transferCommandBuffer;
			
	const VkFormat surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
	const VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
};

#endif