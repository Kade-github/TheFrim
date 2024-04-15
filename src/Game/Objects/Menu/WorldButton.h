#ifndef _WORLDBUTTON_H
#define _WORLDBUTTON_H

#include <Objects/2DSprite.h>
#include <Objects/2DText.h>

#include <functional>

class WorldButton : public Sprite2D
{
	Text2D* _text;
	Texture* _selectGreen;
	Texture* _selectRed;
	Texture* _normal;
	int _selectType = 0;

public:
	std::function<void()> onClick;

	WorldButton(glm::vec3 _pos, std::string _text);

	~WorldButton()
	{
		delete _text;
	}

	void SetText(std::string _text);

	void SetSelect(int type = 0);

	void Draw() override;


	void MouseClick(int button, glm::vec2 mPos) override;
};

#endif