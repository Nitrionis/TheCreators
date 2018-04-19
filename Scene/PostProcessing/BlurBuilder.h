#pragma once

#include "Scene/PostProcessing/BlurRenderer.h"


class BlurBuilder {
private:
	BlurRenderer& renderer;

public:
	BlurBuilder(BlurRenderer& renderer)
		: renderer(renderer) {}
	~BlurBuilder();

	void Initialize();
};


