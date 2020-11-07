module;
#include <iostream>
#include <vector>
#include <optional>

#include "vendor/vulkan/vulkan.h" //Note: don't use the .hpp as it currently doesn't work with c++20

#ifdef _WIN32
#include <wtypes.h>
#include "vendor/vulkan/vulkan_win32.h"
#endif

export module Vulkan;

export struct VulkanWinSurfaceInfo
{
	HWND windowHandle;
};

export class Vulkan
{
public:
	Vulkan(const char** addExtensions, uint16_t extCount, std::optional<VulkanWinSurfaceInfo> surfInfo)
	{
		SetUpVK(addExtensions, extCount);
		CheckForVulkanSupport();
		if (surfInfo.has_value())
			CreateSurface(surfInfo.value());
		PickGPU();
	}

	~Vulkan()
	{
		Terminate();
	}

private:
	struct QueueFamilyIndecies
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
	};

	VkInstance vk;
	uint32_t vkDeviceCount;

	VkPhysicalDevice currentGpu = VK_NULL_HANDLE;
	VkDevice currentLogical;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;

	void CreateWin32Surface(VulkanWinSurfaceInfo surfInfo)
	{
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = surfInfo.windowHandle;
		createInfo.hinstance = GetModuleHandle(NULL);

		if (vkCreateWin32SurfaceKHR(vk, &createInfo, NULL, &surface) != VkResult::VK_SUCCESS)
		{
			std::cout << "Error whilst creating a win32 surface.\n";
			return;
		}
	}

	void CreateSurface(VulkanWinSurfaceInfo surfInfo)
	{
#ifdef _WIN32
		CreateWin32Surface(surfInfo);
		return;
#endif //WIN32

		//TODO: Add other platforms
	}

	QueueFamilyIndecies FindQueueFamilies(const VkPhysicalDevice& device)
	{
		QueueFamilyIndecies indecies;
		uint32_t famCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &famCount, NULL);

		std::vector<VkQueueFamilyProperties> famProps(famCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &famCount, famProps.data());
		uint32_t i = 0;
		for (const auto& queueFam : famProps)
		{
			if (queueFam.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
			{
				indecies.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport)
				indecies.presentFamily = i;
			++i;
		}
		return indecies;
	}

	void CreateLogicalDevice()
	{
		auto queues = FindQueueFamilies(currentGpu);
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queues.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures feats{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		createInfo.pEnabledFeatures = &feats;

		createInfo.enabledExtensionCount = 0;

		createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(currentGpu, &createInfo, NULL, &currentLogical) != VkResult::VK_SUCCESS)
		{
			std::cerr << "Error occured during creation of VkDevice\n";
			return;
		}
		vkGetDeviceQueue(currentLogical, queues.graphicsFamily.value(), 0, &graphicsQueue);
	}

	bool IsGPUSuitable(const VkPhysicalDevice& device)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(device, &props);

		if (props.deviceType != VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return false;

		auto queueFam = FindQueueFamilies(device);
		if (!queueFam.graphicsFamily.has_value())
			return false;

		if (!queueFam.presentFamily.has_value())
			return false;
	}

	void PickGPU()
	{
		std::vector<VkPhysicalDevice> devices(vkDeviceCount);
		vkEnumeratePhysicalDevices(vk, &vkDeviceCount, devices.data());
		for (const auto& device : devices)
		{
			if (!IsGPUSuitable(device))
				continue;

			currentGpu = device;
			break;
		}

		if (currentGpu == VK_NULL_HANDLE)
		{
			std::cerr << "No suitable GPU was found.\n";
			return;
		}
	}

	void SetUpVK(const char** addExtensions, uint32_t extCount)
	{
		VkInstanceCreateInfo info{};
		VkApplicationInfo appInfo{};
		appInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Tester";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.apiVersion = VK_API_VERSION_1_2;
		info.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		info.pApplicationInfo = &appInfo;
		info.enabledLayerCount = 0;
		if (addExtensions && extCount)
		{
			info.enabledExtensionCount = extCount;
			info.ppEnabledExtensionNames = addExtensions;
		}

		if (vkCreateInstance(&info, NULL, &vk) != VkResult::VK_SUCCESS)
		{
			std::cerr << "Error whilst creating Vulkan instance.\n";
			return;
		}
	}

	bool CheckForVulkanSupport()
	{
		vkEnumeratePhysicalDevices(vk, &vkDeviceCount, NULL);
		if (vkDeviceCount == 0)
		{
			std::cerr << "No vulkan supported gpus found.\n";
			return true;
		}
	}

	void Terminate()
	{
		if (!vk)
			return;
		vkDestroySurfaceKHR(vk, surface, NULL);
		vkDestroyDevice(currentLogical, NULL);
		vkDestroyInstance(vk, NULL);
	}

public:
	void PrintExtensions()
	{
		uint32_t extCount;
		vkEnumerateInstanceExtensionProperties(NULL, &extCount, NULL);
		std::vector<VkExtensionProperties> exts(extCount);
		vkEnumerateInstanceExtensionProperties(NULL, &extCount, exts.data());

		std::cout << "Available extensions:\n";
		for (const auto& ext : exts)
			std::cout << ext.extensionName << '\n';
	}

	void PrintGPUs()
	{
		if (!vk)
		{
			std::cerr << "You need to call Init before using this function.\n";
			return;
		}

		std::vector<VkPhysicalDevice> devices(vkDeviceCount);
		vkEnumeratePhysicalDevices(vk, &vkDeviceCount, devices.data());

		std::cout << "GPUs:\n";
		for (const auto& device : devices)
		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(device, &props);
			std::cout << props.deviceName << '\n';
		}
	}
};