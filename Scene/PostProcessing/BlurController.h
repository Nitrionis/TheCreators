#pragma once

#include "BlurRenderer.h"
#include "BlurBuilder.h"

class BlurController :
	public BlurRenderer,
	public BlurBuilder
{
public:
	BlurController();
	~BlurController();

	void Initialize();
};


