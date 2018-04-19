#pragma once

#include "Scene/Chunks/ChunksRenderer.h"
#include "Scene/PostProcessing/BlurRenderer.h"
#include "Scene/UserInterface/InterfaceRenderer.h"

class SceneRenderer :
	private ChunksRenderer,
	private BlurRenderer,
	private InterfaceRenderer
{
public:
    SceneRenderer();
    ~SceneRenderer();

	template <typename T>
	T& GetComponent();

private:
	void DrawScene();
	void CreateCommandBuffers();

	std::vector<uint32_t> times;
};