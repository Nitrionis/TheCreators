#include "Scene/SceneRenderer/SceneRenderer.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>
#include "ImageLoader.h"

#include <thread>

SceneRenderer::SceneRenderer() {
	try
	{
		vulkan.Initialize();
		ChunksRenderer::Initialize();
		BlurRenderer::Initialize();
		InterfaceRenderer::Initialize();
		CreateCommandBuffers();
	}
	catch (std::runtime_error e)
	{
		std::cout << std::endl << e.what() << std::endl;
		return;
	}
	DrawScene();

	vk::Window& window = vulkan.swapChain.window;

	auto startFullTime = std::chrono::high_resolution_clock::now();
	double count = 0;
	double fullTime = 0.0;

	while (window.CheckMsg()) {
		auto startTime = std::chrono::high_resolution_clock::now();

		DrawScene();

		auto endTime = std::chrono::high_resolution_clock::now();
		auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

		if (16 - dtime > 0 || 16 - dtime <= 16) {
			std::this_thread::sleep_for(std::chrono::milliseconds(16 - dtime));
		}
		count++;
		fullTime += dtime;
	}

	auto endFullTime = std::chrono::high_resolution_clock::now();
	std::cout << "Full time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endFullTime - startFullTime).count() / count << std::endl;

	//std::cout << "Full Draw Time: " << dtime.count() << std::endl;
	system("pause");
	//vulkan.ShowIntermediateImage(0);
	//vulkan.ShowIntermediateImage(1);
	//vulkan.ShowIntermediateImage(2);
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

		//vkCmdResetQueryPool(vulkan.commandBuffers[i], vulkan.queryPool, 0, 2);
		//vkCmdWriteTimestamp(vulkan.commandBuffers[i], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, vulkan.queryPool, 0);
		//vkCmdBeginQuery(vulkan.commandBuffers[i], vulkan.queryPool, 0, VK_QUERY_CONTROL_PRECISE_BIT);

		ChunksRenderer::AddToCommandBuffer(vulkan.commandBuffers[i]);

		BlurRenderer::AddToCommandBuffer(vulkan.commandBuffers[i]); // todo CHANGE IMAGE INDEX at descSet in Interface

		InterfaceRenderer::AddToCommandBuffer(vulkan.commandBuffers[i], i);

		//vkCmdEndQuery(vulkan.commandBuffers[i], vulkan.queryPool, 0);
		//vkCmdWriteTimestamp(vulkan.commandBuffers[i], VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, vulkan.queryPool, 1);

		if (vkEndCommandBuffer(vulkan.commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command obj!");
		}
	}
}

void SceneRenderer::DrawScene() {
	//auto startTime = std::chrono::high_resolution_clock::now();

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
		throw std::runtime_error("Failed to submit draw command obj!");
	}
	vulkan.swapChain.Present(vulkan.commandQueue, vulkan.renderFinishedSemaphore);

	//vkQueueWaitIdle(vulkan.commandQueue);

	/*uint64_t time[2];
	if (vkGetQueryPoolResults(
		vulkan.device,
		vulkan.queryPool,
		0, 2, 16, &time, 8,
		VK_QUERY_RESULT_WAIT_BIT
	) != VK_SUCCESS) {
		std::cout << "QueryPoolResult: FAIL     ";
	}
	std::cout << "QueryPoolResult: " << (time[1] - time[0])*vulkan.device.properties.limits.timestampPeriod << std::endl;*/
	//vulkan.
	//auto endTime = std::chrono::high_resolution_clock::now();
	//auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	//std::cout << dtime.count() << std::endl;
}

template<typename T>
T& SceneRenderer::GetComponent() {
	return dynamic_cast<T&>(*this);
}
