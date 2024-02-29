#include "Camera.h"
#include <Game.h>
void Camera::SetDirection()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	Game::instance->shader->Bind();
	Game::instance->shader->SetUniformMat4f("view", &GetViewMatrix()[0][0]);
	Game::instance->shader->Unbind();
}