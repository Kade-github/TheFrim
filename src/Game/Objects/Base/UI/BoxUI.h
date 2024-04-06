#ifndef _BOX_UI_H
#define _BOX_UI_H

#include <Objects/2DGameObject.h>
#include <Objects/2DSprite.h>

class BoxUI : public GameObject2D
{
	std::vector<Sprite2D*> back;
	std::vector<Sprite2D*> slots;
	std::vector<Sprite2D*> front;
public:
	Texture* t;
	int width, height;

	BoxUI(glm::vec3 _pos, int _w, int _h);
	~BoxUI();

	void SetBox();

	void AddFront(Sprite2D* s, int x, int y);
	void RemoveFront(Sprite2D* s);

	void ClearFront();

	void Draw() override;
};

#endif // _BOX_UI_H