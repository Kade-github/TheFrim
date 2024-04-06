#ifndef _ITEM_UI_H
#define _ITEM_UI_H

#include <Objects/2DSprite.h>
#include <Objects/2DText.h>

class ItemUI : public Sprite2D
{
	Text2D* countText;
public:
	int count;
	std::string tag;

	ItemUI(std::string tag, glm::vec3 _pos, Texture* _t, int _count);
	~ItemUI();

	void SetImage();
	void SetCount(int _count);

	void Draw() override;
};

#endif // _ITEM_UI_H