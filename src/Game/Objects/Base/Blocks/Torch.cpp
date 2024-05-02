#include "Torch.h"
#include "../../../LightingManager.h"

void Torch::Destroy()
{
	LightingManager::GetInstance()->RemoveLight(position);
}

void Torch::LoadModel()
{
	m = Model("Assets/Models/torchfrim.obj");
	m.LoadUVMap("torch_frim");
	m.scale = glm::vec3(0.35f, 0.35f, 0.35f);

	LightingManager::GetInstance()->AddLight(position, 10);
}

void Torch::Mo()
{
	Data::DataTag facing = data.GetTag("facing");

	m.position += glm::vec3(0.5f, 0.4f, 0.5f);

	if (facing.IsReal())
	{
		int face = std::stoi(facing.value);

		switch (face)
		{
			case 1:
				m.rotateAxis = glm::vec3(0, 0, 1);
				m.angle = -25;
				m.position += glm::vec3(-0.35f, 0.25f, 0);
				break;
			case 2:
				m.rotateAxis = glm::vec3(0, 0, 1);
				m.angle = 25;
				m.position += glm::vec3(0.35f, 0.25f, 0);
				break;
			case 3:
				m.rotateAxis = glm::vec3(1, 0, 0);
				m.angle = -25;
				m.position += glm::vec3(0, 0.25f, 0.35f);
				break;
			case 4:
				m.rotateAxis = glm::vec3(1, 0, 0);
				m.angle = 25;
				m.position += glm::vec3(0, 0.25f, -0.35f);
				break;
			default:
				m.rotateAxis = glm::vec3(0, 1, 0);
				m.angle = 0;
				break;
		}
	}
}
