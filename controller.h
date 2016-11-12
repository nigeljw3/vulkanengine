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

#ifndef controller_h
#define controller_h

#include <vulkan/vulkan.h>

#include "shared.h"

namespace vfsme
{

class Controller
{
public:
	Controller();
	~Controller();
	
	///@note Only define copy and assignment operators if they are actually required
	Controller& operator=(const Controller&) = delete;
    Controller(const Controller&) = delete;

	bool Init();
	bool SetupQueue();
	bool SetupDevice(VkSurfaceKHR& surface);
	bool Destroy();
	
	VkPhysicalDevice& GetPhysicalDevice() { return physicalDevice; }
	VkDevice& GetDevice() { return device; }
	VkInstance& GetInstance() { return instance; }
	bool Configure(VkSurfaceKHR& surface);
	bool PresentModeSupported(VkSurfaceKHR& surface, VkPresentModeKHR presentMode);
	bool SurfaceFormatSupported(VkSurfaceKHR& surface, VkFormat surfaceFormat);
	
	VkPhysicalDeviceMemoryProperties& GetMemoryProperties() { return memProperties; }
	
	//VkSurfaceCapabilitiesKHR* GetCapabilities() { return &capabilities; }
	//uint32_t GetQueueFamilyId() { return queueFamilyId; }
	
	uint32_t GetQueueFamilyId() { return queueFamilyId; }
	uint32_t GetGraphicsQueueIndex() { return graphicsQueueIndex; }
	uint32_t GetPresentQueueIndex(){ return presentQueueIndex; }
	uint32_t GetComputeQueueIndex(){ return computeQueueIndex; }
	
	bool CheckFormatPropertyType(VkFormat format, VkFormatFeatureFlagBits flags);
	
private:
	void PrintCapabilities();

	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	VkSurfaceCapabilitiesKHR capabilities;
	VkPhysicalDeviceMemoryProperties memProperties;
	float* queuePriorities;
	
	const uint32_t queueCount = 3;
	uint32_t queueFamilyId = InvalidIndex;
	const uint32_t graphicsQueueIndex = 0;
	const uint32_t presentQueueIndex = 1;
	const uint32_t computeQueueIndex = 2;
};

};

#endif