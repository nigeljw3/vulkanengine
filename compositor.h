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

#ifndef compositor_h
#define compositor_h

#include <vulkan/vulkan.h>

#include "renderer.h"
#include "compute.h"

namespace vfsme
{

class Compositor
{
public:
	explicit Compositor(const VkPhysicalDeviceMemoryProperties& memProperties);
	~Compositor();
	
	///@note Only define copy and move constructors and assignment operators if they are actually required
    Compositor(const Compositor&) = delete;
	Compositor(Compositor&&) = delete;
	Compositor& operator=(const Compositor&) = delete;
	Compositor& operator=(Compositor &&) = delete;
	
	bool Init(VkDevice& device,
			  VkSurfaceKHR& surface,
			  uint32_t width,
			  uint32_t height,
			  uint32_t queueFamilyId,
			  uint32_t graphicsQueueIndex,
			  uint32_t presentQueueIndex,
			  uint32_t computeQueueIndex);
			  
	void Loop();
	bool Destroy(VkDevice& device);
	//bool CheckExtensionsSupport(uint32_t extensionCount, const VkExtensionProperties* extensions) const;
	
	inline VkFormat GetSurfaceFormat() const { return surfaceFormat; }
	inline VkPresentModeKHR GetPresentMode() const { return presentMode; }
	
	bool Draw(VkDevice& device);
	
private:
	void PrintCapabilities();
	
	uint32_t imageCount;
	uint32_t drawIndex;

	Renderer* graphicsEngine;
	Compute* computer;
	
	VkSurfaceCapabilitiesKHR capabilities;
	VkImage* images;
	VkSemaphore waitSemaphore;
	VkSemaphore signalSemaphore;
	VkSwapchainKHR swapChain;
	VkImageView* imageViews;
	
	const VkPhysicalDeviceMemoryProperties& memProperties;
	
	VkQueue presentQueue;
	VkQueue graphicsQueue;
	VkQueue computeQueue;
	
	VkCommandBuffer* drawCommandBuffer;
	VkCommandBuffer* transferCommandBuffer;
	VkCommandBuffer* computeCommandBuffer;
	
	const VkFormat surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
	const VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	
	VkImage textureImage;
	VkImageMemoryBarrier barrier;
	
	///@note Grid dimensions are currently restricted by the max X and Y workgroup sizes of the device (32x32 on my hardware)
	/// Needs to be changed to dynamic sizing in a future patchset by either spreading computation to the third workgroup
	/// dimension or across multiple workgroups, or, at the very least, set dynamically to the physical device limits.
	const VkExtent3D grid { 32, 32, 1 };
};

};

#endif