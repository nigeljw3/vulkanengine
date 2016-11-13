/**
 * Copyright (C) 2016 Nigel Williams
 *
 * Vulkan Free Surface Modeling Engine (VFSME) is free software:
 * you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef commands_h
#define commands_h

#include <vulkan/vulkan.h>

namespace vfsme
{

class Commands
{
public:
	explicit Commands(const VkPhysicalDeviceMemoryProperties& memProperties);
	~Commands() = default;
	
	///@note Only define copy and move constructors and assignment operators if they are actually required
    Commands(const Commands&) = delete;
	Commands(Commands&&) = delete;
	Commands& operator=(const Commands&) = delete;
	Commands& operator=(Commands &&) = delete;
	
protected:
	void SetupImage(VkDevice& device, VkImage& image, VkExtent3D& extent, VkFormat& format, VkDeviceMemory& memory, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage);
	bool SetupBuffer(VkDevice& device, VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage);
	uint32_t GetMemoryTypeIndex(VkDevice& device, VkMemoryRequirements& memReqs, const VkPhysicalDeviceMemoryProperties& props, VkMemoryPropertyFlags propFlags, uint32_t& allocSize) const;
	
	const VkPhysicalDeviceMemoryProperties& memProperties;
};

};

#endif