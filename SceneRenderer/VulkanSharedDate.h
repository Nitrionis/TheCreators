#pragma once

#include "CustomVulkanTypes.hpp"

class Vulkan {
private:
	Vulkan(){}

public:
	~Vulkan(){}

	Vulkan(Vulkan const&) = delete;
	Vulkan& operator= (Vulkan const&) = delete;

	static Vulkan& Instance() {
		static Vulkan s;
		return s;
	}

	vk::Instance    instance;
	vk::Debug       debug;

	vk::Device      device;
	vk::SwapChain   swapChain;

	vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;

	vk::CommandQueue                commandQueue    = VK_NULL_HANDLE;
	vk::UniqueCommandPool           commandPool     {device, vkDestroyCommandPool};
	std::vector<vk::CommandBuffer>  commandBuffers;

	vk::UniqueSemaphore imageAvailableSemaphore{device, vkDestroySemaphore};
	vk::UniqueSemaphore renderFinishedSemaphore{device, vkDestroySemaphore};

	vk::UniqueHandle<VkDescriptorPool> descriptorPool{device, vkDestroyDescriptorPool};

	struct {
		vk::Image intermediate[3];
	}image;

	void Initialize();
	void ShowIntermediateImage(uint32_t index);

private:
	void PickPhysicalDevice();

	void CreateDescriptorPool();
	void CreateSemaphores();
	void CreateIntermediateImages();
};