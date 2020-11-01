module;
#include <iostream>
#include <vector>
#include "vendor/vulkan/vulkan.h" //Note: don't use the .hpp as it currently doesn't work with c++20

export module Vulkan;

export class Vulkan
{
public:
	~Vulkan()
	{
		Terminate();
	}

private:
	VkInstance vk;

public:
	void Terminate()
	{
		if (!vk)
			return;
		vkDestroyInstance(vk, NULL);
	}

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
			std::cerr << "You need to call Init before using this function.";
			return;
		}

		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(vk, &deviceCount, NULL);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vk, &deviceCount, devices.data());

		std::cout << "GPUs:\n";
		for (const auto& device : devices)
		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(device, &props);
			std::cout << props.deviceName << '\n';
		}
	}

	void Init(const char** addExtensions, uint32_t extCount)
	{
		VkInstanceCreateInfo info{};
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "CppTest";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.apiVersion = VK_API_VERSION_1_2;
		info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		info.pApplicationInfo = &appInfo;
		info.enabledLayerCount = 0;
		if (addExtensions && extCount)
		{
			info.enabledExtensionCount = extCount;
			info.ppEnabledExtensionNames = addExtensions;
		}

		if (vkCreateInstance(&info, NULL, &vk) != VK_SUCCESS)
		{
			std::cerr << "Error whilst creating Vulkan instance.\n";
			return;
		}
	}
};