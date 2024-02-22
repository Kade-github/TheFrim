#ifndef _2DCAMERA_H
#define _2DCAMERA_H

#include "2DGameObject.h"

namespace Cam
{
	struct Vertex {
	public:
		glm::vec3 position;
		glm::vec2 uv;

		Vertex(glm::vec3 _position, glm::vec2 _uv)
		{
			position = _position;
			uv = _uv;
		}
	};
}


class Camera2D : public GameObject
{
	float _w, _h;
public:
	unsigned int s_vao, s_vbo;

	unsigned int vao, vbo;
	unsigned int fb;

	Texture* t = NULL;
	Shader* s = NULL;

	std::vector<GameObject2D*> objects = {};
	std::vector<Cam::Vertex> vertices = {};


	Camera2D(glm::vec3 pos);

	~Camera2D();

	void Resize();

	void AddObject(GameObject2D* object)
	{
		objects.push_back(object);
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