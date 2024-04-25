#ifndef _OBJWORLD_H
#define _OBJWORLD_H

#include <Objects/2DSprite.h>
#include <Objects/2DText.h>

#include "WorldButton.h"

class World : public Sprite2D
{
	Text2D* label_name;
	Text2D* label_seed;

	WorldButton* selectWorld;
	WorldButton* deleteWorld;

	Sprite2D* worldThumbnail;

public:
	World(glm::vec3 _pos);

	~World();

	void SetName(std::string _name);
	void SetSeed(std::string _seed);

	void SetDeleteText(std::string _text)
	{
		deleteWorld->SetText(_text);
	}

	void SetSelectText(std::string _text)
	{
		selectWorld->SetText(_text);
	}

	void SetSelectCallback(std::function<void()> _callback)
	{
		selectWorld->onClick = _callback;
	}

	void SetDeleteCallback(std::function<void()> _callback)
	{
		deleteWorld->onClick = _callback;
	}

	void SetThumbnail(std::string _path)
	{
		worldThumbnail->t = Texture::createWithImage(_path);
	}

	void Draw() override;

	void MouseClick(int button, glm::vec2 mPos) override
	{
		selectWorld->MouseClick(button, mPos);
		deleteWorld->MouseClick(button, mPos);
	}
};

#endif