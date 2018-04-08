#include "Scene/Chunks/ChunksController.h"

ChunksController::ChunksController() {

}

ChunksController::~ChunksController() {

}

void ChunksController::Initialize() {
	ChunksRenderer::Initialize();
	ChunksBuilder::Initialize();
}
