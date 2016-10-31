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