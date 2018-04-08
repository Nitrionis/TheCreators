#pragma once

#include "InterfaceRenderer.h"
#include "InterfaceBuilder.h"

class InterfaceController :
	public InterfaceRenderer,
	public InterfaceBuilder
{
public:
	InterfaceController();
	~InterfaceController();

	void Initialize();
};