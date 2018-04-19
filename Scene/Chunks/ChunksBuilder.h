#pragma once

#include <Vulkan/Mesh/Mesh.h>

#include "Scene/Chunks/ChunksRenderer.h"

class ChunksBuilder {
private:
	ChunksRenderer& renderer;

public:
	ChunksBuilder(ChunksRenderer& renderer)
		: renderer(renderer) {}
	~ChunksBuilder();

	void Initialize();

private:

};