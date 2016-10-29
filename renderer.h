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
	VkCommandBuffer* TransferBuffer(VkDevice& device);
	bool Destroy(VkDevice& device);
	
private:
	bool SetupClientSideVertexBuffer(VkDevice& device);
	bool SetupServerSideVertexBuffer(VkDevice& device);
	
	bool SetupShaderParameters();
	uint32_t GetMemoryTypeIndex(VkDevice& device, VkBuffer& buffer, VkMemoryPropertyFlags properties, uint32_t& allocSize);

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
	VkCommandBuffer transferCommandBuffer;
	VkBuffer transferBuffer;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory transferBufferMemory;
	VkMemoryAllocateInfo allocInfo = {};

	VkVertexInputAttributeDescription* attributeDescriptions;
	VkVertexInputBindingDescription bindingDescription;
	
	const uint32_t numAttrDesc = 2;
	const uint32_t numVertices = 3;
};

#endif