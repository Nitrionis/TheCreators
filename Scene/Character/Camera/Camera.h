#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vulkan/Other/VectorTypes.h"

class Camera {
public:
	// World camera position
	glm::vec3 position;
	// Normalized direction vector
	glm::vec3 direction;
	// Rotate in degrees for each axis
	glm::vec3 rotation;

	struct {
		// Combined model view projection matrix
		glm::mat4 mvp;
		// Just model matrix. Yuo can only set this matrix
		glm::mat4 model;
		// View matrix
		glm::mat4 view;
		// Projection matrix
		glm::mat4 proj;
	}matrix;

	void RecalculateDirection();
	void RecalculateMvpMatrix();
	void RecalculateViewMatrix();
	void RecalculateProjectionMatrix();
	void RecalculateAll();
};


