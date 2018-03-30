#include "Material.h"
#include <fstream>

vk::RenderPass::RenderPass(VkDevice* device) {
	this->device = device;
}

void vk::RenderPass::DoFinalInitialise() {

	std::vector<VkSubpassDescription> subPassesData(subpasses.size());
	for (int i = 0; i < subpasses.size(); i++) {
		subpasses[i].description.inputAttachmentCount = static_cast<uint32_t >(subpasses[i].inputAttachmentsRefs.size());
		subpasses[i].description.pInputAttachments = subpasses[i].inputAttachmentsRefs.data();
		subpasses[i].description.colorAttachmentCount = static_cast<uint32_t >(subpasses[i].colorAttachmentRefs.size());
		subpasses[i].description.pColorAttachments = subpasses[i].colorAttachmentRefs.data();
		subPassesData[i] = subpasses[i].description;
	}
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.flags = VK_FLAGS_NONE;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(colorAttachments.size());
	renderPassInfo.pAttachments = colorAttachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subPassesData.size());
	renderPassInfo.pSubpasses = subPassesData.data();
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();

	if (vkCreateRenderPass(*device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create render pass!");
	}
}

void vk::RenderPass::Reset() {
	colorAttachments.clear();
	for (int i = 0; i < subpasses.size(); i++) {
		subpasses[i].inputAttachmentsRefs.clear();
		subpasses[i].colorAttachmentRefs.clear();
	}
	dependencies.clear();
}

void vk::RenderPass::InitializeDefault() {

	VkAttachmentDescription colorAttachment = {};

	colorAttachment.flags = VK_FLAGS_NONE;
	colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp =  VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout =   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	colorAttachments.push_back(colorAttachment);

	Subpass subpass;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	subpass.colorAttachmentRefs.push_back(colorAttachmentRef);

	VkSubpassDescription description = {};
	description.flags = VK_FLAGS_NONE;
	description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	description.inputAttachmentCount = static_cast<uint32_t>(subpass.inputAttachmentsRefs.size());
	description.pInputAttachments = subpass.inputAttachmentsRefs.data();
	description.colorAttachmentCount = static_cast<uint32_t>(subpass.colorAttachmentRefs.size());
	description.pColorAttachments = subpass.colorAttachmentRefs.data();

	subpass.description = description;

	subpasses.push_back(subpass);

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dependencyFlags = 0;
	dependencies.push_back(dependency);
}

vk::Material::Material() {

	VkViewport defaultViewport;
		defaultViewport.x = 0.0f;
		defaultViewport.y = 0.0f;
		defaultViewport.width = 0.0f;
		defaultViewport.height = 0.0f;
		defaultViewport.minDepth = 0.0f;
		defaultViewport.maxDepth = 1.0f;
	viewports.push_back(defaultViewport);

	VkRect2D defaultScissor;
		defaultScissor.offset = {0, 0};
		defaultScissor.extent = {0, 0};
	scissors.push_back(defaultScissor);

	vertexInputInfo.vertexBindingDescriptionCount   = 0;
	vertexInputInfo.pVertexBindingDescriptions      = nullptr; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions    = nullptr; // Optional

	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	rasterizerInfo.depthClampEnable        = VK_FALSE; // for other need enable GPU function
	rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL; // for other need enable GPU function
	rasterizerInfo.cullMode  = VK_CULL_MODE_NONE;      // TODO mb VK_CULL_MODE_BACK_BIT,
	rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizerInfo.depthBiasEnable = VK_FALSE;
	rasterizerInfo.depthBiasConstantFactor = 0.0f; // Optional
	rasterizerInfo.depthBiasClamp          = 0.0f; // Optional
	rasterizerInfo.depthBiasSlopeFactor    = 0.0f; // Optional
	rasterizerInfo.lineWidth               = 1.0f; // for other need enable GPU function

	multisamplingInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisamplingInfo.sampleShadingEnable   = VK_FALSE;
	multisamplingInfo.minSampleShading      = 1.0f;     // Optional
	multisamplingInfo.pSampleMask           = nullptr;
	multisamplingInfo.alphaToCoverageEnable = VK_FALSE; // Optional
	multisamplingInfo.alphaToOneEnable      = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState defaulColorBlendAttachment;
		defaulColorBlendAttachment.blendEnable = VK_FALSE;
		defaulColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		defaulColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		defaulColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		defaulColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		defaulColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		defaulColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
		defaulColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachments.push_back(defaulColorBlendAttachment);

	colorBlendingInfo.logicOpEnable = VK_FALSE;
	colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlendingInfo.blendConstants[0] = 0;
	colorBlendingInfo.blendConstants[1] = 0;
	colorBlendingInfo.blendConstants[2] = 0;
	colorBlendingInfo.blendConstants[3] = 0;

	pipelineLayoutInfo.setLayoutCount         = 0;
	pipelineLayoutInfo.pSetLayouts            = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges    = nullptr;
}

vk::Material::~Material() { Destroy(); }

void vk::Material::Setup(
	VkDevice device,
	vk::RenderPass& renderPass,
	std::vector<const char*>& shaderFileNames,
	std::vector<VkShaderStageFlagBits>& shaderUsage,
	SubpassIndex subpassIndex)
{
	this->device = device;

	ConnectObjects();

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
	stagesInfo.resize(shaderUsage.size());
	for (int i = 0; i < stagesInfo.size(); i++) {
		stagesInfo[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stagesInfo[i].pNext = NULL;
		stagesInfo[i].flags = 0;
		stagesInfo[i].pSpecializationInfo = NULL;
		stagesInfo[i].stage = shaderUsage[i];
		stagesInfo[i].module = shaders[i];
		stagesInfo[i].pName = "main";
	}
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout!");
	}
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount          = stagesInfo.size();
	pipelineInfo.pStages             = stagesInfo.data();
	pipelineInfo.pVertexInputState   = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState      = &viewportInfo;
	pipelineInfo.pRasterizationState = &rasterizerInfo;
	pipelineInfo.pMultisampleState   = &multisamplingInfo;
	pipelineInfo.pDepthStencilState  = nullptr;
	pipelineInfo.pColorBlendState    = &colorBlendingInfo;
	pipelineInfo.pDynamicState       = nullptr;
	pipelineInfo.layout              = pipelineLayout;
	pipelineInfo.renderPass          = renderPass;
	pipelineInfo.subpass             = static_cast<uint32_t>(subpassIndex);
	pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex   = -1;
}

void vk::Material::CreateMaterials(vk::Material *materials, uint32_t size) {

	std::vector<VkPipeline> pipelines(size);
	std::vector<VkGraphicsPipelineCreateInfo> pipelineInfos(size);
	for (uint32_t i = 0; i < size; i++) {
		pipelineInfos[i] = materials[i].pipelineInfo;
	}
	if (vkCreateGraphicsPipelines(materials->device, VK_NULL_HANDLE, size, pipelineInfos.data(), nullptr, pipelines.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create graphics pipelines!");
	}
	for (uint32_t i = 0; i < size; i++) {
		materials[i].pipeline = pipelines[i];
	}
}

void vk::Material::ConnectObjects() {

	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.pNext = nullptr;
	dynamicStateInfo.flags = VK_FLAGS_NONE;
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateInfo.pDynamicStates    = dynamicStates.data();

	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.pNext = nullptr;
	viewportInfo.flags = VK_FLAGS_NONE;
	viewportInfo.viewportCount = static_cast<uint32_t>(viewports.size());
	viewportInfo.pViewports    = viewports.data();
	viewportInfo.scissorCount  = static_cast<uint32_t>(scissors.size());
	viewportInfo.pScissors     = scissors.data();

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pNext = nullptr;
	vertexInputInfo.flags = VK_FLAGS_NONE;

	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.pNext = nullptr;
	inputAssemblyInfo.flags = VK_FLAGS_NONE;

	rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerInfo.pNext = nullptr;
	rasterizerInfo.flags = VK_FLAGS_NONE;

	multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingInfo.pNext = nullptr;
	multisamplingInfo.flags = VK_FLAGS_NONE;

	colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingInfo.pNext = nullptr;
	colorBlendingInfo.flags = VK_FLAGS_NONE;
	colorBlendingInfo.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
	colorBlendingInfo.pAttachments    = colorBlendAttachments.data();

	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = nullptr;
	pipelineLayoutInfo.flags = VK_FLAGS_NONE;
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
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	return viewport;
}

VkRect2D vk::initialize::scissorDefault(VkExtent2D extent) {
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = extent;
	return scissor;
}
