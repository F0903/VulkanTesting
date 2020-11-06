module;
#include <iostream>

#ifdef _WIN32
#include <wtypes.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#define GLFW_INCLUDE_VULKAN
#include "vendor/include/GLFW/glfw3.h"
#include "vendor/include/GLFW/glfw3native.h"


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
		//auto err = glfwCreateWindowSurface();
	}

	void Terminate()
	{
		glfwTerminate();
	}

public:
	GLFWVulkan()
	{
		Init();
	}
	
	~GLFWVulkan()
	{
		Terminate();
	}

	WindowHandle MakeWindow(const char* title, int sizeX = 600, int sizeY = 420) const
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

	HWND GetWin32Handle(WindowHandle handle) const { return glfwGetWin32Window((GLFWwindow*)handle); }

	const char** GetRequiredVulkanExtensions(uint32_t* count) const { return glfwGetRequiredInstanceExtensions(count); }
};