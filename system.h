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

#ifndef system_h
#define system_h

#include <vulkan/vulkan.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "compositor.h"

namespace vfsme
{

class System
{
public:
	System();
	~System();
	
	///@note Only define copy and assignment operators if they are actually required
	System& operator=(const System&) = delete;
    System(const System&) = delete;
	
	bool Init(uint32_t width, uint32_t height);
	bool CreateSurface(VkInstance& instance, VkSurfaceKHR* surface);
	void Loop(Compositor& composer, VkDevice& device);
	bool DestroySurface(VkInstance& instance, VkSurfaceKHR& surface);
	bool Destroy();
	bool CheckExtensionsSupport(uint32_t extensionCount, VkExtensionProperties* extensions);

	
private:
	GLFWwindow* window;
	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
};

};

#endif