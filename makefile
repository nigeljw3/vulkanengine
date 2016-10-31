VULKAN_PATH = /c/Dev/VulkanSDK/1.0.26.0
GLFW_PATH = /c/Dev/glfw/glfw-3.2.1.bin.WIN32
GLM_PATH = /C/Dev/glm

CFLAGS = -std=c++11 -Wall
INCLUDE = -I$(VULKAN_PATH)/include -I$(GLFW_PATH)/include -I$(GLM_PATH)
LDFLAGS = -L$(VULKAN_PATH)/lib -L$(GLFW_PATH)/lib-mingw
LDLIBS = -lvulkan -lglfw3 -lgdi32
DEFINES = -DVK_USE_PLATFORM_WIN32_KHR
OBJS = renderer.o system.o controller.o compositor.o

.PHONY: clean test shaders

vulkan: main.cpp $(OBJS)
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) $(LDFLAGS) -o vulkan main.cpp $(OBJS) $(LDLIBS)

system.o: system.h system.cpp
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c system.cpp -o $@	
	
renderer.o: renderer.h renderer.cpp
	g++ $(CFLAGS) $(DEFINES) $(INCLUDE) -c renderer.cpp -o $@

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