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
	float renderWidth = 0;
	float renderHeight = 0;

	BoxUI(glm::vec3 _pos, int _w, int _h);
	~BoxUI();

	void SetBox();

	Sprite2D* GetFront(glm::vec2 _closestPos);
	Sprite2D* GetSlot(glm::vec2 _closestPos);

	std::vector<Sprite2D*> GetFront() { return front; }
	std::vector<Sprite2D*> GetSlots() { return slots; }
	std::vector<Sprite2D*> GetBack() { return back; }

	void AddFront(Sprite2D* s, int x, int y);
	void RemoveFront(Sprite2D* s);

	void ClearFront();

	void Draw() override;
};

#endif // _BOX_UI_H