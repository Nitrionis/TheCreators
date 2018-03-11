#include "SceneRenderer.h"

void SceneRenderer::Bloor::CreateMaterials() {

	material.horizontal.viewports[0] = vk::initialize::viewportDefault(vulkan.swapChain.extent);
	material.horizontal.scissors[0] = vk::initialize::scissorDefault(vulkan.swapChain.extent);

	material.horizontal.pipelineLayoutInfo.setLayoutCount = 1;
	material.horizontal.pipelineLayoutInfo.pSetLayouts = &descSetLayoutHor;

	material.horizontal.colorBlendAttachments.push_back(material.horizontal.colorBlendAttachments[0]);

	material.horizontal.Setup(
		vulkan.device,
		vulkan.renderPass,
		settings.bloorHorShaderNames,
		settings.bloorHorShaderUsage,
		(int)Stage::BloorHor
	);
	vk::Material::CreateMaterials(&material.horizontal, 1);


	material.vertical.viewports[0] = vk::initialize::viewportDefault(vulkan.swapChain.extent);
	material.vertical.scissors[0] = vk::initialize::scissorDefault(vulkan.swapChain.extent);

	material.vertical.pipelineLayoutInfo.setLayoutCount = 1;
	material.vertical.pipelineLayoutInfo.pSetLayouts = &descSetLayoutVer;

	material.vertical.colorBlendAttachments.push_back(material.vertical.colorBlendAttachments[0]);

	material.vertical.Setup(
		vulkan.device,
		vulkan.renderPass,
		settings.bloorVerShaderNames,
		settings.bloorVerShaderUsage,
		(int)Stage::BloorVer
	);
	vk::Material::CreateMaterials(&material.vertical, 1);
}

void SceneRenderer::Bloor::CreateDescriptorSet() {

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};

	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};

	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &samplerLayoutBinding;

	if (vkCreateDescriptorSetLayout(vulkan.device, &layoutInfo, nullptr, descSetLayoutHor.replace()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout!");
	}


	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &samplerLayoutBinding;

	if (vkCreateDescriptorSetLayout(vulkan.device, &layoutInfo, nullptr, descSetLayoutVer.replace()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout!");
	}


	VkDescriptorSetAllocateInfo allocInfo = {};

	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vulkan.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descSetLayoutHor;

	if (vkAllocateDescriptorSets(vulkan.device, &allocInfo, &descSetHor) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor set!");
	}

	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vulkan.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descSetLayoutVer;

	if (vkAllocateDescriptorSets(vulkan.device, &allocInfo, &descSetVer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor set!");
	}


	VkDescriptorImageInfo imageInfos[2];

	imageInfos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfos[0].imageView = vulkan.image.intermediate[0].view;
	imageInfos[0].sampler = sampler;

	imageInfos[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfos[1].imageView = vulkan.image.intermediate[1].view;
	imageInfos[1].sampler = sampler;

	std::array<VkWriteDescriptorSet, 2> descWrites;

	descWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrites[0].pNext = nullptr;
	descWrites[0].dstSet = descSetHor;
	descWrites[0].dstBinding = 0;
	descWrites[0].dstArrayElement = 0;
	descWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descWrites[0].descriptorCount = 1;
	descWrites[0].pImageInfo = &imageInfos[0];

	descWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrites[1].pNext = nullptr;
	descWrites[1].dstSet = descSetVer;
	descWrites[1].dstBinding = 0;
	descWrites[1].dstArrayElement = 0;
	descWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descWrites[1].descriptorCount = 1;
	descWrites[1].pImageInfo = &imageInfos[1];

	vkUpdateDescriptorSets(vulkan.device, descWrites.size(), descWrites.data(), 0, nullptr);
}

void SceneRenderer::Bloor::CreateSamplers() {

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(vulkan.device, &samplerInfo, nullptr, sampler.replace()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture sampler!");
	}
}

void SceneRenderer::Bloor::Initialize() {
	CreateSamplers();
	CreateDescriptorSet();
	CreateMaterials();
}

void SceneRenderer::Bloor::CreateRenderPasses() {

	VkAttachmentDescription colorAttachment = {};

	colorAttachment.flags = VK_FLAGS_NONE;
	colorAttachment.format = vulkan.swapChain.colorFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp =  VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout =   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	renderPass.horizontal.colorAttachments.push_back(colorAttachment);

	colorAttachment.flags = VK_FLAGS_NONE;
	colorAttachment.format = vulkan.swapChain.colorFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp =  VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout =   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	renderPass.vertical.colorAttachments.push_back(colorAttachment);

	VkAttachmentReference attachmentRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

	vk::Subpass subpass;
	subpass.colorAttachmentRefs.push_back(attachmentRef);
	subpass.description.flags = VK_FLAGS_NONE;
	subpass.description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	renderPass.horizontal.subpasses.push_back(subpass);
	renderPass.vertical.subpasses.push_back(subpass);

	std::vector<VkSubpassDependency> dependencies(2);

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	renderPass.horizontal.dependencies = dependencies;
	renderPass.vertical.dependencies = dependencies;

	renderPass.horizontal.DoFinalInitialise();
	renderPass.vertical.DoFinalInitialise();
}

void SceneRenderer::Bloor::CreateFramebuffers() {

	VkImageView attachments[] = {vulkan.image.intermediate[1].view};

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass.horizontal;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = vulkan.swapChain.extent.width;
	framebufferInfo.height = vulkan.swapChain.extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(vulkan.device, &framebufferInfo, nullptr, framebuffer.horizontal.replace()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer!");
	}

	vulkan.framebuffers.resize(vulkan.swapChain.imageCount, vk::UniqueFramebuffer{vulkan.device, vkDestroyFramebuffer});
	for (size_t i = 0; i < vulkan.swapChain.imageCount; i++)
	{
		VkImageView attachments[] = {vulkan.swapChain.images[i]};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = vulkan.renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = vulkan.swapChain.extent.width;
		framebufferInfo.height = vulkan.swapChain.extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(vulkan.device, &framebufferInfo, nullptr, vulkan.framebuffers[i].replace()) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}
