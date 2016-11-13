/**
 * Copyright (C) 2016 Nigel Williams
 *
 * Vulkan Free Surface Modeling Engine (VFSME) is free software:
 * you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "compositor.h"

#include <iostream>
#include <fstream>
#include <limits>
#include <thread>

namespace vfsme
{

Compositor::Compositor(const VkPhysicalDeviceMemoryProperties& memProps)
: imageCount(2),
  drawIndex(0),
  memProperties(memProps)
{
	images = new VkImage[imageCount]();
	imageViews = new VkImageView[imageCount]();
}

Compositor::~Compositor()
{
	delete[] images;
	delete[] imageViews;
}

bool Compositor::Init(VkDevice& device,
					  const VkSurfaceKHR& surface,
					  uint32_t width,
					  uint32_t height,
					  uint32_t queueFamilyId,
					  uint32_t graphicsQueueIndex,
					  uint32_t presentQueueIndex,
					  uint32_t computeQueueIndex)
{	
	VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	
	VkExtent2D screenExtent = {width, height};

    //actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    //actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
	
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
		throw std::runtime_error("Swapchain creation failed");
	}
	
	vkGetDeviceQueue(device, queueFamilyId, graphicsQueueIndex, &graphicsQueue);
	vkGetDeviceQueue(device, queueFamilyId, presentQueueIndex, &presentQueue);
	vkGetDeviceQueue(device, queueFamilyId, computeQueueIndex, &computeQueue);
	
	vkDeviceWaitIdle(device);
	
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images);
	
	for (uint32_t i = 0; i < imageCount; ++i)
	{
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = images[i];
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
		
		result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]);
		
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Image view creation failed");
		}
	}
	
	graphicsEngine = new Renderer(screenExtent, grid, memProperties);
	
	graphicsEngine->Init(device, surfaceFormat, imageViews, queueFamilyId);
	
	VkCommandBuffer& staticTransferCommandBuffer = graphicsEngine->TransferStaticBuffers(device);
	
	VkCommandBuffer& dynamicTransferCommandBuffer = graphicsEngine->TransferDynamicBuffers(device);
	
	computer = new Compute(grid, memProperties);
	
	computer->Init(device);
	
	computer->SetupQueue(device, queueFamilyId);
	
	computeCommandBuffer = computer->SetupCommandBuffer(device, queueFamilyId);	
	
	graphicsEngine->ConstructFrames(computer->GetStorageBuffer(), computer->GetNormalBuffer());
	
	VkCommandBuffer transferCommandBuffers[] = { staticTransferCommandBuffer, dynamicTransferCommandBuffer };
	
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 2;
	submitInfo.pCommandBuffers = transferCommandBuffers;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);
		
	drawCommandBuffer = graphicsEngine->GetFrame(0);
	
	VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	vkCreateSemaphore(device, &semaphoreInfo, nullptr, &waitSemaphore);
	vkCreateSemaphore(device, &semaphoreInfo, nullptr, &signalSemaphore);
	
	return true;
}

bool Compositor::Destroy(VkDevice& device)
{
	vkDestroySemaphore(device, waitSemaphore, nullptr);
	vkDestroySemaphore(device, signalSemaphore, nullptr);	
	
	graphicsEngine->Destroy(device);
	
	computer->Destroy(device);
	
	delete graphicsEngine;
	
	delete computer;
	
	for(uint32_t i = 0; i < imageCount; ++i)
	{
		vkDestroyImageView(device, imageViews[i], nullptr);
	}
	
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	vkDestroyDevice(device, nullptr);
	
	return true;
}

//bool once = true;

bool Compositor::Draw(VkDevice& device)
{
	computer->UpdateWave(device);
	
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = computeCommandBuffer;

	vkQueueSubmit(computeQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(computeQueue);
	
	/*if(once)
	{
		computer->PrintResults(device);
		once = false;
	}*/
	
	transferCommandBuffer = &graphicsEngine->TransferDynamicBuffers(device);
	
	submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = transferCommandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);
	
	drawCommandBuffer = graphicsEngine->GetFrame(drawIndex);
	
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
	submitInfo.pCommandBuffers = drawCommandBuffer;
	
	VkResult result = vkQueueSubmit(presentQueue, 1, &submitInfo, VK_NULL_HANDLE);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Queue submit failed");
	}
	
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &signalSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain;
	presentInfo.pImageIndices = &imageIndex;
	//presentInfo.pResults = nullptr; 
	
	vkQueuePresentKHR(presentQueue, &presentInfo);
	
	vkQueueWaitIdle(presentQueue);
	
	// Increment draw index to use correct image view for next frame
	drawIndex = (drawIndex + 1) % 2;
	
	return true;
}

};
