#include "SceneRenderer.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>
#include "ImageLoader.h"

SceneRenderer::SceneRenderer() {
	try
	{
		settings.Initialize();
		vulkan.Initialize(settings);
		chunks.Initialize();
		CreateCommandBuffers();
		//ImageLoader imageLoader("C:\\Developer\\JetBrains\\Clion\\Proj\\TheCreators\\Textures\\texture.png");
	}
	catch (std::runtime_error e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
	DrawScene();

	const int drawCount = 10;
	auto startTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < drawCount; i++)
		DrawScene();
	auto endTime = std::chrono::high_resolution_clock::now();

	auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::cout << "Draw Count: " << drawCount << " Time: " << dtime.count() << " ms" << std::endl;

	VkImageCopy region = {};
	region.extent = {1920, 1080, 1};
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

	vulkan.swapChain.AcquireNext(vulkan.imageAvailableSemaphore);
	uint32_t imageIndex = vulkan.swapChain.currImageIndex;

	vulkan.device.SetImageBarrier(vulkan.intermediateImage.image,
	    VK_IMAGE_LAYOUT_UNDEFINED,         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
	    0,                                 VK_ACCESS_TRANSFER_READ_BIT,
	    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
	);
	vulkan.device.SetImageBarrier(vulkan.swapChain.images[imageIndex],
	    VK_IMAGE_LAYOUT_UNDEFINED,         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	    0,                                 VK_ACCESS_TRANSFER_WRITE_BIT,
	    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	vulkan.device.CopyImage(
		vulkan.intermediateImage.image,
		vulkan.swapChain.images[imageIndex],
		&region
	);

	vulkan.device.SetImageBarrier(vulkan.swapChain.images[imageIndex],
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	    VK_ACCESS_TRANSFER_WRITE_BIT,         VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
	    VK_PIPELINE_STAGE_TRANSFER_BIT,       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	);
	system("pause");
	vulkan.swapChain.Present(vulkan.commandQueue, VK_NULL_HANDLE);
}

SceneRenderer::~SceneRenderer() {
	vkDeviceWaitIdle(vulkan.device);
}

void SceneRenderer::CreateCommandBuffers() {

	vulkan.commandQueue = vulkan.device.GetQueue(vulkan.device.indices.graphicsFamily);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = vulkan.device.indices.graphicsFamily;
	poolInfo.flags = VK_FLAGS_NONE;

	if (vkCreateCommandPool(vulkan.device, &poolInfo, nullptr, vulkan.commandPool.replace()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool!");
	}

	vulkan.commandBuffers.resize(vulkan.swapChain.imageCount);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vulkan.commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(vulkan.commandBuffers.size());

	if (vkAllocateCommandBuffers(vulkan.device, &allocInfo, vulkan.commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	for (size_t i = 0; i < vulkan.commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vkBeginCommandBuffer(vulkan.commandBuffers[i], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vulkan.renderPass;
		renderPassInfo.framebuffer = vulkan.framebuffers[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = vulkan.swapChain.extent;

		VkClearValue clearColor[] = {{0.157f, 0.173f, 0.191f, 1.0f},
		                             {0.000f, 1.000f, 0.000f, 1.0f}};
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearColor;

		vkCmdBeginRenderPass(vulkan.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(vulkan.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan.material);

		vkCmdBindDescriptorSets(
			vulkan.commandBuffers[i],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			vulkan.material.pipelineLayout, 0, 1, &chunks.descriptorSet, 0, nullptr);

		vkCmdDraw(vulkan.commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(vulkan.commandBuffers[i]);

		if (vkEndCommandBuffer(vulkan.commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}
	}
}

void SceneRenderer::DrawScene() {
	auto startTime = std::chrono::high_resolution_clock::now();

	//vkQueueWaitIdle(vulkan.commandQueue);

	vulkan.swapChain.AcquireNext(vulkan.imageAvailableSemaphore);
	uint32_t i = vulkan.swapChain.currImageIndex;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &vulkan.imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vulkan.commandBuffers[i];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &vulkan.renderFinishedSemaphore;

	if (vkQueueSubmit(vulkan.commandQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}
	vulkan.swapChain.Present(vulkan.commandQueue, vulkan.renderFinishedSemaphore);

	auto endTime = std::chrono::high_resolution_clock::now();
	auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::cout << dtime.count() << std::endl;
}
