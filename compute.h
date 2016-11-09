#ifndef compute_h
#define compute_h

#include "commands.h"

#include <vulkan/vulkan.h>
#include <functional>

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
	
private:
	//uint32_t GetMemoryTypeIndex(VkDevice& device, VkImage& image, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags propFlags, uint32_t& allocSize);

	void SetupBuffers(VkDevice& device);
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
};

#endif