#ifndef _SCENE_H
#define _SCENE_H

#pragma once

#include "GameObject.h"
#include <stdlib.h>
#include <vector>

class Scene
{
public:
	int lastId = 0;
	bool isCreated = false;

	Scene()
	{
	}

	~Scene()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			GameObject* object = objects[i];

			object->Destroy();

			delete object;
		}
	}

	std::vector<GameObject*> objects = {};
	std::vector<GameObject*> delayedObjects = {};

	virtual void MouseMove(float x, float y) {};
	virtual void MouseClick(int button, glm::vec2 mPos) {};
	virtual void MouseRelease(int button, glm::vec2 mPos) {};
	virtual void KeyPress(int key) {};
	virtual void KeyRelease(int key) {};
	virtual void OnChar(unsigned int c) {};
	virtual void OnScroll(double x, double y) {};
	virtual void FocusChange(bool focus) {};

	void DelayedAddObject(GameObject* object)
	{
		delayedObjects.push_back(object);
	}

	void AddObject(GameObject* object)
	{
		object->id = lastId;
		objects.push_back(object);

		std::qsort(&objects[0], objects.size(), sizeof(GameObject*), [](const void* a, const void* b) -> int
			{
			GameObject* _a = *(GameObject**)a;
			GameObject* _b = *(GameObject**)b;

			return _a->order - _b->order;
		});

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

	virtual void Resize(float _w, float _h) {};

	virtual void Destroy() {};

	virtual void Create() {};

	virtual void Draw()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			objects[i]->Draw();
		}

		for (int i = 0; i < delayedObjects.size(); i++)
		{
			AddObject(delayedObjects[i]);
		}

		delayedObjects.clear();
	}
};

#endif