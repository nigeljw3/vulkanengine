#ifndef renderer_h
#define renderer_h

#include <vulkan/vulkan.h>

class Renderer
{
public:
	bool Init(VkDevice& device, VkFormat& surfaceFormat, const VkImageView* imageView, uint32_t queueFamilyId);
	VkCommandBuffer* ConstructFrame();
	bool Destroy(VkDevice& device);

	Renderer(VkExtent2D& extent);
	~Renderer();

private:
	VkExtent2D imageExtent;
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline pipeline;
	VkFramebuffer framebuffer;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
};

#endif