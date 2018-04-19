#pragma once
#include "Scene/SceneRenderer/SceneRenderer.h"

#include "Scene/Chunks/ChunksBuilder.h"
#include "Scene/PostProcessing/BlurBuilder.h"
#include "Scene/UserInterface/InterfaceBuilder.h"

class Scene :
	private SceneRenderer/*,
	private ChunksBuilder,
	private BlurBuilder,
	private InterfaceBuilder*/
{
public:
    Scene();
    ~Scene();

	template <typename T>
	T& GetComponent();

private:
};