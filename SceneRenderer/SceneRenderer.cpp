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
		blur.Initialize();
		CreateCommandBuffers();
		//ImageLoader imageLoader("C:\\Developer\\JetBrains\\Clion\\Proj\\TheCreators\\Textures\\texture.png");
	}
	catch (std::runtime_error e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
	DrawScene();
	/*DrawScene();
	DrawScene();
	DrawScene();
	DrawScene();
	DrawScene();*/
	vulkan.ShowIntermediateImage(0);
	vulkan.ShowIntermediateImage(1);
	vulkan.ShowIntermediateImage(2);
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
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vkBeginCommandBuffer(vulkan.commandBuffers[i], &beginInfo);

		chunks.AddToCommandBuffer(vulkan.commandBuffers[i]);

		blur.AddToCommandBuffer(vulkan.commandBuffers[i]);

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
	submitInfo.signalSemaphoreCount = 0; // TODO 1
	submitInfo.pSignalSemaphores = &vulkan.renderFinishedSemaphore;

	if (vkQueueSubmit(vulkan.commandQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}
	//vulkan.swapChain.Present(vulkan.commandQueue, vulkan.renderFinishedSemaphore);

	auto endTime = std::chrono::high_resolution_clock::now();
	auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::cout << dtime.count() << std::endl;
}