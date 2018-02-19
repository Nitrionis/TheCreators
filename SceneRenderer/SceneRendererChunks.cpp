#include "SceneRenderer.h"

void SceneRenderer::Chunks::Initialize() {
	CreateMaterials();
}

void SceneRenderer::Chunks::CreateMaterials() {

	material.viewports[0] = vk::initialize::viewportDefault(vulkan.swapChain.extent);
	material.scissors[0] = vk::initialize::scissorDefault(vulkan.swapChain.extent);
	material.Setup(
		vulkan.device,
		vulkan.renderPass,
		settings.chunkShaderNames,
		settings.chunkShaderUsage,
		0
	);
	vk::Material::CreateMaterials(&material, 1);
}
