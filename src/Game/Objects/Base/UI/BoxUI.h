#ifndef _BOX_UI_H
#define _BOX_UI_H

#include <Objects/2DGameObject.h>
#include <Objects/2DSprite.h>

struct BoxSlot
{
	int id = 0;
	float x = 0, y = 0;
	Sprite2D* slot = nullptr;
	Sprite2D* front = nullptr;
};

class BoxUI : public GameObject2D
{
	std::vector<Sprite2D*> back;
public:
	std::vector<BoxSlot> boxSlots;

	Texture* t;
	int width, height;
	float renderWidth = 0;
	float renderHeight = 0;

	BoxUI(glm::vec3 _pos, int _w, int _h);
	void Destroy() override;

	void SetBox();

	void ClearFronts();
	void RemoveFront(int id);

	void ClearSlots();

	void AddSlot(float x, float y, int id);
	void AddFront(Sprite2D* s, int id);

	BoxSlot& GetSlot(glm::vec2 pos);
	BoxSlot& GetSlot(int id);

	Sprite2D* GetFront(glm::vec2 pos);

	void Draw() override;
};

#endif // _BOX_UI_H