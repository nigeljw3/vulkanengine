#include "compositor.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <limits>

Compositor::Compositor(VkPhysicalDeviceMemoryProperties& memProps)
: memProperties(memProps)
{
	
}

Compositor::~Compositor()
{
	
}

bool Compositor::Init(VkDevice& device,
					  VkSurfaceKHR& surface,
					  uint32_t queueFamilyId,
					  uint32_t graphicsQueueIndex,
					  uint32_t presentQueueIndex)
{	
	//bool supported = false;
	VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	
	//VkSurfaceFormatKHR swapSurfaceFormat;
	
	//swapSurfaceFormat.format = requiredFormat;
	//swapSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	
	VkExtent2D screenExtent = {800, 600};

    //actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    //actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
	
	uint32_t imageCount = 2;
	
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = imageCount;
	swapchainCreateInfo.imageFormat = surfaceFormat;
	swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCreateInfo.imageExtent = screenExtent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	swapchainCreateInfo.preTransform = transform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		
	bool result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapChain);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Swapchain creation failed" << std::endl;
	}
	
	VkQueue presentQueue;
	VkQueue graphicsQueue;
	
	vkGetDeviceQueue(device, queueFamilyId, graphicsQueueIndex, &graphicsQueue);
	vkGetDeviceQueue(device, queueFamilyId, presentQueueIndex, &presentQueue);

	vkDeviceWaitIdle(device);
	
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	
	images = static_cast<VkImage*>(malloc(sizeof(VkImage)*imageCount));
	
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images);
	
	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = images[0];
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = surfaceFormat;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	
	result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Image view creation failed" << std::endl;
	}
	
	graphicsEngine = new Renderer(screenExtent, memProperties);
	
	graphicsEngine->Init(device, surfaceFormat, &imageView, queueFamilyId);
	
	VkCommandBuffer* transferCommandBuffer = graphicsEngine->TransferBuffers(device);
	
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = transferCommandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);
		
	VkCommandBuffer* commandBuffer = graphicsEngine->ConstructFrame();
	
	VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	vkCreateSemaphore(device, &semaphoreInfo, nullptr, &waitSemaphore);
	vkCreateSemaphore(device, &semaphoreInfo, nullptr, &signalSemaphore);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Semaphore creation failed" << std::cout;
	}
	
	uint32_t imageIndex;
	uint64_t max64BitInt = std::numeric_limits<uint64_t>::max();
	vkAcquireNextImageKHR(device, swapChain, max64BitInt, waitSemaphore, VK_NULL_HANDLE, &imageIndex);
	
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	
	submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &waitSemaphore;
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &signalSemaphore;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer;
	
	result = vkQueueSubmit(presentQueue, 1, &submitInfo, VK_NULL_HANDLE);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Queue submit failed" << std::cout;
	}
	
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &signalSemaphore;
	
	//VkSwapchainKHR swapChains[] = {swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain;
	presentInfo.pImageIndices = &imageIndex;
	//presentInfo.pResults = nullptr; 
	
	vkQueuePresentKHR(presentQueue, &presentInfo);
	
	vkQueueWaitIdle(presentQueue);
	
	return true;
}

bool Compositor::Destroy(VkDevice& device)
{
	vkDestroySemaphore(device, waitSemaphore, nullptr);
	vkDestroySemaphore(device, signalSemaphore, nullptr);	
	
	graphicsEngine->Destroy(device);
	
	delete graphicsEngine;
	
	vkDestroyImageView(device, imageView, nullptr);
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	vkDestroyDevice(device, nullptr);
		
	free(images);
	
	return true;
}
