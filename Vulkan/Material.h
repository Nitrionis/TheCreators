#pragma once
#include "VulkanSharedHaders.h"

namespace vk
{
	namespace initialize
	{
		VkViewport viewportDefault(VkExtent2D extent);
		VkRect2D scissorDefault(VkExtent2D extent);
	};

	struct MaterialBehavior
	{
		friend class Material;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT
		};
		VkPipelineDynamicStateCreateInfo dynamicState = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
			.pDynamicStates = dynamicStates.data()
		};
		std::vector<VkViewport> viewports = {
			VkViewport {
				.x = 0.0f,
				.y = 0.0f,
				.width = 0.0f,
				.height = 0.0f,
				.minDepth = 0.0f,
				.maxDepth = 1.0f
			}
		};
		std::vector<VkRect2D> scissors = {
			VkRect2D {
				.offset = {0, 0},
				.extent = {0, 0}
			}
		};
	private:
		VkPipelineViewportStateCreateInfo viewportState = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.viewportCount = static_cast<uint32_t>(viewports.size()),
			.pViewports = viewports.data(),
			.scissorCount = static_cast<uint32_t>(scissors.size()),
			.pScissors = scissors.data()
		};
	public:
		VkPipelineVertexInputStateCreateInfo vertexInput = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.vertexBindingDescriptionCount = 0,
			.pVertexBindingDescriptions = nullptr, // Optional
			.vertexAttributeDescriptionCount = 0,
			.pVertexAttributeDescriptions = nullptr // Optional
		};
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};
		VkPipelineRasterizationStateCreateInfo  rasterizer = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.depthClampEnable = VK_FALSE, // need enable GPU function
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL, // for other need enable GPU function
			.cullMode = VK_CULL_MODE_NONE,//VK_CULL_MODE_BACK_BIT,// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f, // Optional
			.depthBiasClamp = 0.0f, // Optional
			.depthBiasSlopeFactor = 0.0f, // Optional
			.lineWidth = 1.0f // for other need enable GPU function
		};
		VkPipelineMultisampleStateCreateInfo multisampling = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.0f, // Optional
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE, // Optional
			.alphaToOneEnable = VK_FALSE // Optional
		};
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = {
			VkPipelineColorBlendAttachmentState {
				.blendEnable = VK_FALSE,
				.srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
				.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
				.colorBlendOp = VK_BLEND_OP_ADD, // Optional
				.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
				.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
				.alphaBlendOp = VK_BLEND_OP_ADD, // Optional
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			}
		};
		VkPipelineColorBlendStateCreateInfo colorBlending = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY, // Optional
			.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size()),
			.pAttachments = colorBlendAttachments.data(),
			.blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f } // Optional
		};
		VkPipelineLayoutCreateInfo pipelineLayout = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};
	};

	class RenderPass {
		friend class Material;
	private:
		VkDevice device = VK_NULL_HANDLE;
		VkRenderPass renderPass = VK_NULL_HANDLE;
	public:

		operator VkRenderPass() const {
			return renderPass;
		}

		const VkRenderPass* operator &() const {
			return &renderPass;
		}

		~RenderPass()
		{
			if (renderPass != VK_NULL_HANDLE) {
				vkDestroyRenderPass(device, renderPass, nullptr);
				renderPass = VK_NULL_HANDLE;
			}
		}
		void Setup(VkDevice device)
		{
			this->device = device;
			if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create render pass!");
			}
		}
		/********************************************************\
		 *                Render Pass Settings                  *
		\********************************************************/

		std::vector<VkAttachmentDescription> colorAttachments = {
			VkAttachmentDescription {
				.flags = 0,
				.format = VK_FORMAT_B8G8R8A8_UNORM,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp =  VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp =  VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout =   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			}
		};
		std::vector<VkAttachmentReference> colorAttachmentRefs = {
			VkAttachmentReference {
				.attachment = 0,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			}
		};
		std::vector<VkSubpassDescription> subPasses = {
			VkSubpassDescription {
				.flags = 0,
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount = 0,
				.pInputAttachments = nullptr,
				.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size()),
				.pColorAttachments = colorAttachmentRefs.data()
			}
		};
		std::vector<VkSubpassDependency> dependencies = {
			VkSubpassDependency {
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = 0,
				.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask = 0,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = 0
			}
		};
		VkRenderPassCreateInfo renderPassInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.attachmentCount = static_cast<uint32_t>(colorAttachments.size()),
			.pAttachments = colorAttachments.data(),
			.subpassCount = static_cast<uint32_t>(subPasses.size()),
			.pSubpasses = subPasses.data(),
			.dependencyCount = static_cast<uint32_t>(dependencies.size()),
			.pDependencies = dependencies.data()
		};
	};

	class Material {
	private:
		VkDevice device = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		std::vector<VkShaderModule> shaders;

		std::string directoryPath = "C:\\Developer\\JetBrains\\Clion\\Proj\\TheCreators\\";
	public:

		operator VkPipeline() const {
			return pipeline;
		}

		const VkPipeline* operator &() const {
			return &pipeline;
		}

		Material();
		~Material();
		void Setup(
			VkDevice device,
			vk::RenderPass& renderPass,
			vk::MaterialBehavior materialBehavior,
			std::vector<const char*>& shaderFileNames,
			std::vector<VkShaderStageFlagBits>& shaderUsage,
			size_t subpassIndex
		);
		void Destroy();

	private:
		std::vector<char> LoadFile(const char* filename);
		void CreatePSO();
	};
}