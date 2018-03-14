#include "SceneRenderer.h"
#include "ImageLoader.h"

void SceneRenderer::Chunks::Initialize() {
	CreateRenderPasses();
	CreateFramebuffers();
	CreateAtlasImage();
	CreateAtlasImageView();
	CreateSamplers();
	CreateDescriptorSet();
	CreateMaterialGround();
}

void SceneRenderer::Chunks::CreateMaterialGround() {

	material.ground.viewports[0] = vk::initialize::viewportDefault(vulkan.swapChain.extent);
	material.ground.scissors[0] = vk::initialize::scissorDefault(vulkan.swapChain.extent);

	material.ground.pipelineLayoutInfo.setLayoutCount = 1;
	material.ground.pipelineLayoutInfo.pSetLayouts = &descSetLayout;

	material.ground.Setup(
		vulkan.device,
		renderPass,
		settings.chunkShaderNames,
		settings.chunkShaderUsage,
		0
	);
	vk::Material::CreateMaterials(&material.ground);
}

void SceneRenderer::Chunks::CreateAtlasImage() {

	ImageLoader imageLoader("C:\\Developer\\JetBrains\\Clion\\Proj\\TheCreators\\Textures\\texture.png");

	VkExtent3D imageExtent = {
		static_cast<uint32_t>(imageLoader.width),
		static_cast<uint32_t>(imageLoader.height),
		1
	};
	vulkan.device.CreateImage(
		&image.atlas,
		imageExtent,
		imageLoader.size,
		imageLoader.pixels
	);
}

void SceneRenderer::Chunks::CreateAtlasImageView() {

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image.atlas.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(vulkan.device, &viewInfo, nullptr, &image.atlas.view) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture textureImage view!");
	}
}

void SceneRenderer::Chunks::CreateSamplers() {

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

void SceneRenderer::Chunks::CreateDescriptorSet() {

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

	if (vkAllocateDescriptorSets(vulkan.device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor set!");
	}

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = image.atlas.view;
	imageInfo.sampler = sampler;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(vulkan.device, 1, &descriptorWrite, 0, nullptr);
}

void SceneRenderer::Chunks::CreateRenderPasses() {

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

	vk::Subpass subpass;
	subpass.colorAttachmentRefs.push_back(attachmentRef);
	VkSubpassDescription description = {};
	description.flags = VK_FLAGS_NONE;
	description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
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

void SceneRenderer::Chunks::CreateFramebuffers() {

	std::array<VkImageView, 1> attachments = {vulkan.image.intermediate[0].view};

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = vulkan.swapChain.extent.width;
	framebufferInfo.height = vulkan.swapChain.extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(vulkan.device, &framebufferInfo, nullptr, framebuffer.replace()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer!");
	}
}
