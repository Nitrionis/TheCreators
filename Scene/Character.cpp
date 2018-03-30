#include "Character.h"

void Character::UpdateMatrixMvp() {

	matrix.model = glm::rotate(glm::mat4(), 1 * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	matrix.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	matrix.proj = glm::perspective(glm::radians(45.0f), 1920 / (float)1080, 0.1f, 10.0f);
	matrix.proj[1][1] *= -1;
	matrix.mvp = matrix.model * matrix.view * matrix.proj;
}
