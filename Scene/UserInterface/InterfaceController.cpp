//
// Created by nitro on 31.03.2018.
//

#include "Scene/UserInterface/InterfaceController.h"

InterfaceController::InterfaceController() {

}

InterfaceController::~InterfaceController() {

}

void InterfaceController::Initialize() {
	InterfaceRenderer::Initialize();
	InterfaceBuilder::Initialize();
}
