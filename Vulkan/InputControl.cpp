#include "InputControl.h"

using namespace InputControl;

InputSharedData& InputInterface::sharedData = InputSharedData::Instance();

InputData::InputData() {
	for (int i = 0; i < 255; i++) {
		keyCondsDown[i] = false;
		keyCondsUp[i] = true;
	}
}

bool InputInterface::GetKeyDown(KeyCode keyCode) {
	int keyIndex = static_cast<uint32_t>(keyCode);
	if (timeLineDown[keyIndex] != sharedData.timeLineDown[keyIndex]) {
		timeLineDown[keyIndex] = sharedData.timeLineDown[keyIndex];
		keyCondsDown[keyIndex] = sharedData.keyCondsDown[keyIndex];
	}

	if (keyCondsDown[keyIndex]) {
		keyCondsDown[keyIndex] = false;
		return true;
	}
	else
		return false;
}

bool InputInterface::GetKeyCode(KeyCode keyCode) {
	return sharedData.keyCondsCode[static_cast<uint32_t>(keyCode)];
}

bool InputInterface::GetKeyUp(KeyCode keyCode) {
	int keyIndex = static_cast<uint32_t>(keyCode);
	if (timeLineUp[keyIndex] != sharedData.timeLineUp[keyIndex]) {
		timeLineUp[keyIndex] = sharedData.timeLineUp[keyIndex];
		keyCondsUp[keyIndex] = sharedData.keyCondsUp[keyIndex];
	}

	if (keyCondsUp[keyIndex]) {
		keyCondsUp[keyIndex] = false;
		return true;
	}
	else
		return false;
}

POINT InputInterface::GetMouseCode(MouseCode mouseCode) {
	return sharedData.mouseCondsCode[static_cast<uint32_t>(mouseCode)];
}

POINT InputInterface::GetMouseDown(MouseCode mouseCode) {
	return sharedData.mouseCondsDown[static_cast<uint32_t>(mouseCode)];
}

POINT InputInterface::GetMouseUp(MouseCode mouseCode) {
	return sharedData.mouseCondsUp[static_cast<uint32_t>(mouseCode)];
}

POINT InputInterface::GetMouseMove() {
	return sharedData.mouseMove;
}

POINT InputInterface::GetMousePosition() {
	return sharedData.mousePosition;
}

InputSharedData::InputSharedData() {
	for (int i = 0; i < 255; i++) {
		keyCondsCode[i] = false;
	}
}
