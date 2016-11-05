#ifndef compute_h
#define compute_h

#include <vulkan/vulkan.h>
#include <functional>

class Compute
{
public:
	Compute(VkPhysicalDeviceMemoryProperties& props, std::function<uint32_t(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags properties, uint32_t& allocSize)> memTypeIndexCallback);
	~Compute();
	
	void Init(VkDevice& device, VkExtent3D extent);
	void Destroy(VkDevice& device);
	void SetupQueue(VkDevice& device);
	VkCommandBuffer* SetupCommandBuffer(VkDevice& device, uint32_t graphicsQueueFamilyId, uint32_t computeQueueFamilyId);
	
private:
	uint32_t GetMemoryTypeIndex(VkDevice& device, VkImage& image, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags propFlags, uint32_t& allocSize);

	void SetupCommandBuffer();
	std::function<uint32_t(VkDevice& device, VkBuffer& buffer, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags properties, uint32_t& allocSize)> GetMemoryTypeIndexCallback;
		
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	
	VkCommandBuffer commandBuffer;
	VkBufferMemoryBarrier memoryBarrier;
	VkDeviceMemory memory;
	
	VkBuffer uniformBuffer;
	VkBuffer storageBuffer;
	
	VkImage image;
	VkCommandPool commandPool;
	
	VkExtent3D extent;
	
	VkDescriptorSet descriptorSets[2];
	
	uint32_t size;
	
	VkPhysicalDeviceMemoryProperties memProperties;
};

#endif