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

#ifndef compute_h
#define compute_h

#include "commands.h"

#include <vulkan/vulkan.h>
#include <chrono>

namespace vfsme
{

class Compute : Commands
{
public:
	Compute(VkExtent3D extent, VkPhysicalDeviceMemoryProperties& props);
	~Compute();
	
	///@note Only define copy and assignment operators if they are actually required
	Compute& operator=(const Compute&) = delete;
    Compute(const Compute&) = delete;
	
	void Init(VkDevice& device);
	void Destroy(VkDevice& device);
	void SetupQueue(VkDevice& device, uint32_t queueFamilyId);
	VkCommandBuffer* SetupCommandBuffer(VkDevice& device, uint32_t queueFamilyId);
	VkBuffer& GetStorageBuffer() { return storageBuffer; }
	VkBuffer& GetNormalBuffer() { return normalBuffer; }
	void PrintResults(VkDevice& device);
	void UpdateWave(VkDevice& device);
	
private:
	const uint32_t numWaveComponents = 5;

	VkShaderModule shaderModule;
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	
	VkCommandBuffer commandBuffer;
	VkBufferMemoryBarrier memoryBarriers[2] = { {}, {} };
	
	//VkImage image;
	VkBuffer uniformBuffer;
	VkBuffer storageBuffer;
	VkBuffer normalBuffer;
	
	VkDeviceMemory uniformBufferMemory;
	VkDeviceMemory storageBufferMemory;
	VkDeviceMemory normalBufferMemory;
	//VkDeviceMemory imageMemory;
	
	VkCommandPool commandPool;
	
	VkExtent3D extent;
	
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout = {};
	VkDescriptorPool descriptorPool = {};
	
	uint32_t uniformBufferSize;
	uint32_t storageBufferSize;
	uint32_t normalBufferSize;
	
	VkFence fence;
	
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

};

#endif