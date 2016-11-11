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

#include "compute.h"

#include <stdexcept>
#include <fstream>
#include <iostream>

#include "types.h"

//Compute::Compute(VkExtent3D inputExtent, VkPhysicalDeviceMemoryProperties& props, std::function<uint32_t(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags properties, uint32_t& allocSize)> memTypeIndexCallback)
Compute::Compute(VkExtent3D inputExtent, VkPhysicalDeviceMemoryProperties& props)
: Commands(props),
  extent(inputExtent),
  uniformBufferSize(sizeof(float) * numWaveComponents),
  storageBufferSize(sizeof(float) * inputExtent.width * inputExtent.height),	
  startTime(std::chrono::high_resolution_clock::now())
{
	//GetMemoryTypeIndexCallback = memTypeIndexCallback;
	
	std::cout << "ubo size: " << uniformBufferSize << std::endl;
}

Compute::~Compute()
{
	
}

void Compute::Init(VkDevice& device)
{	
	//VkFormat format = VK_FORMAT_R32_SFLOAT;
	
	//assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);
	
	VkMemoryPropertyFlags properties; //= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	VkBufferUsageFlags usage; //= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		
	//SetupImage(device, image, extent, format, imageMemory, properties, usage);
	
	properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	
    SetupBuffer(device, uniformBuffer, uniformBufferMemory, uniformBufferSize, properties, usage);
	
	properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	//properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	SetupBuffer(device, storageBuffer, storageBufferMemory, storageBufferSize, properties, usage);
}

void Compute::Destroy(VkDevice& device)
{
	//vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	
	//vkDestroyCommandPool(device, commandPool, nullptr);
	
	//vkFreeMemory(device, imageMemory, nullptr);
	
	//vkDestroyImage(device, image, nullptr);
	
	vkFreeMemory(device, uniformBufferMemory, nullptr);
	vkDestroyBuffer(device, uniformBuffer, nullptr);

	vkFreeMemory(device, storageBufferMemory, nullptr);
	vkDestroyBuffer(device, storageBuffer, nullptr);
	
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	
	vkDestroyPipeline(device, pipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyShaderModule(device, shaderModule, nullptr);
	
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	
	vkDestroyCommandPool(device, commandPool, nullptr);
	
	vkDestroyFence(device, fence, nullptr);
}

/*void Compute::SetupBuffers(VkDevice& device)
{
	
}*/

void Compute::SetupQueue(VkDevice& device, uint32_t queueFamilyId)
{	
	uint32_t uniformIndex = 0;
	uint32_t storageIndex = 1;
	
	VkDescriptorSetLayoutBinding layoutBindings[2];

	layoutBindings[uniformIndex].binding = 0;
	layoutBindings[uniformIndex].descriptorCount = 1;
	layoutBindings[uniformIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings[uniformIndex].pImmutableSamplers = nullptr;
	layoutBindings[uniformIndex].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	
	layoutBindings[storageIndex].binding = 1;
	layoutBindings[storageIndex].descriptorCount = 1;
	layoutBindings[storageIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	layoutBindings[storageIndex].pImmutableSamplers = nullptr;
	layoutBindings[storageIndex].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	
	VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.bindingCount = 2;
	layoutCreateInfo.pBindings = layoutBindings;

	VkResult result = vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayout);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Descriptor set layout creation failed");
	}
	
	VkDescriptorPoolSize poolSizes[2];
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[1].descriptorCount = 1;
	
	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = 1;
	poolCreateInfo.poolSizeCount = 2;
	poolCreateInfo.pPoolSizes = poolSizes;
		
	result = vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &descriptorPool);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Descriptor pool creation failed");
	}
	
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;

	result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Descriptor set allocation failed");
	}
	
	VkDescriptorBufferInfo bufferInfo[2];
	bufferInfo[uniformIndex].buffer = uniformBuffer;
	bufferInfo[uniformIndex].offset = 0;
	bufferInfo[uniformIndex].range = uniformBufferSize;

	bufferInfo[storageIndex].buffer = storageBuffer;
	bufferInfo[storageIndex].offset = 0;
	bufferInfo[storageIndex].range = storageBufferSize;
	
	VkWriteDescriptorSet descriptorWrites[] = { {}, {} };
	descriptorWrites[uniformIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[uniformIndex].dstSet = descriptorSet;
	descriptorWrites[uniformIndex].dstBinding = 0;
	descriptorWrites[uniformIndex].dstArrayElement = 0;
	descriptorWrites[uniformIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[uniformIndex].descriptorCount = 1;
	descriptorWrites[uniformIndex].pBufferInfo = &bufferInfo[uniformIndex];
	
	descriptorWrites[storageIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[storageIndex].dstSet = descriptorSet;
	descriptorWrites[storageIndex].dstBinding = 1;
	descriptorWrites[storageIndex].dstArrayElement = 0;
	descriptorWrites[storageIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrites[storageIndex].descriptorCount = 1;
	descriptorWrites[storageIndex].pBufferInfo = &bufferInfo[storageIndex];
	
	vkUpdateDescriptorSets(device, 2, descriptorWrites, 0, nullptr);
		
	std::ifstream file("comp.spv", std::ios::ate | std::ios::binary);
	
	if (!file.is_open())
	{
		std::cout << "Failed to open shader file" << std::endl;
	}
	
	size_t shaderFileSize = static_cast<size_t>(file.tellg());
	char* shader = static_cast<char*>(malloc(sizeof(char)*shaderFileSize));
	
	file.seekg(0);
	file.read(shader, shaderFileSize);
	file.close();
	
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = shaderFileSize;
	shaderCreateInfo.pCode = (uint32_t*) shader;
	
	vkCreateShaderModule(device, &shaderCreateInfo, nullptr, &shaderModule);
		
	free(shader);
	
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shaderStageCreateInfo.module = shaderModule;
	shaderStageCreateInfo.pName = "main";
	
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	
	result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Pipeline layout creation failed" << std::cout;
	}
	
	VkComputePipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stage = shaderStageCreateInfo;
	pipelineCreateInfo.layout = pipelineLayout;
	
	result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
		
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = queueFamilyId;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	result = vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &commandPool);

	VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
    cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocInfo.commandPool = commandPool;
    cmdBufAllocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &cmdBufAllocInfo, &commandBuffer);

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
}

VkCommandBuffer* Compute::SetupCommandBuffer(VkDevice& device, uint32_t queueFamilyId)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Compute command buffer beign failed");
	}
	
	memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	memoryBarrier.buffer = storageBuffer;
	memoryBarrier.size = storageBufferSize;
	memoryBarrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	memoryBarrier.srcQueueFamilyIndex = queueFamilyId;
	memoryBarrier.dstQueueFamilyIndex = queueFamilyId;
	
	vkCmdPipelineBarrier(commandBuffer,
						 VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
						 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						 0,
					     0, nullptr,
						 1, &memoryBarrier,
						 0, nullptr);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, 0);
	vkCmdDispatch(commandBuffer, extent.width, extent.height, 1);

	memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	memoryBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	memoryBarrier.buffer = storageBuffer;
	memoryBarrier.size = storageBufferSize;
	memoryBarrier.srcQueueFamilyIndex = queueFamilyId;
	memoryBarrier.dstQueueFamilyIndex = queueFamilyId;
	
	vkCmdPipelineBarrier(commandBuffer,
						 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						 VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
						 0,
						 0, nullptr,
						 1, &memoryBarrier,
						 0, nullptr);

	vkEndCommandBuffer(commandBuffer);
	
	return &commandBuffer;
}

void Compute::UpdateWave(VkDevice& device)
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;
	
	float pi = 3.14;
	float lambda = 4.0;
	float k = 2.0*pi/lambda;
	float omega = 2.0;
	float dx = 0.5;
	float amplitude = 0.5;
	
	void* data;
    vkMapMemory(device, uniformBufferMemory, 0, uniformBufferSize, 0, &data);
	
	float* wave = static_cast<float*>(data);
	
	wave[0] = time;
	wave[1] = dx;
	wave[2] = k;
	wave[3] = omega;
	wave[4] = amplitude;
	
	vkUnmapMemory(device, uniformBufferMemory);
}

void Compute::PrintResults(VkDevice& device)
{
	void* data;
    vkMapMemory(device, storageBufferMemory, 0, storageBufferSize, 0, &data);
	
	for(uint32_t i = 0; i < extent.width; ++i)
	{
		for(uint32_t j = 0; j < extent.height; ++j)
		{
			std::cout << static_cast<float*>(data)[i*extent.width + j] << ", ";
		}
		
		std::cout << std::endl;
	}
	
    vkUnmapMemory(device, storageBufferMemory);
}