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

#include "renderer.h"
#include "types.h"

#ifdef __STDC_LIB_EXT1__ 
#define __STDC_WANT_LIB_EXT1__ 1
#endif

#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include <chrono>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp> //"glm/gtx/string_cast.hpp"

Renderer::Renderer(VkExtent2D& extent, VkPhysicalDeviceMemoryProperties& memProps)
:	imageExtent(extent),
	memProperties(memProps)
{	
	numVerts = numVertsX * numVertsY;
	numPrims = (numVertsX - 1) * (numVertsY - 1) * 2;
	vertexInfoSize = sizeof(float) * numComponents * numVerts * 2;
	numIndices = numPrims * numComponents;
	indicesBufferSize = sizeof(uint16_t) * numIndices;
	
	vertexInfo = static_cast<float*>(malloc(vertexInfoSize));
	indices = static_cast<uint16_t*>(malloc(indicesBufferSize));
	framebuffers = static_cast<VkFramebuffer*>(malloc(sizeof(VkFramebuffer)*numFBOs));
	drawCommandBuffers = static_cast<VkCommandBuffer*>(malloc(sizeof(VkCommandBuffer)*numDrawCmdBuffers));
	attributeDescriptions = static_cast<VkVertexInputAttributeDescription*>(malloc(sizeof(VkVertexInputAttributeDescription)*numAttrDesc));
}

Renderer::~Renderer()
{
	free(vertexInfo);
	free(indices);
	free(framebuffers);
	free(drawCommandBuffers);
	free(attributeDescriptions);
}

bool Renderer::Init(VkDevice& device, const VkFormat& surfaceFormat, const VkImageView* imageViews, uint32_t queueFamilyId)
{
	bool result = false;
	
	size_t vertexShaderFileSize;
	char* vertexShader;
	size_t fragmentShaderFileSize;
	char* fragmentShader;
	
	float delta = 0.5f;
	uint32_t index = 0;
	float xStartPos = - ((numVertsX - 1) * delta) / 2;
	float yStartPos = - ((numVertsY - 1) * delta) / 2;
	float xPos = xStartPos;
	float yPos = yStartPos;
	float zPos = 0.0f;
	
	for (uint32_t i = 0; i < numVertsY; ++i)
	{
		for (uint32_t j = 0; j < numVertsX; ++j)
		{
			vertexInfo[index] = xPos;
			vertexInfo[index + 1] = zPos;
			vertexInfo[index + 2] = yPos;
			
			vertexInfo[index + 3] = 0.0f;
			vertexInfo[index + 4] = 0.0f;
			vertexInfo[index + 5] = 1.0f;
						
			index += 6;
			xPos += delta;
		}
		
		xPos = xStartPos;
		yPos += delta;
	}
		
	index = 0;
	
	for (uint32_t i = 0; i < numVertsY - 1; ++i)
	{
		for (uint32_t j = 0; j < numVertsX - 1; ++j)
		{			
			indices[index] = i * numVertsX + j;
			indices[index + 1] = indices[index] + 1;
			indices[index + 2] = indices[index] + numVertsX;
			
			indices[index + 3] = indices[index + 2];
			indices[index + 4] = indices[index + 3] + 1;
			indices[index + 5] = indices[index + 1];
			
			index += 6;
		}
	}
	
	SetupServerSideVertexBuffer(device);
	SetupIndexBuffer(device);
	SetupUniformBuffer(device);
	SetupShaderParameters(device);

	std::ifstream file("vert.spv", std::ios::ate | std::ios::binary);
	
	if (!file.is_open())
	{
		std::cout << "Failed to open shader file" << std::endl;
	}
	
	vertexShaderFileSize = static_cast<size_t>(file.tellg());
	vertexShader = static_cast<char*>(malloc(sizeof(char)*vertexShaderFileSize));
	
	file.seekg(0);
	file.read(vertexShader, vertexShaderFileSize);
	file.close();
	
	file.open("frag.spv", std::ios::ate | std::ios::binary);
	
	if (!file.is_open())
	{
		std::cout << "Failed to open shader file" << std::endl;
	}
	
	fragmentShaderFileSize = static_cast<size_t>(file.tellg());
	fragmentShader = static_cast<char*>(malloc(sizeof(char)*fragmentShaderFileSize));
	
	file.seekg(0);
	file.read(fragmentShader, fragmentShaderFileSize);
	file.close();
	
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = vertexShaderFileSize;
	shaderCreateInfo.pCode = (uint32_t*) vertexShader;
	
	vkCreateShaderModule(device, &shaderCreateInfo, nullptr, &vertexShaderModule);
		
	shaderCreateInfo.codeSize = fragmentShaderFileSize;
	shaderCreateInfo.pCode = (uint32_t*) fragmentShader;
	
	vkCreateShaderModule(device, &shaderCreateInfo, nullptr, &fragmentShaderModule);
	
	free(fragmentShader);
	free(vertexShader);
	
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertexShaderModule;
	vertShaderStageInfo.pName = "main";
	
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragmentShaderModule;
	fragShaderStageInfo.pName = "main";
	
	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = numAttrDesc;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;
		
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) imageExtent.width;
	viewport.height = (float) imageExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = imageExtent;
	
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	//rasterizer.depthBiasConstantFactor = 0.0f;
	//rasterizer.depthBiasClamp = 0.0f;
	//rasterizer.depthBiasSlopeFactor = 0.0f;
	
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	//multisampling.minSampleShading = 1.0f;
	//multisampling.pSampleMask = nullptr;
	//multisampling.alphaToCoverageEnable = VK_FALSE;
	//multisampling.alphaToOneEnable = VK_FALSE;
	
	//VkPipelineDepthStencilStateCreateInfo
	
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.blendEnable = VK_TRUE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	//colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	//colorBlending.blendConstants[0] = 0.0f;
	//colorBlending.blendConstants[1] = 0.0f;
	//colorBlending.blendConstants[2] = 0.0f;
	//colorBlending.blendConstants[3] = 0.0f;
	
	//VkDynamicState dynamicStates[] = {
	//	VK_DYNAMIC_STATE_VIEWPORT,
	//	VK_DYNAMIC_STATE_LINE_WIDTH
	//};

	//VkPipelineDynamicStateCreateInfo dynamicState = {};
	//dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	//dynamicState.dynamicStateCount = 2;
	//dynamicState.pDynamicStates = dynamicStates;
	
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	//pipelineLayoutInfo.pushConstantRangeCount = 0;
	//pipelineLayoutInfo.pPushConstantRanges = 0;
	
	result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Pipeline layout creation failed" << std::cout;
	}
	
	VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = surfaceFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subPass = {};
	subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subPass.colorAttachmentCount = 1;
	subPass.pColorAttachments = &colorAttachmentRef;
	
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subPass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Render pass creation failed" << std::cout;
	}
	
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	//pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	//pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;
	
	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Pipeline creation failed" << std::cout;
	}

	for (uint32_t i = 0; i < numFBOs; ++i)
	{
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &imageViews[i];
		framebufferInfo.width = imageExtent.width;
		framebufferInfo.height = imageExtent.height;
		framebufferInfo.layers = 1;

		result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]);
		
		if (result != VK_SUCCESS)
		{
			std::cout << "Framebuffer creation failed" << std::cout;
		}
	}

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyId;
	poolInfo.flags = 0;
	
	vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Command pool creation failed" << std::cout;
	}
	
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = numDrawCmdBuffers;

	result = vkAllocateCommandBuffers(device, &allocInfo, drawCommandBuffers);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Command buffer creation failed" << std::cout;
	}
	
	SetupDynamicTransfer(device);
			
	return true;
}

bool Renderer::Destroy(VkDevice& device)
{
	vkFreeCommandBuffers(device, commandPool, 1, &staticTransferCommandBuffer);
	vkFreeCommandBuffers(device, commandPool, 1, &dynamicTransferCommandBuffer);
	vkFreeCommandBuffers(device, commandPool, numDrawCmdBuffers, drawCommandBuffers);
	
	vkDestroyCommandPool(device, commandPool, nullptr);
	
	vkFreeMemory(device, vertexBufferMemory, nullptr);
	vkFreeMemory(device, vertexTransferBufferMemory, nullptr);
	
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkDestroyBuffer(device, vertexTransferBuffer, nullptr);
	
	vkFreeMemory(device, indexBufferMemory, nullptr);
	vkFreeMemory(device, indexTransferBufferMemory, nullptr);
	
	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkDestroyBuffer(device, indexTransferBuffer, nullptr);
	
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	
	vkDestroyBuffer(device, uniformTransferBuffer, nullptr);
	vkFreeMemory(device, uniformTransferBufferMemory, nullptr);
	vkDestroyBuffer(device, uniformBuffer, nullptr);
	vkFreeMemory(device, uniformBufferMemory, nullptr);
	
	for (uint32_t i = 0; i < numFBOs; ++i)
	{
		vkDestroyFramebuffer(device, framebuffers[i], nullptr);
	}
	
	vkDestroyPipeline(device, pipeline, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyShaderModule(device, vertexShaderModule, nullptr);
	vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
	
	return true;
}

void Renderer::ConstructFrames()
{
	bool result = false;
	
	VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr;  
	
	VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
	
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	
	renderPassBeginInfo.renderArea.offset = {0, 0};
	renderPassBeginInfo.renderArea.extent = imageExtent;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;
	
	VkDeviceSize offsets[] = {0};
	
	assert(numDrawCmdBuffers == numFBOs);
	
	for (uint32_t i = 0; i < numDrawCmdBuffers; ++i)
	{
		renderPassBeginInfo.framebuffer = framebuffers[i];
	
		vkBeginCommandBuffer(drawCommandBuffers[i], &beginInfo);
		vkCmdBeginRenderPass(drawCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, &vertexBuffer, offsets);
		vkCmdBindIndexBuffer(drawCommandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(drawCommandBuffers[i], numIndices, 1, 0, 0, 0);
		vkCmdEndRenderPass(drawCommandBuffers[i]);
		
		result = vkEndCommandBuffer(drawCommandBuffers[i]);
	
		if (result != VK_SUCCESS)
		{
			std::cout << "Command buffer end failed" << std::cout;
		}
	}
}

bool Renderer::SetupShaderParameters(VkDevice& device)
{
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(float[3]) * 2;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = 0;
	
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = sizeof(float[3]);

    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	//uboLayoutBinding.pImmutableSamplers = nullptr;
	
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Descriptor set layout creation failed");
	}
		
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1;
	
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 1;
	
	result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
	
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
	
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = mvpSize;
	
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	//descriptorWrite.pImageInfo = nullptr;
	//descriptorWrite.pTexelBufferView = nullptr;
	
	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		
	return true;
}

uint32_t Renderer::GetMemoryTypeIndex(VkDevice& device, VkBuffer& buffer, VkMemoryPropertyFlags properties, uint32_t& allocSize)
{
	VkMemoryRequirements memRequirements;
	
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
		
	uint32_t memTypeIndex = InvalidIndex;
	
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
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
}

bool Renderer::SetupClientSideVertexBuffer(VkDevice& device)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = vertexInfoSize;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer);
    	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vertex buffer");
    }
	
	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	
	uint32_t allocSize;
	uint32_t bufferMemTypeIndex = GetMemoryTypeIndex(device, vertexBuffer, properties, allocSize);
	
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = allocSize;
	allocInfo.memoryTypeIndex = bufferMemTypeIndex;
	
	std::cout << allocSize << std::endl;
	std::cout << bufferMemTypeIndex << std::endl;
	
	result = vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Vertex buffer memory allocation failed");
	}
	
	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	//std::memcpy_s(data, sizeof(data), vertexInfo, (size_t) bufferInfo.size)
	memcpy(data, vertexInfo, (size_t) bufferInfo.size);
	vkUnmapMemory(device, vertexBufferMemory);
	
	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
	
	return true;
}

bool Renderer::SetupServerSideVertexBuffer(VkDevice& device)
{
	VkDeviceSize size = vertexInfoSize;
	
	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	
    SetupBuffer(device, vertexTransferBuffer, vertexTransferBufferMemory, size, properties, usage);
	
	properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	SetupBuffer(device, vertexBuffer, vertexBufferMemory, size, properties, usage);
	
	void* data;
    vkMapMemory(device, vertexTransferBufferMemory, 0, size, 0, &data);
    memcpy(data, vertexInfo, (size_t) size);
    vkUnmapMemory(device, vertexTransferBufferMemory);

	return true;
}

void Renderer::SetupDynamicTransfer(VkDevice& device)
{
	VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocInfo, &dynamicTransferCommandBuffer);
	
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	vkBeginCommandBuffer(dynamicTransferCommandBuffer, &beginInfo);
	
	VkBufferCopy copyRegion = {};
	copyRegion.size = mvpSize;
	vkCmdCopyBuffer(dynamicTransferCommandBuffer, uniformTransferBuffer, uniformBuffer, 1, &copyRegion);
	
	copyRegion.size = vertexInfoSize;
	vkCmdCopyBuffer(dynamicTransferCommandBuffer, vertexTransferBuffer, vertexBuffer, 1, &copyRegion);
		
	vkEndCommandBuffer(dynamicTransferCommandBuffer);
}

VkCommandBuffer& Renderer::TransferStaticBuffers(VkDevice& device)
{
	VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocInfo, &staticTransferCommandBuffer);
	
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(staticTransferCommandBuffer, &beginInfo);
	
	VkBufferCopy copyRegion = {};
	//copyRegion.srcOffset = 0;
	//copyRegion.dstOffset = 0;
	
	copyRegion.size = indicesBufferSize;
	vkCmdCopyBuffer(staticTransferCommandBuffer, indexTransferBuffer, indexBuffer, 1, &copyRegion);
	
	vkEndCommandBuffer(staticTransferCommandBuffer);
	
	return staticTransferCommandBuffer;
}

VkCommandBuffer& Renderer::TransferDynamicBuffers(VkDevice& device)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;
	
	glm::mat4 model; // = glm::rotate(glm::mat4(), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), imageExtent.width / (float) imageExtent.height, 0.1f, 100.0f);
	proj[1][1] *= -1;
	
	mvp = proj * view * model;
	
	VkDeviceSize size = mvpSize;
	
	void* data;
    vkMapMemory(device, uniformTransferBufferMemory, 0, size, 0, &data);
	memcpy(data, glm::value_ptr(mvp), (size_t) size);
    vkUnmapMemory(device, uniformTransferBufferMemory);
	
	return dynamicTransferCommandBuffer;
}

bool Renderer::SetupIndexBuffer(VkDevice& device)
{
	VkDeviceSize size = indicesBufferSize;
	
	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	
    SetupBuffer(device, indexTransferBuffer, indexTransferBufferMemory, size, properties, usage); 
	
	properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	SetupBuffer(device, indexBuffer, indexBufferMemory, size, properties, usage);
	
	void* data;
    vkMapMemory(device, indexTransferBufferMemory, 0, size, 0, &data);
    memcpy(data, indices, (size_t) size);
    vkUnmapMemory(device, indexTransferBufferMemory);

	return true;
}

bool Renderer::SetupBuffer(VkDevice& device, VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage)
{
	VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
	
	if (result != VK_SUCCESS)
	{
        throw std::runtime_error("Transfer buffer creation failed");
    }

	uint32_t allocSize;
	uint32_t memTypeIndex = GetMemoryTypeIndex(device, buffer, properties, allocSize);
	
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = allocSize;
    allocInfo.memoryTypeIndex = memTypeIndex;
	
    result = vkAllocateMemory(device, &allocInfo, nullptr, &memory);
	
	if (result != VK_SUCCESS)
	{
        throw std::runtime_error("Buffer allocation failed");
    }

    vkBindBufferMemory(device, buffer, memory, 0);
	
	return true;
}

void Renderer::SetupUniformBuffer(VkDevice &device)
{
	VkDeviceSize size = mvpSize;
	
	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	
    SetupBuffer(device, uniformTransferBuffer, uniformTransferBufferMemory, size, properties, usage);
	
	properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	SetupBuffer(device, uniformBuffer, uniformBufferMemory, size, properties, usage);
}

void Renderer::Draw(VkDevice &device)
{
}
