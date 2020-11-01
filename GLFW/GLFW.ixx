module;
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include "vendor/include/GLFW/glfw3.h"

export module GLFW;

export using WindowHandle = void*;

export class GLFWVulkan
{
private:
	void Init()
	{
		if (!glfwInit())
		{
			std::cerr << "Could not init GLFW\n";
		}

		if (!glfwVulkanSupported())
		{
			std::cerr << "Vulkan was not found by GLFW\n";
		}

		//auto err = glfwCreateWindowSurface();
	}

public:
	GLFWVulkan()
	{
		Init();
	}
	
	~GLFWVulkan()
	{
		glfwTerminate();
	}

	void Terminate()
	{
		glfwTerminate();
	}

	WindowHandle CreateWindow(const char* title, int sizeX = 600, int sizeY = 420) const
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		WindowHandle window = glfwCreateWindow(sizeX, sizeY, title, NULL, NULL);
		if (!window)
			throw "Could not create GLFW window";

		glfwMakeContextCurrent((GLFWwindow*)window);
		return window;
	}

	void SetCurrentWindow(WindowHandle window) const
	{
		glfwMakeContextCurrent((GLFWwindow*)window);
	}

	bool ShouldWindowClose(WindowHandle window) const
	{
		return glfwWindowShouldClose((GLFWwindow*)window);
	}

	void ClearBuffer() const
	{
		
	}

	void SwapBuffers(WindowHandle window) const
	{
		glfwSwapBuffers((GLFWwindow*)window);
	}

	void PollEvents() const
	{
		glfwPollEvents();
	}
};