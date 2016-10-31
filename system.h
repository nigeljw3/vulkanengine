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

#ifndef system_h
#define system_h

#include "compositor.h"

#include <vulkan/vulkan.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

class System
{
public:
	System();
	~System();
	
	bool Init();
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

#endif