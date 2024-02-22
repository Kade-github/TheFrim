#include "2DSprite.h"

Sprite2D::Sprite2D(std::string texture, glm::vec3 _pos) : GameObject2D(_pos)
{
	t = Texture::createWithImage(texture);
}

Sprite2D::~Sprite2D()
{
	delete t;
}

void Sprite2D::Resize(float _w, float _h)
{
	t->resizeTexture(_w, _h);
}

void Sprite2D::Draw()
{
	if (t == NULL)
		return;

	std::vector<Vertex2D> vertices;

	Vertex2D tl = { position.x, position.y, 0, 0, 0 };
	Vertex2D tr = { position.x + (float)t->width, position.y, 0, 1, 0 };
	Vertex2D bl = { position.x, position.y + (float)t->height, 0, 0, 1 };
	Vertex2D br = { position.x + (float)t->width, position.y + (float)t->height, 0, 1, 1 };

	vertices.push_back(tl);
	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(bl);
	vertices.push_back(tr);
	vertices.push_back(br);

	draws = { { t, s, vertices } };
}
