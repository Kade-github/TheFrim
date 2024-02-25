#include "2DText.h"
#include <Game.h>

Text2D::Text2D(std::string text, std::string font, glm::vec3 pos, glm::vec4 color, int size) : GameObject2D(pos)
{
	this->text = text;
	this->font = Fnt::GetFont(font);
	this->color = color;
	this->size = size;
	Draw();
}

void Text2D::Draw()
{
	if (font == NULL)
		return;

	Camera* cam = Game::instance->GetCamera();

	glm::vec3 p = position;

	Draw2D call;
	call.textureId = font->GetTexture();
	call.shaderId = s;
	call.clipRect = clip;

	std::vector<Line> lines;

	float scale = (float)size / (float)font->ogSize;

	float totalHeight = 0;

	Line currentLine;
	currentLine.w = 0;

	for (int i = 0; i < text.size(); i++)
	{
		char ch = text[i];
		const FntChar& c = font->GetChar(ch);
		if (c.id == -1)
			continue;
		float fAdvance = (float)c.xAdvance;
		if (i + 1 < text.size() && font->hasKernings)
		{
			char next = text[i + 1];
			for (int j = 0; j < c.kernings.size(); j++)
			{
				if (c.kernings[j].with == next)
				{
					fAdvance -= c.kernings[j].amount;
					break;
				}
			}
		}
		float characterWidth = c.dst.z * scale;
		float characterHeight = c.dst.w * scale;


		float advance = (fAdvance * scale);
		if (ch == 32) // space
		{
			RenderedCharacter c;
			c.space = true;
			c.advance = advance;
			p.x += advance;

			currentLine.w += advance;

			if (wrap && p.x + characterWidth > cam->width)
			{
				lines.push_back(currentLine);
				currentLine = Line();
				currentLine.w = 0;
				p.x = position.x;
				totalHeight += size;
			}

			currentLine.characters.push_back(c);
			continue;
		}

		RenderedCharacter rc;

		rc.advance = advance;
		rc.w = characterWidth;
		rc.h = characterHeight;
		rc.src = c.src;

		p.x += advance;
		currentLine.w += advance;

		currentLine.characters.push_back(rc);
	}

	if (currentLine.characters.size() > 0)
		lines.push_back(currentLine);

	float y = position.y;

	if (center)
		y = position.y - (totalHeight / 2.0f);

	std::vector<Vertex2D> vertices;

	float biggestW = 0;

	for (int i = 0; i < lines.size(); i++)
	{
		Line l = lines[i];
		float x = position.x;

		if (center)
			x = position.x - (l.w / 2.0f);

		if (l.w > biggestW)
			biggestW = l.w;


		for (int j = 0; j < lines[i].characters.size(); j++)
		{
			RenderedCharacter& rc = lines[i].characters[j];

			if (rc.space)
			{
				x += rc.advance;
				continue;
			}

			vertices.clear();

			// these are flipped vertically

			glm::vec3 tl = glm::vec3(x, y + rc.h, 0);
			glm::vec3 tr = glm::vec3(x + rc.w, y + rc.h, 0);
			glm::vec3 bl = glm::vec3(x, y, 0);
			glm::vec3 br = glm::vec3(x + rc.w, y, 0);

			if (alienMode)
			{
				// unflip them

				tl.y = y;
				tr.y = y;
				bl.y = y + rc.h;
				br.y = y + rc.h;
			}

			vertices.push_back(Vertex2D(tl, rc.src, color));
			vertices.push_back(Vertex2D(bl, glm::vec2{ rc.src.x, rc.src.y + rc.src.w }, color));
			vertices.push_back(Vertex2D(tr, glm::vec2{ rc.src.x + rc.src.z, rc.src.y }, color));
			vertices.push_back(Vertex2D(tr, glm::vec2{ rc.src.x + rc.src.z, rc.src.y }, color));
			vertices.push_back(Vertex2D(bl, glm::vec2{ rc.src.x, rc.src.y + rc.src.w }, color));
			vertices.push_back(Vertex2D(br, glm::vec2{ rc.src.x + rc.src.z, rc.src.y + rc.src.w }, color));

			call.vertices.insert(call.vertices.end(), vertices.begin(), vertices.end());
			
			x += rc.advance;
		}
		y += size;
	}

	width = biggestW;

	draws = { call };
}