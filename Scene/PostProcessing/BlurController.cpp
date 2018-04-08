//
// Created by nitro on 02.04.2018.
//

#include "BlurController.h"

BlurController::BlurController() {

}

BlurController::~BlurController() {

}

void BlurController::Initialize() {
	BlurRenderer::Initialize();
	BlurBuilder::Initialize();
}
