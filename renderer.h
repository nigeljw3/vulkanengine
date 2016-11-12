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

#ifndef renderer_h
#define renderer_h

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include <functional>

#include "commands.h"

class Renderer : Commands
{
public:
	//Renderer(VkExtent2D& screenExtent, const VkExtent3D& gridDim, VkPhysicalDeviceMemoryProperties& memProperties, std::function<uint32_t(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags properties, uint32_t& allocSize)> memTypeIndexCallback);
	Renderer(VkExtent2D& screenExtent, const VkExtent3D& gridDim, VkPhysicalDeviceMemoryProperties& memProps);
	
	~Renderer();
	
	bool Init(VkDevice& device, const VkFormat& surfaceFormat, const VkImageView* imageViews, uint32_t queueFamilyId);
	void ConstructFrames(VkBuffer& heightBuffer, VkBuffer& normalBuffer);
	
	VkCommandBuffer* GetFrame(uint32_t index)
	{
		return &drawCommandBuffers[index];
	}
	
	VkCommandBuffer& TransferStaticBuffers(VkDevice& device);
	VkCommandBuffer& TransferDynamicBuffers(VkDevice& device);
	bool Destroy(VkDevice& device);
	
	void Draw(VkDevice &device);
	
private:
	bool SetupIndexBuffer(VkDevice& device);
	bool SetupClientSideVertexBuffer(VkDevice& device);
	bool SetupServerSideVertexBuffer(VkDevice& device);
	void SetupUniformBuffer(VkDevice &device);
	void SetupDynamicTransfer(VkDevice &device);
	void SetupStaticTransfer(VkDevice &device);
	
	bool SetupShaderParameters(VkDevice& device);
	
	//auto GetGetMemoryTypeIndexCallback;
	//std::function<uint32_t(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags properties, uint32_t& allocSize)> GetMemoryTypeIndexCallback;
	//uint32_t GetMemoryTypeIndex(VkDevice& device, VkBuffer& buffer, VkMemoryPropertyFlags properties, uint32_t& allocSize);

	VkExtent2D imageExtent;
	//VkPhysicalDeviceMemoryProperties memProperties;
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline pipeline;
	VkFramebuffer* framebuffers;
	VkCommandPool commandPool;
	VkCommandBuffer* drawCommandBuffers;
	VkCommandBuffer staticTransferCommandBuffer;
	VkCommandBuffer dynamicTransferCommandBuffer;
	
	VkBuffer vertexBuffer;
	VkBuffer vertexTransferBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory vertexTransferBufferMemory;
	
	VkBuffer indexTransferBuffer;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory indexTransferBufferMemory;
	
	VkBuffer uniformTransferBuffer;
	VkDeviceMemory uniformTransferBufferMemory;
	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;
	
	VkBuffer* heightBuffer;

	VkVertexInputAttributeDescription* attributeDescriptions;
	VkVertexInputBindingDescription* bindingDescriptions;
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	
	const VkExtent3D grid;
	
	const uint32_t numFBOs = 2;
	const uint32_t numDrawCmdBuffers = 2;
	const uint32_t numAttrDesc = 4;
	const uint32_t numBindDesc = 3;
	//const uint32_t numVertices = 3;
	const uint32_t numComponents = 3;
	const uint32_t numVertexElements = 2;
	
	uint32_t mat4Size;
	uint32_t uboSize;
	
	glm::mat4 mvp;
	
	float* vertexInfo;
	uint16_t* indices;
	
	uint32_t vertexInfoSize;
	uint16_t numIndices;
	uint32_t indicesBufferSize;
	uint32_t numVerts;
	uint32_t numPrims;
};

#endif