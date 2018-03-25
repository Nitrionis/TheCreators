#pragma once

#include "ChunkRenderer.h"
#include "BlurRenderer.h"
#include "UserInterfaceRenderer.h"

class SceneRenderer :
	public ChunkRenderer,
	public BlurRenderer,
	public UserInterfaceRenderer
{
public:
    SceneRenderer();
    ~SceneRenderer();

private:
	void DrawScene();
	void CreateCommandBuffers();

public:

};