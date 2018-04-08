#pragma once

#include "Scene/Chunks/ChunksController.h"
#include "Scene/PostProcessing/BlurController.h"
#include "Scene/UserInterface/InterfaceController.h"

class SceneRenderer :
	public ChunksController,
	public BlurController,
	public InterfaceController
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