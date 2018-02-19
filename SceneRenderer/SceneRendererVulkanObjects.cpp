#include "SceneRenderer.h"

void SceneRenderer::VulkanSharedDate::Initialize(SceneRenderer::VulkanObjectsSettings &settings) {
	VK_THROW_IF_FAILED(instance.Setup(
		settings.instanceExtensions,
		settings.validationLayers
	));
	VK_THROW_IF_FAILED(debug.Setup(
		instance,
		VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
		VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_ERROR_BIT_EXT
	));
	PickPhysicalDevice();
	VK_THROW_IF_FAILED(device.Setup(
		physicalDevice,
		settings.deviceFeatures,
		settings.deviceExtensions,
		VK_QUEUE_GRAPHICS_BIT,
		instance,
		&swapChain
	));
	swapChain.Setup();
	CreateRenderPass();
	CreateFrameBuffers();
	CreateSemaphores();
}

void SceneRenderer::VulkanSharedDate::CreateFrameBuffers() {
	swapChainFramebuffers.resize(swapChain.imageCount, vk::UniqueFramebuffer{device, vkDestroyFramebuffer});
	for (size_t i = 0; i < swapChain.imageCount; i++)
	{
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &swapChain.views[i];
		framebufferInfo.width = swapChain.extent.width;
		framebufferInfo.height = swapChain.extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, swapChainFramebuffers[i].replace()) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void SceneRenderer::VulkanSharedDate::CreateRenderPass() {
	renderPass.colorAttachments[0].format = swapChain.colorFormat;
	renderPass.DoFinalInitialise();
}

void SceneRenderer::VulkanSharedDate::CreateSemaphores() {
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, imageAvailableSemaphore.replace()) != VK_SUCCESS ||
	    vkCreateSemaphore(device, &semaphoreInfo, nullptr, renderFinishedSemaphore.replace()) != VK_SUCCESS) {

		throw std::runtime_error("failed to create semaphores!");
	}
}

void SceneRenderer::VulkanSharedDate::PickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (auto device : devices)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			std::cout << "\nSelected GPU: " << deviceProperties.deviceName << std::endl;
			physicalDevice = device;
			return;
		}
	}

	throw std::runtime_error("Failed to find physical device!");
}