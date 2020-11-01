#include <iostream>

import GLFW;
import Lua;
import Vulkan;

int main()
{
	auto vulkan = Vulkan();
	vulkan.Init(nullptr, NULL);
	vulkan.PrintGPUs();

	auto glfw = GLFWVulkan();
	
	// This casting is strange, but I belive its due to modules being in preview.
	WindowHandle window = (WindowHandle)glfw.CreateWindow("Hello!", 600, 420);
	while (!glfw.ShouldWindowClose(window))
	{
		glfw.ClearBuffer();
		glfw.SwapBuffers(window);
		glfw.PollEvents();
	}
	glfw.Terminate();
	vulkan.Terminate();

	std::cin.get();
}