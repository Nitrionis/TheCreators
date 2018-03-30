#include "VulkanSharedDate.h"
#include "RendererSettings.h"

void Vulkan::Initialize() {
	VK_THROW_IF_FAILED(instance.Setup(
		RendererSettings::Instance().instanceExtensions,
		RendererSettings::Instance().validationLayers
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
		RendererSettings::Instance().deviceFeatures,
		RendererSettings::Instance().deviceExtensions,
		VK_QUEUE_GRAPHICS_BIT,
		instance,
		&swapChain
	));
	swapChain.Setup();
	CreateDescriptorPool();
	CreateIntermediateImages();
	CreateSemaphores();
}

void Vulkan::CreateSemaphores() {
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, imageAvailableSemaphore.replace()) != VK_SUCCESS ||
	    vkCreateSemaphore(device, &semaphoreInfo, nullptr, renderFinishedSemaphore.replace()) != VK_SUCCESS) {

		throw std::runtime_error("failed to create semaphores!");
	}
}

void Vulkan::PickPhysicalDevice() {
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

void Vulkan::ShowIntermediateImage(uint32_t index) {
	vkDeviceWaitIdle(device);

	VkImageCopy region = {};
	region.extent = image.intermediate[index].extent;
	region.dstOffset = {0, 0, 0};
	region.srcOffset = {0, 0, 0};
	region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.dstSubresource.baseArrayLayer = 0;
	region.dstSubresource.layerCount     = 1;
	region.dstSubresource.mipLevel       = 0;
	region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.srcSubresource.baseArrayLayer = 0;
	region.srcSubresource.layerCount     = 1;
	region.srcSubresource.mipLevel       = 0;

	swapChain.AcquireNext(imageAvailableSemaphore);
	uint32_t imageIndex = swapChain.currImageIndex;

	vk::CommandBuffer commandBuffer = device.CreateCommandBuffer(
		device.defaultPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true
	);

	device.SetImageBarrier(swapChain.images[imageIndex],
	    VK_IMAGE_LAYOUT_UNDEFINED,         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	    0,                                 VK_ACCESS_TRANSFER_WRITE_BIT,
	    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
	    VK_QUEUE_FAMILY_IGNORED,           VK_QUEUE_FAMILY_IGNORED,
	    nullptr,
	    commandBuffer
	);
	device.SetImageBarrier(image.intermediate[index].image,
	    VK_IMAGE_LAYOUT_UNDEFINED,         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
	    0,                                 VK_ACCESS_TRANSFER_READ_BIT,
	    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
	    VK_QUEUE_FAMILY_IGNORED,           VK_QUEUE_FAMILY_IGNORED,
	    nullptr,
	    commandBuffer
	);
	vkCmdCopyImage(
		commandBuffer,
		image.intermediate[index].image,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		swapChain.images[imageIndex],
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);
	device.SetImageBarrier(swapChain.images[imageIndex],
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	    VK_ACCESS_TRANSFER_WRITE_BIT,         VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
	    VK_PIPELINE_STAGE_TRANSFER_BIT,       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    VK_QUEUE_FAMILY_IGNORED,              VK_QUEUE_FAMILY_IGNORED,
	    nullptr,
	    commandBuffer
	);
	VK_THROW_IF_FAILED(vkEndCommandBuffer(commandBuffer));

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

	if (vkQueueSubmit(commandQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}
	swapChain.Present(commandQueue, renderFinishedSemaphore);

	vkDeviceWaitIdle(device);
	vkFreeCommandBuffers(device, device.defaultPool, 1, &commandBuffer);

	system("pause");
}

void Vulkan::CreateIntermediateImages() {

	VkExtent3D imageExtent = {
		static_cast<uint32_t>(swapChain.extent.width),
		static_cast<uint32_t>(swapChain.extent.height),
		1
	};
	for (int i = 0; i < 3; i++) {
		device.CreateImage(
			&image.intermediate[i],
			imageExtent,
			imageExtent.width * imageExtent.height * 4,
			nullptr,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			swapChain.colorFormat
		);
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image      = image.intermediate[i].image;
		viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format     = swapChain.colorFormat;
		viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel   = 0;
		viewInfo.subresourceRange.levelCount     = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount     = 1;

		if (vkCreateImageView(device, &viewInfo, nullptr, &image.intermediate[i].view) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture textureImage view!");
		}
	}
}

void Vulkan::CreateDescriptorPool() {

	std::array<VkDescriptorPoolSize, 2> poolSize = {};

	poolSize[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[0].descriptorCount = 5;

	poolSize[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.maxSets = 5;
	poolInfo.poolSizeCount = poolSize.size();
	poolInfo.pPoolSizes = poolSize.data();

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, descriptorPool.replace()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool!");
	}
}