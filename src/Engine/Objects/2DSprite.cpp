#include "2DSprite.h"

Sprite2D::Sprite2D(glm::vec3 _pos) : GameObject2D(_pos)
{
	position = _pos;
	t = NULL;
	width = 0;
	height = 0;

	draws = {};
}

Sprite2D::Sprite2D(std::string texture, glm::vec3 _pos) : GameObject2D(_pos)
{
	t = Texture::createWithImage(texture);

	width = t->width;
	height = t->height;

	draws = {};
}

Sprite2D::Sprite2D(Texture* t, glm::vec3 _pos) : GameObject2D(_pos)
{
	this->t = t;

	width = t->width;
	height = t->height;

	draws = {};
	_noDelete = true;
}


void Sprite2D::Resize(float _w, float _h)
{
	width = _w;
	height = _h;
}

void Sprite2D::Draw()
{
	if (t == NULL)
		return;

	std::vector<Vertex2D> vertices;

	Vertex2D tl = { position, glm::vec2{0,0}, color };
	Vertex2D tr = { position + glm::vec3{width, 0, 0}, glm::vec2{1,0}, color };
	Vertex2D bl = { position + glm::vec3{0, height, 0}, glm::vec2{0,1}, color };
	Vertex2D br = { position + glm::vec3{width, height, 0}, glm::vec2{1,1}, color };

	float rX = t->width * src.x;
	float rY = t->height * src.y;
	float rW = t->width * src.z;
	float rH = t->height * src.w;

	tl.u = rX / t->width;
	tl.v = rY / t->height;

	tr.u = (rX + rW) / t->width;
	tr.v = rY / t->height;

	bl.u = rX / t->width;
	bl.v = (rY + rH) / t->height;

	br.u = (rX + rW) / t->width;
	br.v = (rY + rH) / t->height;

	vertices.push_back(tl);
	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(bl);
	vertices.push_back(tr);
	vertices.push_back(br);

	draws = { { t, s, vertices, clip } };
}
