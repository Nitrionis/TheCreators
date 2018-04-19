#include "Scene/PostProcessing/BlurRenderer.h"
#include "Scene/SceneRenderer/RendererSettings/RendererSettings.h"
#include "BlurRenderer.h"

void BlurRenderer::CreateMaterials() {

	material.compression.viewports[0] = vk::initialize::viewportDefault({width, height});
	material.compression.scissors[0] = vk::initialize::scissorDefault({width, height});

	material.compression.pipelineLayoutInfo.setLayoutCount = 1;
	material.compression.pipelineLayoutInfo.pSetLayouts = &descSetLayout;

	material.compression.Setup(
		vulkan.device,
		renderPass,
		RendererSettings::Instance().blurComShaderNames,
		RendererSettings::Instance().blurComShaderUsage,
		SubpassIndex::zero
	);
	vk::Material::CreateMaterials(&material.compression);

	material.blur.viewports[0] = vk::initialize::viewportDefault({width, height});
	material.blur.scissors[0] = vk::initialize::scissorDefault({width, height});

	material.blur.pipelineLayoutInfo.setLayoutCount = 1;
	material.blur.pipelineLayoutInfo.pSetLayouts = &descSetLayout;

	material.blur.Setup(
		vulkan.device,
		renderPass,
		RendererSettings::Instance().blurShaderNames,
		RendererSettings::Instance().blurShaderUsage,
		SubpassIndex::zero
	);
	vk::Material::CreateMaterials(&material.blur);

	material.decompression.viewports[0] = vk::initialize::viewportDefault(vulkan.swapChain.extent);
	material.decompression.scissors[0] = vk::initialize::scissorDefault(vulkan.swapChain.extent);

	material.decompression.pipelineLayoutInfo.setLayoutCount = 1;
	material.decompression.pipelineLayoutInfo.pSetLayouts = &descSetLayout;

	material.decompression.Setup(
		vulkan.device,
		renderPass,
		RendererSettings::Instance().blurDecomShaderNames,
		RendererSettings::Instance().blurDecomShaderUsage,
		SubpassIndex::zero
	);
	vk::Material::CreateMaterials(&material.decompression);
}

void BlurRenderer::CreateDescriptorSet() {

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

	if (vkCreateDescriptorSetLayout(vulkan.device, &layoutInfo, nullptr, descSetLayout.replace()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout!");
	}

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vulkan.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descSetLayout;

	if (vkAllocateDescriptorSets(vulkan.device, &allocInfo, &descSet.compression) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate compression descriptor set!");
	}
	if (vkAllocateDescriptorSets(vulkan.device, &allocInfo, &descSet.horizontal) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate horizontal descriptor set!");
	}
	if (vkAllocateDescriptorSets(vulkan.device, &allocInfo, &descSet.vertical) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate vertical descriptor set!");
	}

	VkDescriptorImageInfo imageInfos[3];

	imageInfos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfos[0].imageView = vulkan.image.intermediate[0].view;
	imageInfos[0].sampler = sampler;

	imageInfos[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfos[1].imageView = vulkan.image.intermediate[1].view;
	imageInfos[1].sampler = sampler;

	imageInfos[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfos[2].imageView = vulkan.image.intermediate[2].view;
	imageInfos[2].sampler = sampler;

	std::array<VkWriteDescriptorSet, 3> descWrites;

	descWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrites[0].pNext = nullptr;
	descWrites[0].dstSet = descSet.compression;
	descWrites[0].dstBinding = 0;
	descWrites[0].dstArrayElement = 0;
	descWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descWrites[0].descriptorCount = 1;
	descWrites[0].pImageInfo = &imageInfos[0];

	descWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrites[1].pNext = nullptr;
	descWrites[1].dstSet = descSet.horizontal;
	descWrites[1].dstBinding = 0;
	descWrites[1].dstArrayElement = 0;
	descWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descWrites[1].descriptorCount = 1;
	descWrites[1].pImageInfo = &imageInfos[1];

	descWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrites[2].pNext = nullptr;
	descWrites[2].dstSet = descSet.vertical;
	descWrites[2].dstBinding = 0;
	descWrites[2].dstArrayElement = 0;
	descWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descWrites[2].descriptorCount = 1;
	descWrites[2].pImageInfo = &imageInfos[2];

	vkUpdateDescriptorSets(vulkan.device, descWrites.size(), descWrites.data(), 0, nullptr);
}

void BlurRenderer::CreateSamplers() {

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

void BlurRenderer::Initialize() {
	std::cout <<"\n***************************************\n";
	std::cout <<  "*     BlurRenderer::Initialize()      *\n";
	std::cout <<  "***************************************\n\n";

	width = vulkan.swapChain.extent.width / 16;
	height = vulkan.swapChain.extent.height / 16;
	CreateRenderPasses();
	CreateFramebuffers();
	CreateSamplers();
	CreateDescriptorSet();
	CreateMaterials();
}

void BlurRenderer::CreateRenderPasses() {

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

	renderPass.colorAttachments.push_back(colorAttachment);

	VkAttachmentReference attachmentRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

	VkSubpassDescription description = {};
	description.flags = VK_FLAGS_NONE;
	description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	vk::Subpass subpass;
	subpass.colorAttachmentRefs.push_back(attachmentRef);
	subpass.description = description;

	renderPass.subpasses.push_back(subpass);

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

	renderPass.dependencies = dependencies;

	renderPass.DoFinalInitialise();
}

void BlurRenderer::CreateFramebuffers() {

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.width = vulkan.swapChain.extent.width;
	framebufferInfo.height = vulkan.swapChain.extent.height;
	framebufferInfo.layers = 1;
	framebufferInfo.attachmentCount = 1;

	framebufferInfo.pAttachments = &vulkan.image.intermediate[1].view;

	if (vkCreateFramebuffer(vulkan.device, &framebufferInfo, nullptr, framebuffer.firstTarget.replace()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer!");
	}

	framebufferInfo.pAttachments = &vulkan.image.intermediate[2].view;

	if (vkCreateFramebuffer(vulkan.device, &framebufferInfo, nullptr, framebuffer.secondTarget.replace()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer!");
	}
}

void BlurRenderer::AddToCommandBuffer(vk::CommandBuffer commandBuffer) {

	std::array<VkClearValue, 1> clearColors = {VkClearValue{0.000f, 1.000f, 0.000f, 1.0f}};

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = {width, height};
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.clearValueCount = clearColors.size();
	renderPassInfo.pClearValues = clearColors.data();

	// TODO subpass Blur Compression
	renderPassInfo.framebuffer = framebuffer.firstTarget;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.compression);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		material.compression.pipelineLayout, 0, 1, &descSet.compression, 0, nullptr);

	vkCmdDraw(commandBuffer, 6, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	// TODO subpass Blur Horizontal
	renderPassInfo.framebuffer = framebuffer.secondTarget;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.blur);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		material.blur.pipelineLayout, 0, 1, &descSet.horizontal, 0, nullptr);

	vkCmdDraw(commandBuffer, 6, 1, 0, 0); // TODO

	vkCmdEndRenderPass(commandBuffer);

	// TODO subpass Blur Vertical
	renderPassInfo.framebuffer = framebuffer.firstTarget;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	//vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.blur);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		material.blur.pipelineLayout, 0, 1, &descSet.vertical, 0, nullptr);

	vkCmdDraw(commandBuffer, 6, 1, 0, 1); // TODO

	vkCmdEndRenderPass(commandBuffer);

	// TODO subpass Blur Horizontal
	/*renderPassInfo.framebuffer = framebuffer.secondTarget;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.horizontal);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		material.horizontal.pipelineLayout, 0, 1, &descSet.horizontal, 0, nullptr);

	vkCmdDraw(commandBuffer, 6, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);*/

	// TODO subpass Blur Vertical
	/*renderPassInfo.framebuffer = framebuffer.firstTarget;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.vertical);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		material.vertical.pipelineLayout, 0, 1, &descSet.vertical, 0, nullptr);

	vkCmdDraw(commandBuffer, 6, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);*/

	// TODO subpass Blur Decompression
	renderPassInfo.renderArea.extent = vulkan.swapChain.extent;

	renderPassInfo.framebuffer = framebuffer.secondTarget;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.decompression);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		material.decompression.pipelineLayout, 0, 1, &descSet.horizontal, 0, nullptr);

	vkCmdDraw(commandBuffer, 6, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);
}