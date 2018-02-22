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

	VkExtent3D imageExtent = {
		static_cast<uint32_t>(1920),
		static_cast<uint32_t>(1080),
		1
	};
	device.CreateImage(
		&intermediateImage,
		imageExtent,
		imageExtent.width * imageExtent.height * 4,
		nullptr,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		swapChain.colorFormat
	);
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image      = intermediateImage.image;
	viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format     = swapChain.colorFormat;
	viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = 1;

	if (vkCreateImageView(device, &viewInfo, nullptr, &intermediateImage.view) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture textureImage view!");
	}


	framebuffers.resize(swapChain.imageCount, vk::UniqueFramebuffer{device, vkDestroyFramebuffer});
	for (size_t i = 0; i < swapChain.imageCount; i++)
	{
		VkImageView attachments[] = {swapChain.views[i], intermediateImage.view};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChain.extent.width;
		framebufferInfo.height = swapChain.extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, framebuffers[i].replace()) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void SceneRenderer::VulkanSharedDate::CreateRenderPass() {
	VkAttachmentDescription colorAttachment = {};
		colorAttachment.flags = VK_FLAGS_NONE;
		colorAttachment.format = swapChain.colorFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp =  VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout =   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	renderPass.colorAttachments.push_back(colorAttachment);

	VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 1;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	renderPass.colorAttachmentRefs.push_back(colorAttachmentRef);

	renderPass.subPasses[0].colorAttachmentCount = static_cast<uint32_t>(renderPass.colorAttachmentRefs.size());
	renderPass.subPasses[0].pColorAttachments = renderPass.colorAttachmentRefs.data();

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