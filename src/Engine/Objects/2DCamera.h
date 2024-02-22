#ifndef _2DCAMERA_H
#define _2DCAMERA_H

#include "2DGameObject.h"

class Camera2D : public GameObject
{
	int _lastId = 0;
	float _w, _h;
public:
	unsigned int s_vao, s_vbo;

	unsigned int vao, vbo;
	unsigned int fb;

	Texture* t = NULL;
	Shader* s = NULL;

	std::vector<GameObject2D*> objects = {};
	std::vector<Vertex2D> vertices = {};

	Camera2D(glm::vec3 pos);

	~Camera2D();

	void Resize();

	void AddObject(GameObject2D* object)
	{
		object->id = _lastId;
		objects.push_back(object);

		_lastId++;
	}

	void RemoveObject(GameObject2D* object)
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

	void Draw() override;
};

#endif