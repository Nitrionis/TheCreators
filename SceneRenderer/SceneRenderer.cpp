#include "SceneRenderer.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>
#include "ImageLoader.h"

SceneRenderer::VulkanObjectsSettings SceneRenderer::settings;
SceneRenderer::VulkanSharedDate SceneRenderer::vulkan;

SceneRenderer::SceneRenderer() {
	try
	{
		settings.Initialize();
		vulkan.Initialize(settings);
		chunks.Initialize();
		bloor.Initialize();
		CreateCommandBuffers();
		//ImageLoader imageLoader("C:\\Developer\\JetBrains\\Clion\\Proj\\TheCreators\\Textures\\texture.png");
	}
	catch (std::runtime_error e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
	DrawScene();

	vulkan.ShowIntermediateImage();
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
		renderPassInfo.renderPass = chunks.renderPass;
		renderPassInfo.framebuffer = chunks.framebuffer;
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = vulkan.swapChain.extent;

		std::array<VkClearValue, 1> clearColors = {VkClearValue{1.000f, 0.000f, 0.000f, 1.0f}};
		renderPassInfo.clearValueCount = clearColors.size();
		renderPassInfo.pClearValues = clearColors.data();

		vkCmdBeginRenderPass(vulkan.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(vulkan.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, chunks.material.ground);

		vkCmdBindDescriptorSets(
			vulkan.commandBuffers[i],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			chunks.material.ground.pipelineLayout, (uint32_t)Stage::DrawChunks, 1, &chunks.descriptorSet, 0, nullptr);

		vkCmdDraw(vulkan.commandBuffers[i], 6, 1, 0, 0);

		vkCmdEndRenderPass(vulkan.commandBuffers[i]);

		// TODO subpass Bloor Horizontal
		renderPassInfo.renderPass = bloor.renderPass.horizontal;
		renderPassInfo.framebuffer = bloor.framebuffer.horizontal;

		vkCmdBeginRenderPass(vulkan.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(vulkan.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, bloor.material.horizontal);

		vkCmdBindDescriptorSets(
			vulkan.commandBuffers[i],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			bloor.material.horizontal.pipelineLayout, 0, 1, &bloor.descSetHor, 0, nullptr);

		vkCmdDraw(vulkan.commandBuffers[i], 6, 1, 0, 0);

		vkCmdEndRenderPass(vulkan.commandBuffers[i]);

		// TODO subpass Bloor Vertical
		renderPassInfo.renderPass = bloor.renderPass.vertical;
		renderPassInfo.framebuffer = bloor.framebuffer.vertical[i];

		vkCmdBeginRenderPass(vulkan.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(vulkan.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, bloor.material.vertical);

		vkCmdBindDescriptorSets(
			vulkan.commandBuffers[i],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			bloor.material.vertical.pipelineLayout, 0, 1, &bloor.descSetVer, 0, nullptr);

		vkCmdDraw(vulkan.commandBuffers[i], 6, 1, 0, 0);

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
