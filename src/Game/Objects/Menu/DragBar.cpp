#include "DragBar.h"
#include <Game.h>
#include <Helpers/StringTools.h>

DragBar::DragBar(glm::vec3 _position, std::string _text, float _value) : Sprite2D("Assets/Textures/MainMenu/inputBar.png", _position)
{
	_initialText = _text;
	t = new Text2D(_text, "ArialFrim", glm::vec3(0,0,0), glm::vec4(1, 1, 1, 1), 62);

	t->position = glm::vec3(_position.x + 10, _position.y + 10, 0);

	drag = new Sprite2D("Assets/Textures/MainMenu/dragBar.png", glm::vec3(_position.x + 10, _position.y + 10, 0));

	value = _value;
}

void DragBar::Draw()
{
	if (initialPos.x >= 0 && initialPos.y >= 0)
	{
		glm::vec2 mPos = Game::instance->GetCursorPos();

		value = _initialValue + ((mPos.x - initialPos.x) / width);

		if (value < 0.35f)
			value = 0.35f;
		if (value > 1)
			value = 1;
	}

	t->text = _initialText + ": " + StringTools::ToTheDecimial(value * max, 1);

	drag->width = width * (0.019);
	drag->height = height;

	drag->position = glm::vec3(position.x + (width * value), position.y, 0);

	if (drag->position.x + drag->width > position.x + width)
		drag->position.x = position.x + width - drag->width;

	t->position = glm::vec3(position.x + (width / 2) - (t->width / 2), position.y + (height / 2) - (t->height / 2), 0);

	draws.clear();

	Sprite2D::Draw();

	t->Draw();
	drag->Draw();

	draws.push_back(t->draws[0]);
	draws.push_back(drag->draws[0]);
}

void DragBar::MouseClick(int button, glm::vec2 mPos)
{
	if (mPos.x > position.x && mPos.x < position.x + width && mPos.y > position.y && mPos.y < position.y + height)
	{
		_initialValue = (mPos.x - position.x) / width;
		initialPos = mPos;
	}
}

void DragBar::MouseRelease(int button, glm::vec2 mPos)
{
	initialPos = glm::vec2(-1, -1);
}

void DragBar::SetText(std::string _text)
{
	_initialText = _text;
	t->text = _text;
}
