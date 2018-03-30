#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VectorTypes.h"

class Character {
private:
	Character(){}

public:
	~Character(){}

	Character(Character const&) = delete;
	Character& operator= (Character const&) = delete;

	static Character& Instance() {
		static Character s;
		return s;
	}

	Vector3<float> position;
	Vector3<float> rotation;

	struct {
		glm::mat4 mvp;
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	}matrix;

	void UpdateMatrixMvp();
};


