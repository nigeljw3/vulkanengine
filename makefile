# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

VULKAN_PATH = /c/Dev/VulkanSDK/1.0.30.0
GLFW_PATH = /c/Dev/glfw/glfw-3.2.1.bin.WIN32
GLM_PATH = /C/Dev/glm

CFLAGS = -std=c++11 -Wall
INCLUDE = -I$(VULKAN_PATH)/include -I$(GLFW_PATH)/include -I$(GLM_PATH)
LDFLAGS = -L$(VULKAN_PATH)/Bin32 -L$(GLFW_PATH)/lib-mingw
LDLIBS = -lvulkan-1 -lglfw3 -lgdi32
DEFINES = -DVK_USE_PLATFORM_WIN32_KHR
OBJS = renderer.o system.o controller.o compositor.o compute.o

.PHONY: clean test shaders

vulkan: main.cpp $(OBJS)
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) $(LDFLAGS) -o vulkan main.cpp $(OBJS) $(LDLIBS)

system.o: system.h system.cpp
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c system.cpp -o $@	
	
renderer.o: renderer.h renderer.cpp
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c renderer.cpp -o $@
	
compute.o: compute.h compute.cpp
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c compute.cpp -o $@

controller.o: controller.h controller.cpp
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c controller.cpp -o $@
	
compositor.o: compositor.h compositor.cpp
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c compositor.cpp -o $@
	
test: vulkan
	LD_LIBRARY_PATH=$(VULKAN_PATH)/lib VK_LAYER_PATH=$(VULKAN_PATH)/etc/explicit_layer.d ./vulkan

shaders:
	$(VULKAN_PATH)/Bin32/glslangValidator.exe -V shader.vert
	$(VULKAN_PATH)/Bin32/glslangValidator.exe -V shader.frag
	
clean:
	rm *.exe *.o