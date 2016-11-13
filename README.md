# VFSME - Vulkan Free Surface Modeling Engine

A simple framework and demo application for free surface modeling using compute and rendering with the Vulkan API.

![Screenshot](/screenshot.png)

Todo List:
- [ ] Integrate existing Boussinesq equation framework for depth integration: See research at [Nigel J W](http://nigeljw.com)
- [ ] Add other free surface modeling like cloth
- [ ] Use lighter weight synchronization primitives between compute/transfer/render queues
- [ ] Fix specular lighting in fragment shader

Dependencies:

[LunarG Vulkan SDK] (https://vulkan.lunarg.com)

[GLFW] (http://www.glfw.org)

[GLM] (http://glm.g-truc.net)

Great references for the Vulkan API:

[Khronos Vulkan API] (https://www.khronos.org/vulkan)

[Vulkan Tutorial](https://vulkan-tutorial.com)

[Sascha Willems Vulkan Examples] (https://github.com/SaschaWillems/Vulkan)
