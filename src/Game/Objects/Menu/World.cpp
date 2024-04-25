#include "World.h"

World::World(glm::vec3 _pos) : Sprite2D("Assets/Textures/WorldSelect/world_select.png", _pos)
{
	label_name = new Text2D("", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 64);
	label_seed = new Text2D("", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 42);

	selectWorld = new WorldButton(glm::vec3(0, 0, 0), "Select");
	deleteWorld = new WorldButton(glm::vec3(0, 0, 0), "Delete");

	deleteWorld->SetSelect(1);

	selectWorld->width *= 1.25;
	selectWorld->height *= 1.25;

	deleteWorld->width *= 1.25;
	deleteWorld->height *= 1.25;

	worldThumbnail = new Sprite2D("Assets/Textures/WorldSelect/new_world.png", glm::vec3(0, 0, 0));

	worldThumbnail->width *= 0.56;
	worldThumbnail->height *= 0.56;

}

void World::SetName(std::string _name)
{
	label_name->text = _name;
}

void World::SetSeed(std::string _seed)
{
	label_seed->text = "Seed: " + _seed;
}

void World::Draw()
{
	worldThumbnail->clip = clip;

	worldThumbnail->position = position + glm::vec3(2, 2, 0);
	worldThumbnail->Draw();

	Sprite2D::Draw();

	draws = { worldThumbnail->draws[0], draws[0] };

	label_name->clip = clip;

	label_name->position = position + glm::vec3(162,64,0);
	label_name->Draw();

	label_seed->clip = clip;

	label_seed->position = position + glm::vec3(162, 16, 0);
	label_seed->Draw();

	draws.push_back(label_name->draws[0]);
	draws.push_back(label_seed->draws[0]);

	selectWorld->clip = clip;

	selectWorld->position = position + glm::vec3(width - (((selectWorld->width * 2) + 42) + 52), (height / 2) - (selectWorld->height / 2), 0);
	selectWorld->Draw();

	draws.push_back(selectWorld->draws[0]);
	draws.push_back(selectWorld->draws[1]);

	deleteWorld->clip = clip;

	deleteWorld->position = position + glm::vec3(width - ((deleteWorld->width + 16) + 52), (height / 2) - (deleteWorld->height / 2), 0);
	deleteWorld->Draw();

	draws.push_back(deleteWorld->draws[0]);
	draws.push_back(deleteWorld->draws[1]);
}

World::~World()
{
    delete label_name;
    delete label_seed;

    delete selectWorld;
    delete deleteWorld;

    delete worldThumbnail;
}
