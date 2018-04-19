#pragma once
#include "Vulkan/Other/VulkanSharedHaders.h"
#include "Vulkan/Mesh/Mesh.h"
#include "Scene/SceneRenderer/SubpassIndex.h"

namespace vk
{
	namespace initialize
	{
		VkViewport viewportDefault(VkExtent2D extent);
		VkRect2D scissorDefault(VkExtent2D extent);
	};

	struct Subpass{
		std::vector<VkAttachmentReference>    inputAttachmentsRefs;
		std::vector<VkAttachmentReference>    colorAttachmentRefs;
		VkSubpassDescription                  description;
	};

	class RenderPass {
		friend class Material;
	private:
		VkDevice* device = nullptr;
		VkRenderPass renderPass = VK_NULL_HANDLE;
	public:
		std::vector<VkAttachmentDescription>  colorAttachments;
		std::vector<Subpass>                  subpasses;
		std::vector<VkSubpassDependency>      dependencies;

		operator VkRenderPass() const {
			if (renderPass == VK_NULL_HANDLE)
				throw std::runtime_error("Runtime: RenderPass usage without initialisation!");
			return renderPass;
		}
		VkRenderPass* operator &() const {
			if (renderPass == VK_NULL_HANDLE)
				throw std::runtime_error("Runtime: RenderPass usage without initialisation!");
			return (VkRenderPass*)&renderPass;
		}
		RenderPass(VkDevice* device);
		~RenderPass() {
			if (renderPass != VK_NULL_HANDLE) {
				vkDestroyRenderPass(*device, renderPass, nullptr);
				renderPass = VK_NULL_HANDLE;
			}
		}
		void InitializeDefault();
		void DoFinalInitialise();
		void Reset();
	};

	class Material {
	private:
		VkDevice device = VK_NULL_HANDLE;

		VkPipeline pipeline = VK_NULL_HANDLE;
		VkGraphicsPipelineCreateInfo pipelineInfo = {};

		std::vector<VkShaderModule> shaders;
		std::vector<VkPipelineShaderStageCreateInfo> stagesInfo;

		std::string directoryPath = "C:\\Developer\\JetBrains\\Clion\\Proj\\TheCreators\\";
	public:
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		std::vector<VkDynamicState> dynamicStates;
		std::vector<VkViewport> viewports;
		std::vector<VkRect2D> scissors;

		VkPipelineDynamicStateCreateInfo        dynamicStateInfo   = {};
		VkPipelineViewportStateCreateInfo       viewportInfo       = {};
		VkPipelineVertexInputStateCreateInfo    vertexInputInfo    = {};
		VkPipelineInputAssemblyStateCreateInfo  inputAssemblyInfo  = {};
		VkPipelineRasterizationStateCreateInfo  rasterizerInfo     = {};
		VkPipelineMultisampleStateCreateInfo    multisamplingInfo  = {};
		VkPipelineColorBlendStateCreateInfo     colorBlendingInfo  = {};
		VkPipelineLayoutCreateInfo              pipelineLayoutInfo = {};

		operator VkPipeline() const {
			return pipeline;
		}

		Material();
		~Material();
		void Setup(
			VkDevice device,
			vk::RenderPass& renderPass,
			std::vector<const char*>& shaderFileNames,
			std::vector<VkShaderStageFlagBits>& shaderUsage,
			SubpassIndex subpassIndex
		);
		void Destroy();

		static void CreateMaterials(vk::Material* materials, uint32_t size = 1);

	private:
		std::vector<char> LoadFile(const char* filename);
		void ConnectObjects();
	};
}