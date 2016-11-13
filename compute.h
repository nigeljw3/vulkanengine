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
	Compute(const VkExtent3D& extent, const VkPhysicalDeviceMemoryProperties& props);
	~Compute() = default;
	
	///@note Only define copy and move constructors and assignment operators if they are actually required
    Compute(const Compute&) = delete;
	Compute(Compute&&) = delete;
	Compute& operator=(const Compute&) = delete;
	Compute& operator=(Compute &&) = delete;
	
	void Init(VkDevice& device);
	void Destroy(VkDevice& device);
	void SetupQueue(VkDevice& device, uint32_t queueFamilyId);
	VkCommandBuffer* SetupCommandBuffer(VkDevice& device, uint32_t queueFamilyId);
	
	inline VkBuffer& GetStorageBuffer() { return storageBuffer; }
	inline VkBuffer& GetNormalBuffer() { return normalBuffer; }
	
	void PrintResults(VkDevice& device);
	void UpdateWave(VkDevice& device);
	
private:
	struct Wave
	{
		float lambda;
		float k;
		float omega;
		float dx;
		float amplitude;
	} wave = {};

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
	
	const VkExtent3D& extent;
	
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout = {};
	VkDescriptorPool descriptorPool = {};
	
	uint32_t uniformBufferSize;
	uint32_t storageBufferSize;
	uint32_t normalBufferSize;
	
	///@todo Use the fence
	VkFence fence;
	
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

};

#endif