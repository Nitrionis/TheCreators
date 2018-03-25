#include "ChunkRenderer.h"
#include "RendererSettings.h"
#include "ImageLoader.h"

void ChunkRenderer::Initialize() {
	mesh.Initialize();

	CreateRenderPasses();
	CreateFramebuffers();
	CreateAtlasImage();
	CreateAtlasImageView();
	CreateSamplers();
	CreateDescriptorSet();
	CreateMaterialGround();
}

void ChunkRenderer::CreateMaterialGround() {

	material.ground.viewports[0] = vk::initialize::viewportDefault(vulkan.swapChain.extent);
	material.ground.scissors[0] = vk::initialize::scissorDefault(vulkan.swapChain.extent);

	material.ground.pipelineLayoutInfo.setLayoutCount = 1;
	material.ground.pipelineLayoutInfo.pSetLayouts = &descSetLayout;

	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(uint32_t);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions = {};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32_UINT;
	attributeDescriptions[0].offset = 0;

	material.ground.vertexInputInfo.vertexBindingDescriptionCount = 1;
	material.ground.vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	material.ground.vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
	material.ground.vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	material.ground.Setup(
		vulkan.device,
		renderPass,
		RendererSettings::Instance().chunkShaderNames,
		RendererSettings::Instance().chunkShaderUsage,
		0
	);
	vk::Material::CreateMaterials(&material.ground);
}

void ChunkRenderer::CreateAtlasImage() {

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

void ChunkRenderer::CreateAtlasImageView() {

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

void ChunkRenderer::CreateSamplers() {

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

void ChunkRenderer::CreateDescriptorSet() {

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

void ChunkRenderer::CreateRenderPasses() {

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

void ChunkRenderer::CreateFramebuffers() {

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

void ChunkRenderer::AddToCommandBuffer(vk::CommandBuffer commandBuffer) {

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = framebuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = vulkan.swapChain.extent;

	std::array<VkClearValue, 1> clearColors = {VkClearValue{1.000f, 0.000f, 0.000f, 1.0f}};
	renderPassInfo.clearValueCount = clearColors.size();
	renderPassInfo.pClearValues = clearColors.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.ground);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		material.ground.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

	VkBuffer vertexBuffers[] = {mesh.buffer.vertices.buffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, mesh.buffer.indices.buffer, 0, VK_INDEX_TYPE_UINT16);

	vkCmdDrawIndexed(commandBuffer, 3, 1, 0, 3, 0);

	vkCmdEndRenderPass(commandBuffer);
}

void ChunkRenderer::Mesh::CreateBuffers() {

	vulkan.device.CreateBuffer(
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&buffer.vertices,
		441*65536*4
	);
	vulkan.device.CreateBuffer(
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&buffer.indices,
		441*65536*2*12
	);
	vulkan.device.CreateBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		&buffer.staging.vertices,
		65536*4
	);
	vulkan.device.CreateBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		&buffer.staging.indices,
		65536*2*12
	);
}

void ChunkRenderer::Mesh::CreateDate() {
	vertices = vk::shared_array<uint32_t>(new uint32_t[4]);
	vertices[0] = 0;
	vertices[1] = 1;
	vertices[2] = 2;
	vertices[3] = 3;

	buffer.staging.vertices.Map();
	buffer.staging.vertices.CopyFrom(vertices.get(), 4*sizeof(uint32_t));
	buffer.staging.vertices.Unmap();

	buffer.staging.vertices.Flush();
	buffer.staging.vertices.Invalidate();

	vulkan.device.CopyBuffer(
		&buffer.staging.vertices,
		&buffer.vertices
	);

	indices = vk::shared_array<uint16_t>(new uint16_t[6]);
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 1;
	indices[5] = 2;

	buffer.staging.indices.Map();
	buffer.staging.indices.CopyFrom(indices.get(), 6*sizeof(uint16_t));
	buffer.staging.indices.Unmap();

	buffer.staging.indices.Flush();
	buffer.staging.indices.Invalidate();

	vulkan.device.CopyBuffer(
		&buffer.staging.indices,
		&buffer.indices
	);
}

void ChunkRenderer::Mesh::Initialize() {
	CreateBuffers();
	CreateDate();
}
