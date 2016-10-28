#ifndef renderer_h
#define renderer_h

#include <vulkan/vulkan.h>

const float vertexInfo[][3] =
	{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f},
     {0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f},
     {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}};

class Renderer
{
public:
	Renderer(VkExtent2D& extent, VkPhysicalDeviceMemoryProperties& memProperties);
	~Renderer();
	
	bool Init(VkDevice& device, const VkFormat& surfaceFormat, const VkImageView* imageView, uint32_t queueFamilyId);
	VkCommandBuffer* ConstructFrame();
	bool Destroy(VkDevice& device);
	
	bool SetupVertexBuffer(VkDevice& device);

private:
	VkExtent2D imageExtent;
	VkPhysicalDeviceMemoryProperties memProperties;
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline pipeline;
	VkFramebuffer framebuffer;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkMemoryAllocateInfo allocInfo = {};

	VkVertexInputAttributeDescription* attributeDescriptions;
	VkVertexInputBindingDescription bindingDescription;
	
	const uint32_t numAttrDesc = 2;
	const uint32_t numVertices = 3;
};

#endif