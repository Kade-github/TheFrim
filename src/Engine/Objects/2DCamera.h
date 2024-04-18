#ifndef _2DCAMERA_H
#define _2DCAMERA_H

#include "2DGameObject.h"
#include "2DText.h"

class Camera2D : public GameObject
{
	int _lastId = 0;
	bool setProject = false;
public:
	float _w, _h;
	float _rW, _rH;
	unsigned int s_vao, s_vbo;

	unsigned int vao, vbo;
	unsigned int fb;

	Texture* t = NULL;
	Texture* def = NULL;
	Shader* s = NULL;

	Text2D* debugText = NULL;
	std::vector<Draw2D> dbgDraws = {};

	std::vector<GameObject2D*> objects = {};
	std::vector<Vertex2D> vertices = {};

	Camera2D(glm::vec3 pos);
	
	void Resize();
	void ResizeTo();
	void UpdateFramebuffer();

	void MouseClick(int button, glm::vec2 pos) override;
	void MouseRelease(int button, glm::vec2 pos) override;
	void MouseMove(float x, float y) override;
	void KeyPress(int key) override;


	void DrawDebugText(std::string text, glm::vec2 pos, int size);

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
	void DrawSprite();

	void Destroy() override;
};

#endif