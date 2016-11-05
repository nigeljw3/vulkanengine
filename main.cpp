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