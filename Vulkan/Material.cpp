#include "Material.h"
#include <fstream>

vk::Material::Material() {}

vk::Material::~Material() { Destroy(); }

void vk::Material::Setup(
	VkDevice device,
	vk::RenderPass& renderPass,
	vk::MaterialBehavior materialBehavior,
	std::vector<const char*>& shaderFileNames,
	std::vector<VkShaderStageFlagBits>& shaderUsage,
	size_t subpassIndex)
{
	this->device = device;

	shaders.resize(shaderFileNames.size());

	for (int i = 0; i < shaderFileNames.size(); i++) {
		auto code = LoadFile(shaderFileNames[i]);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaders[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module!");
		}
	}

	std::vector<VkPipelineShaderStageCreateInfo> stagesInfo(shaderUsage.size());
	for (int i = 0; i < stagesInfo.size(); i++) {
		stagesInfo[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stagesInfo[i].pNext = NULL;
		stagesInfo[i].flags = 0;
		stagesInfo[i].pSpecializationInfo = NULL;
		stagesInfo[i].stage = shaderUsage[i];
		stagesInfo[i].module = shaders[i];
		stagesInfo[i].pName = "main";
	}

	if (vkCreatePipelineLayout(device, &materialBehavior.pipelineLayout, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout!");
	}

	/*VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)800,
		.height = (float)600,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = { 800, 600 }
	};
	VkPipelineViewportStateCreateInfo viewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor
	};*/

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = stagesInfo.size();
	pipelineInfo.pStages = stagesInfo.data();
	pipelineInfo.pVertexInputState = &materialBehavior.vertexInput;
	pipelineInfo.pInputAssemblyState = &materialBehavior.inputAssembly;
	pipelineInfo.pViewportState = &materialBehavior.viewportState;//&viewportState;
	pipelineInfo.pRasterizationState = &materialBehavior.rasterizer;
	pipelineInfo.pMultisampleState = &materialBehavior.multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &materialBehavior.colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = subpassIndex;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1; // Optional


	{
		//pipelineInfo.pViewportState->pScissors->offset.x = 0;
	}

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

void vk::Material::CreatePSO() {

}

void vk::Material::Destroy() {
	if (pipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(device, pipeline, nullptr);
		pipeline = VK_NULL_HANDLE;
	}
	if (pipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;
	}
	for (int i = 0; i < shaders.size(); i++) {
		if (shaders[i] != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, shaders[i], nullptr);
			shaders[i] = VK_NULL_HANDLE;
		}
	}
}

std::vector<char> vk::Material::LoadFile(const char* filename) {
	std::ifstream file(directoryPath + filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("LoadFile: failed to open file!");
	}
	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

VkViewport vk::initialize::viewportDefault(VkExtent2D extent) {
	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)extent.width,
		.height = (float)extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	return viewport;
}

VkRect2D vk::initialize::scissorDefault(VkExtent2D extent) {
	VkRect2D scissor = {
		.offset = {0, 0},
		.extent = extent
	};
	return scissor;
}
