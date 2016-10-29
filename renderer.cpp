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

Renderer::Renderer(VkExtent2D& extent, VkPhysicalDeviceMemoryProperties& memProps)
:	imageExtent(extent),
	memProperties(memProps)
{
}

Renderer::~Renderer()
{
	
}

bool Renderer::Init(VkDevice& device, const VkFormat& surfaceFormat, const VkImageView* imageView, uint32_t queueFamilyId)
{
	bool result = false;
	
	size_t vertexShaderFileSize;
	char* vertexShader;
	size_t fragmentShaderFileSize;
	char* fragmentShader;
	
	//SetupClientSideVertexBuffer(device);
	
	SetupServerSideVertexBuffer(device);
	
	SetupShaderParameters();
	
	try
	{
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
	}
	catch(std::exception e)
	{
		//std::cout << e.message() << std::endl;
		std::cout << "exception" << std::endl;
	}
	
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
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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
	//pipelineLayoutInfo.setLayoutCount = 0;
	//pipelineLayoutInfo.pSetLayouts = nullptr;
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
	
	VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = imageView;
    framebufferInfo.width = imageExtent.width;
    framebufferInfo.height = imageExtent.height;
    framebufferInfo.layers = 1;

    result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Framebuffer creation failed" << std::cout;
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
	allocInfo.commandBufferCount = 1;

	result = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Command buffer creation failed" << std::cout;
	}
	
	return true;
}

bool Renderer::Destroy(VkDevice& device)
{
	vkFreeCommandBuffers(device, commandPool, 1, &transferCommandBuffer);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	
	vkFreeMemory(device, vertexBufferMemory, nullptr);
	vkFreeMemory(device, transferBufferMemory, nullptr);
	
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkDestroyBuffer(device, transferBuffer, nullptr);
	
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyFramebuffer(device, framebuffer, nullptr);
	vkDestroyPipeline(device, pipeline, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyShaderModule(device, vertexShaderModule, nullptr);
	vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
	
	return true;
}

VkCommandBuffer* Renderer::ConstructFrame()
{
	bool result = false;
	
	VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
	
	VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
	
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffer;
	renderPassBeginInfo.renderArea.offset = {0, 0};
	renderPassBeginInfo.renderArea.extent = imageExtent;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;
	
	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		
	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(commandBuffer, numVertices, 1, 0, 0);
	
	vkCmdEndRenderPass(commandBuffer);
	
	result = vkEndCommandBuffer(commandBuffer);
	
	if (result != VK_SUCCESS)
	{
		std::cout << "Command buffer end failed" << std::cout;
	}
	
	return &commandBuffer;
}

bool Renderer::SetupShaderParameters()
{
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(float[3]) * 2;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	
	attributeDescriptions = static_cast<VkVertexInputAttributeDescription*>(malloc(sizeof(VkVertexInputAttributeDescription)*numAttrDesc));
	
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = 0;
	
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = sizeof(float[3]);
	
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
	bufferInfo.size = sizeof(float[3]) * numVertices * 2;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	std::cout << bufferInfo.size << std::endl;
	
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
	VkDeviceSize bufferSize = sizeof(vertexInfo);
	
	VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &transferBuffer);
	
	if (result != VK_SUCCESS)
	{
        throw std::runtime_error("Transfer buffer creation failed");
    }

    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	
	uint32_t allocSize;
	uint32_t transferMemTypeIndex = GetMemoryTypeIndex(device, transferBuffer, properties, allocSize);
	
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = allocSize;
    allocInfo.memoryTypeIndex = transferMemTypeIndex;
	
    result = vkAllocateMemory(device, &allocInfo, nullptr, &transferBufferMemory);
	
	if (result != VK_SUCCESS)
	{
        throw std::runtime_error("Buffer allocation failed");
    }

    vkBindBufferMemory(device, transferBuffer, transferBufferMemory, 0);

	void* data;
    vkMapMemory(device, transferBufferMemory, 0, bufferSize, 0, &data);	
    memcpy(data, vertexInfo, (size_t) bufferSize);
    vkUnmapMemory(device, transferBufferMemory);
	
	bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer);
	
	if (result != VK_SUCCESS)
	{
        throw std::runtime_error("Vertex buffer creation failed");
    }
	
	properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	
	uint32_t bufferMemTypeIndex = GetMemoryTypeIndex(device, vertexBuffer, properties, allocSize);
	
    allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = allocSize;
    allocInfo.memoryTypeIndex = bufferMemTypeIndex;
	
    result = vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory);
	
	if (result != VK_SUCCESS)
	{
        throw std::runtime_error("Vertex buffer allocation failed");
    }

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

	return true;
}

VkCommandBuffer* Renderer::TransferBuffer(VkDevice& device)
{
	VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocInfo, &transferCommandBuffer);
	
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(transferCommandBuffer, &beginInfo);
	
	VkBufferCopy copyRegion = {};
	//copyRegion.srcOffset = 0;
	//copyRegion.dstOffset = 0;
	copyRegion.size = sizeof(vertexInfo);
	vkCmdCopyBuffer(transferCommandBuffer, transferBuffer, vertexBuffer, 1, &copyRegion);
	
	vkEndCommandBuffer(transferCommandBuffer);
	
	return &transferCommandBuffer;
}
