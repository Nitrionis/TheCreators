#pragma once
#include "VulkanSharedHaders.h"

namespace vk
{
	namespace initialize
	{
		VkViewport viewportDefault(VkExtent2D extent);
		VkRect2D scissorDefault(VkExtent2D extent);
	};

	class RenderPass {
		friend class Material;
	private:
		VkDevice* device = nullptr;
		VkRenderPass renderPass = VK_NULL_HANDLE;
	public:
		std::vector<VkAttachmentDescription>  colorAttachments;
		std::vector<VkAttachmentReference>    colorAttachmentRefs;
		std::vector<VkSubpassDescription>     subPasses;
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
		void DoFinalInitialise();
	};

	class Material {
	private:
		VkDevice         device         = VK_NULL_HANDLE;
		VkPipeline       pipeline       = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

		std::vector<VkShaderModule> shaders;

		std::string directoryPath = "C:\\Developer\\JetBrains\\Clion\\Proj\\TheCreators\\";

		std::vector<VkPipelineShaderStageCreateInfo> stagesInfo;
		VkGraphicsPipelineCreateInfo pipelineInfo;

	public:
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
			VkDevice                            device,
			vk::RenderPass&                     renderPass,
			std::vector<const char*>&           shaderFileNames,
			std::vector<VkShaderStageFlagBits>& shaderUsage,
			size_t                              subpassIndex
		);
		void Destroy();

		static void CreateMaterials(Material* materials, uint32_t size);

	private:
		std::vector<char> LoadFile(const char* filename);
		void ConnectObjects();
	};
}