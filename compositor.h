/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef compositor_h
#define compositor_h

#include "renderer.h"
#include "compute.h"

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
			  uint32_t presentQueueIndex,
			  uint32_t computeQueueIndex);
			  
	void Loop();
	bool Destroy(VkDevice& device);
	bool CheckExtensionsSupport(uint32_t extensionCount, VkExtensionProperties* extensions);
	VkFormat GetSurfaceFormat() { return surfaceFormat; }
	VkPresentModeKHR GetPresentMode() { return presentMode; }
	
	bool Draw(VkDevice& device);
	
	static uint32_t GetMemoryTypeIndex(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags properties, uint32_t& allocSize);
	
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
	
	VkPhysicalDeviceMemoryProperties memProperties;
	
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
	
	const VkExtent3D grid { 10, 10, 1 };
};

#endif