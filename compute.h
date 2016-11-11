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

#ifndef compute_h
#define compute_h

#include "commands.h"

#include <vulkan/vulkan.h>
#include <functional>
#include <chrono>

class Compute : Commands
{
public:
	//Compute(VkExtent3D extent, VkPhysicalDeviceMemoryProperties& props, std::function<uint32_t(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags properties, uint32_t& allocSize)> memTypeIndexCallback);
	Compute(VkExtent3D extent, VkPhysicalDeviceMemoryProperties& props);
	~Compute();
	
	void Init(VkDevice& device);
	void Destroy(VkDevice& device);
	void SetupQueue(VkDevice& device, uint32_t queueFamilyId);
	VkCommandBuffer* SetupCommandBuffer(VkDevice& device, uint32_t queueFamilyId);
	
	VkBuffer& GetStorageBuffer() { return storageBuffer; }
	
	void PrintResults(VkDevice& device);
	
	void UpdateWave(VkDevice& device);
	
private:
	const uint32_t numWaveComponents = 5;

	//uint32_t GetMemoryTypeIndex(VkDevice& device, VkImage& image, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags propFlags, uint32_t& allocSize);

	//void SetupBuffers(VkDevice& device);
	//void SetupCommandBuffer();
	//std::function<uint32_t(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags properties, uint32_t& allocSize)> GetMemoryTypeIndexCallback;
	
	VkShaderModule shaderModule;
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	
	VkCommandBuffer commandBuffer;
	VkBufferMemoryBarrier memoryBarrier = {};
	//VkDeviceMemory memory;
	
	//VkImage image;
	VkBuffer uniformBuffer;
	VkBuffer storageBuffer;
	
	VkDeviceMemory uniformBufferMemory;
	VkDeviceMemory storageBufferMemory;
	//VkDeviceMemory imageMemory;
	
	VkCommandPool commandPool;
	
	VkExtent3D extent;
	
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	
	//uint32_t size;
	
	//VkPhysicalDeviceMemoryProperties memProperties;
	
	uint32_t uniformBufferSize;
	uint32_t storageBufferSize;
	
	VkFence fence;
	
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

#endif