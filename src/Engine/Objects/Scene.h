#ifndef _SCENE_H
#define _SCENE_H

#pragma once

#include "GameObject.h"

#include <vector>

class Scene
{
public:
	int lastId = 0;

	Scene()
	{
	}

	~Scene()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			delete objects[i];
		}
	}

	std::vector<GameObject*> objects = {};

	virtual void MouseMove(float x, float y) {};
	virtual void KeyPress(int key) {};
	virtual void KeyRelease(int key) {};

	void AddObject(GameObject* object)
	{
		object->id = lastId;
		objects.push_back(object);
		object->Create();
		lastId++;
	}

	void RemoveObject(GameObject* object)
	{
		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i]->id == object->id)
			{
				objects.erase(objects.begin() + i);
				break;
			}
		}
	}

	virtual void Create() {};

	virtual void Draw()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->Draw();
		}
	}
};

#endif