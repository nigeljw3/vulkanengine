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
#include <stdexcept>

int main()
{	
	try
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
			
		Compositor composer(devCtrl.GetMemoryProperties());
		
		bool supported = devCtrl.PresentModeSupported(surface, composer.GetPresentMode()) &&
						 devCtrl.SurfaceFormatSupported(surface, composer.GetSurfaceFormat());
		
		if (supported)
		{
			composer.Init(devCtrl.GetDevice(),
						  surface,
						  devCtrl.GetQueueFamilyId(),
						  devCtrl.GetGraphicsQueueIndex(),
						  devCtrl.GetPresentQueueIndex());

			window.Loop();
		}
		
		composer.Destroy(devCtrl.GetDevice());
		
		window.DestroySurface(devCtrl.GetInstance(), surface);
		
		devCtrl.Destroy();
		
		window.Destroy();
	
	}
	catch (const std::runtime_error& e)
	{
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
	
    return 0;
}