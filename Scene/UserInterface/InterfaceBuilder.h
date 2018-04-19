#pragma once

#include "Scene/UserInterface/InterfaceRenderer.h"

class InterfaceBuilder {
private:
	InterfaceRenderer& renderer;

public:
	InterfaceBuilder(InterfaceRenderer& renderer)
		: renderer(renderer) {}
	~InterfaceBuilder();

	void Initialize();
};
