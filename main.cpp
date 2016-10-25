#include <vulkan/vulkan.h>

#include "renderer.h"
#include "system.h"
#include "controller.h"
#include "compositor.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <limits>

int main()
{	
	System window;
    	
	window.Init();
	
	Controller devCtrl;
	
	devCtrl.Init();

	devCtrl.SetupQueue();
	
	VkSurfaceKHR surface;
	
	window.CreateSurface(devCtrl.GetInstance(), &surface);
	
	devCtrl.SetupDevice(surface);
	
	devCtrl.Configure(surface);
		
	Compositor composer;
	
	composer.Init(devCtrl.GetDevice(),
				  surface,
				  devCtrl.GetQueueFamilyId(),
				  devCtrl.GetGraphicsQueueIndex(),
				  devCtrl.GetPresentQueueIndex());

	window.Loop();
	
	composer.Destroy(devCtrl.GetDevice());
	
	window.DestroySurface(devCtrl.GetInstance(), surface);
	
	window.Destroy();
    
    return 0;
}