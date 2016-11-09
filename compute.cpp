#include "compute.h"

#include <stdexcept>
#include <fstream>
#include <iostream>

#include "types.h"

//Compute::Compute(VkExtent3D inputExtent, VkPhysicalDeviceMemoryProperties& props, std::function<uint32_t(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags properties, uint32_t& allocSize)> memTypeIndexCallback)
Compute::Compute(VkExtent3D inputExtent, VkPhysicalDeviceMemoryProperties& props)
: Commands(props),
  extent(inputExtent),
  uniformBufferSize(sizeof(float)),
  storageBufferSize(sizeof(float) * inputExtent.width * inputExtent.height)
{
	//GetMemoryTypeIndexCallback = memTypeIndexCallback;
}

Compute::~Compute()
{
	
}

void Compute::Init(VkDevice& device)
{	
	//size = sizeof(float)*extent.width*extent.height;
	
	VkFormat format = VK_FORMAT_R32_SFLOAT;
	
	//assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);
	
	VkMemoryPropertyFlags properties; //= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	VkBufferUsageFlags usage; //= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		
	//SetupImage(device, image, extent, format, imageMemory, properties, usage);
	
	VkDeviceSize size = sizeof(float);
	
	properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	
    SetupBuffer(device, uniformBuffer, uniformBufferMemory, size, properties, usage);
	
	size = extent.width*extent.height*sizeof(float);
	
	properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	SetupBuffer(device, storageBuffer, storageBufferMemory, size, properties, usage);
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
	
	//vkDestroyPipeline(device, pipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyShaderModule(device, shaderModule, nullptr);
}

void Compute::SetupBuffers(VkDevice& device)
{
	
}

void Compute::SetupQueue(VkDevice& device)
{
	/*VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = NULL;
	queueCreateInfo.queueFamilyIndex = queueIndex;
	queueCreateInfo.queueCount = 1;
	
	vkGetDeviceQueue(device, queueFamilyId, 0, &queue);*/
	
	//uint32_t computeQueueFamilyId = 0;
	
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
	
	/*VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.bindingCount = 1;
	layoutCreateInfo.pBindings = &layoutBindings[uniformIndex];

	VkResult result = vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayouts[uniformIndex]);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Descriptor set layout creation failed");
	}
	
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.bindingCount = 1;
	layoutCreateInfo.pBindings = &layoutBindings[storageIndex];

	result = vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayouts[storageIndex]);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Descriptor set layout creation failed");
	}*/
	
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
	
	/*VkComputePipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stage = shaderStageCreateInfo;
	pipelineCreateInfo.layout = pipelineLayout;
	
	result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
		
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = computeQueueFamilyId;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	result = vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &commandPool);

	VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
    cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocInfo.commandPool = commandPool;
    cmdBufAllocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &cmdBufAllocInfo, &commandBuffer);

	VkFence fence;
	
	VkFenceCreateInfo fenceCreateInfo;
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);*/
}

VkCommandBuffer* Compute::SetupCommandBuffer(VkDevice& device, uint32_t graphicsQueueFamilyId, uint32_t computeQueueFamilyId)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Compute command buffer beign failed");
	}
	
	/*memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.buffer = storageBuffer;
	memoryBarrier.size = storageBufferSize;
	memoryBarrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	memoryBarrier.srcQueueFamilyIndex = graphicsQueueFamilyId;
	memoryBarrier.dstQueueFamilyIndex = computeQueueFamilyId;
	
	vkCmdPipelineBarrier(commandBuffer,
						 VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
						 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						 0,
					     0, nullptr,
						 1, &memoryBarrier,
						 0, nullptr);*/

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, 0);
	vkCmdDispatch(commandBuffer, extent.width, extent.height, 1);

	/*memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	memoryBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	memoryBarrier.buffer = storageBuffer;
	memoryBarrier.size = storageBufferSize;
	memoryBarrier.srcQueueFamilyIndex = computeQueueFamilyId;
	memoryBarrier.dstQueueFamilyIndex = graphicsQueueFamilyId;
	
	vkCmdPipelineBarrier(commandBuffer,
						 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
						 VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
						 0,
						 0, nullptr,
						 1, &memoryBarrier,
						 0, nullptr);*/

	vkEndCommandBuffer(commandBuffer);
	
	return &commandBuffer;
}

/*uint32_t Compute::GetMemoryTypeIndex(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags propFlags, uint32_t& allocSize)
{
	VkMemoryRequirements memRequirements;
	
	vkGetImageMemoryRequirements(device, buffer, &memRequirements);
		
	uint32_t memTypeIndex = InvalidIndex;
	
	for (uint32_t i = 0; i < props.memoryTypeCount; ++i)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(props.memoryTypes[i].propertyFlags & propFlags) == propFlags)
		{
			memTypeIndex = i;
		}
	}
	
	if (memTypeIndex == InvalidIndex)
	{
		throw std::runtime_error("Memory property combination not supported");
	}
	
	allocSize = memRequirements.size;
	
	return memTypeIndex;
}*/