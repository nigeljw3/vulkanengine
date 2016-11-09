#ifndef commands_h
#define commands_h

#include <vulkan/vulkan.h>

class Commands
{
public:
	Commands(VkPhysicalDeviceMemoryProperties& memProperties);
	~Commands();
protected:
	void SetupImage(VkDevice& device, VkImage& image, VkExtent3D& extent, VkFormat& format, VkDeviceMemory& memory, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage);
	bool SetupBuffer(VkDevice& device, VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage);
	uint32_t GetMemoryTypeIndex(VkDevice& device, VkMemoryRequirements& memReqs, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags propFlags, uint32_t& allocSize);

	VkPhysicalDeviceMemoryProperties memProperties;
private:
};

#endif