#include "WorldButton.h"
#include "Game.h"

WorldButton::WorldButton(glm::vec3 _pos, std::string _text) : Sprite2D(_pos)
{
	position = _pos;
	this->_text = new Text2D(_text, "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 42);
	_normal = Texture::createWithImage("Assets/Textures/WorldSelect/button.png");
	_selectGreen = Texture::createWithImage("Assets/Textures/WorldSelect/select_green.png");
	_selectRed = Texture::createWithImage("Assets/Textures/WorldSelect/select_red.png");

	width = _normal->width;
	height = _normal->height;
}

void WorldButton::SetText(std::string _text)
{
	this->_text->text = _text;
}

void WorldButton::SetSelect(int type)
{	
	_selectType = type;
}

void WorldButton::Draw()
{
	glm::vec2 mouse = Game::instance->GetCursorPos();

	if (mouse.x > position.x && mouse.x < position.x + width)
	{
		if (mouse.y > position.y && mouse.y < position.y + height)
		{
			if (_selectType == 0)
				t = _selectGreen;
			else
				t = _selectRed;
		}
		else
			t = _normal;
	}
	else
		t = _normal;

	Sprite2D::Draw();

	_text->position = position + glm::vec3((width / 2) - (_text->width / 2), (height / 2) - (_text->height / 2), 0);
	_text->position -= glm::vec3(6, 6, 0);
	_text->Draw();

	draws = { draws[0], _text->draws[0] };

}

void WorldButton::MouseClick(int button, glm::vec2 mPos)
{
	if (button == 0)
	{
		if (mPos.x > position.x && mPos.x < position.x + width)
		{
			if (mPos.y > position.y && mPos.y < position.y + height)
			{
				if (onClick != NULL)
					onClick();
			}
		}
	}
}
