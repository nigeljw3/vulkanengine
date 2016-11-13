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
	///@note Singleton pattern implementation for the system class
	/// as there will only ever be a single system on any platform
	/// This is assured to be reentrant as of c++11 since concuurent
	/// execution will wait for initialization
	static System& GetSingletonInstance()
	{
	  static System instance;
	  return instance;
	}

	///@note Remove copy and move contructors and assignment operators
	/// since they should never be used with the single pattern
	System(const System&) = delete;
	System(System&&) = delete;
	System& operator=(const System&) = delete;
	System& operator=(System &&) = delete;
    	
	bool Init(uint32_t width, uint32_t height);
	bool Destroy();
	
	bool CreateSurface(VkInstance& instance, VkSurfaceKHR* surface);
	void Loop(Compositor& composer, VkDevice& device);
	bool DestroySurface(VkInstance& instance, VkSurfaceKHR& surface);
	bool CheckExtensionsSupport(uint32_t extensionCount, VkExtensionProperties* extensions);

private:
	System() = default;
	~System() = default;

	GLFWwindow* window;
	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
};

};

#endif