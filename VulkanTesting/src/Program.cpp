#include <iostream>

import GLFW;
import Lua;
import Vulkan;

int main()
{
	auto glfw = GLFWVulkan();
	
	// This casting is strange, but I belive its due to modules being in preview.
	WindowHandle window = (WindowHandle)glfw.MakeWindow("Hello!", 600, 420);
	while (!glfw.ShouldWindowClose(window))
	{
		glfw.ClearBuffer();
		glfw.SwapBuffers(window);
		glfw.PollEvents();
	}

	uint32_t extCount;
	const char** exts = glfw.GetRequiredVulkanExtensions(&extCount);
	VulkanWinSurfaceInfo surf{glfw.GetWin32Handle(window)};
	auto vulkan = Vulkan(exts, extCount, surf);
	vulkan.PrintGPUs();

	std::cin.get();
}