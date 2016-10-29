#ifndef renderer_h
#define renderer_h

#include <vulkan/vulkan.h>

//const float vertexInfo[][3] =
//	{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f},
//     {0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f},
//     {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}};

const float vertexInfo[][3] = {
    {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f},
    {0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f},
    {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f},
    {-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}
};

const uint16_t indices[] = {
    0, 1, 2, 2, 3, 0
};

class Renderer
{
public:
	Renderer(VkExtent2D& extent, VkPhysicalDeviceMemoryProperties& memProperties);
	~Renderer();
	
	bool Init(VkDevice& device, const VkFormat& surfaceFormat, const VkImageView* imageView, uint32_t queueFamilyId);
	VkCommandBuffer* ConstructFrame();
	VkCommandBuffer* TransferBuffers(VkDevice& device);
	bool Destroy(VkDevice& device);
	
private:
	bool SetupBuffer(VkDevice& device, VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage);
	bool SetupIndexBuffer(VkDevice& device);
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
	
	VkBuffer vertexBuffer;
	VkBuffer vertexTransferBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory vertexTransferBufferMemory;
	
	VkBuffer indexTransferBuffer;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory indexTransferBufferMemory;

	VkVertexInputAttributeDescription* attributeDescriptions;
	VkVertexInputBindingDescription bindingDescription;
	
	const uint32_t numAttrDesc = 2;
	const uint32_t numVertices = 3;
};

#endif