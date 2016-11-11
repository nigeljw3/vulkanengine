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

#include "commands.h"

#include "types.h"

#include <stdexcept>
#include <iostream>

Commands::Commands(VkPhysicalDeviceMemoryProperties& memProps)
: memProperties(memProps)
{
	
}

Commands::~Commands()
{
	
}

void Commands::SetupImage(VkDevice& device, VkImage& image, VkExtent3D& extent, VkFormat& format, VkDeviceMemory& memory, VkMemoryPropertyFlags props, VkBufferUsageFlags usage)
{
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = format;
	imageCreateInfo.extent = extent;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = usage;
	//imageCreateInfo.flags = 0;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	VkResult result = vkCreateImage(device, &imageCreateInfo, nullptr, &image);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Image creation failed");
	}
		
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);
		
	uint32_t imageSize = 0;
	uint32_t imageMemTypeIndex = GetMemoryTypeIndex(device, memRequirements, memProperties, props, imageSize);
		
	VkMemoryAllocateInfo memAllocInfo { };
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.allocationSize = imageSize;
	memAllocInfo.memoryTypeIndex = imageMemTypeIndex;
	
	result = vkAllocateMemory(device, &memAllocInfo, nullptr, &memory);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Memory allocation failed");
	}
	
	result = vkBindImageMemory(device, image, memory, 0);
	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Bind image failed");
	}
}

bool Commands::SetupBuffer(VkDevice& device, VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage)
{
	VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	std::cout << "request size: " << size << std::endl;

    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
	
	if (result != VK_SUCCESS)
	{
        throw std::runtime_error("Transfer buffer creation failed");
    }
	
	VkMemoryRequirements memRequirements;
	
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
	
	uint32_t allocSize;
	uint32_t memTypeIndex = GetMemoryTypeIndex(device, memRequirements, memProperties, properties, allocSize);
	
	std::cout << "alloc size: " << allocSize << std::endl;
	
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

uint32_t Commands::GetMemoryTypeIndex(VkDevice& device, VkMemoryRequirements& memRequirements, VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags propFlags, uint32_t& allocSize)
{	
	uint32_t memTypeIndex = InvalidIndex;
	
	for (uint32_t i = 0; i < props.memoryTypeCount; ++i)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(props.memoryTypes[i].propertyFlags & propFlags) == propFlags)
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