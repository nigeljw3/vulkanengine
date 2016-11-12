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
	///@note Static window size for now
	/// If dynamic window resizing is added, then swap chain reconstruction is necessary
	const uint32_t width = 1920;
	const uint32_t height = 1080;

	try
	{
		vfsme::System window;
			
		window.Init(width, height);
		
		vfsme::Controller devCtrl;
		
		devCtrl.Init();
		devCtrl.SetupQueue();
		
		VkSurfaceKHR surface;
		
		window.CreateSurface(devCtrl.GetInstance(), &surface);
		
		devCtrl.SetupDevice(surface);
		devCtrl.Configure(surface);
			
		vfsme::Compositor composer(devCtrl.GetMemoryProperties());
		
		bool supported = devCtrl.PresentModeSupported(surface, composer.GetPresentMode()) &&
						 devCtrl.SurfaceFormatSupported(surface, composer.GetSurfaceFormat());
		
		if (supported)
		{
			composer.Init(devCtrl.GetDevice(),
						  surface, width, height,
						  devCtrl.GetQueueFamilyId(),
						  devCtrl.GetGraphicsQueueIndex(),
						  devCtrl.GetPresentQueueIndex(),
						  devCtrl.GetComputeQueueIndex());

			window.Loop(composer, devCtrl.GetDevice());
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