#include "Collision2D.h"

bool Collision2D::PointInRect(const glm::vec2& point, const glm::vec2& rectPos, const glm::vec2& rectSize)
{
	return point.x >= rectPos.x && point.x <= rectPos.x + rectSize.x && point.y >= rectPos.y && point.y <= rectPos.y + rectSize.y;
}
