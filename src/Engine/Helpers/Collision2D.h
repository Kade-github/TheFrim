#ifndef _COLLISION2D_H_
#define _COLLISION2D_H_

#include <glm/glm.hpp>

class Collision2D
{
public:
	static bool PointInRect(const glm::vec2& point, const glm::vec2& rectPos, const glm::vec2& rectSize);
};

#endif