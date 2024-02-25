#include "World.h"

World::World(glm::vec3 _pos) : Sprite2D("Assets/Textures/WorldSelect/world_select.png", _pos)
{
	label_name = new Text2D("", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 42);
	label_seed = new Text2D("", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 42);
}

void World::SetName(std::string _name)
{
	label_name->text = _name;
}

void World::SetSeed(std::string _seed)
{
	label_seed->text = _seed;
}

void World::Draw()
{
	Sprite2D::Draw();

	label_name->position = position;
	label_name->Draw();

	label_seed->position = position;
	label_seed->Draw();
}
