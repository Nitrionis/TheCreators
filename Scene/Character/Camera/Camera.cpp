#include "Camera.h"
#include <glm/gtx/rotate_vector.hpp>

void Camera::RecalculateMvpMatrix() {
	matrix.mvp = matrix.proj * matrix.view;
}

void Camera::RecalculateDirection() {
	direction = glm::vec3(0.0f, 0.0f, 1.0f);
	direction = glm::rotateX(direction, rotation.x);
	direction = glm::rotateY(direction, rotation.y);
}

void Camera::RecalculateAll() {
	RecalculateDirection();
	RecalculateViewMatrix();
	RecalculateProjectionMatrix();
	RecalculateMvpMatrix();

	matrix.mvp = glm::mat4();
}

void Camera::RecalculateViewMatrix() {
	matrix.view = glm::lookAt(
		position,                    // position
		direction + position,        // target
		glm::vec3(0.0f, 1.0f, 0.0f));// up dir
}

void Camera::RecalculateProjectionMatrix() {
	matrix.proj = glm::perspective(
		glm::radians(70.0f), // FOV
		1920 / (float)1080,  // Aspect ratio
		0.1f,                // Near
		100.0f);             // Far
	matrix.proj[1][1] *= -1;
}
