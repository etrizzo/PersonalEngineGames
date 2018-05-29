#include "Sprite.hpp"
#include "Engine/Core/XmlUtilities.hpp"

Sprite::Sprite(const Texture * tex, AABB2 uvs, Vector2 pivot, Vector2 dimensions, Vector2 scale)
{
	m_tex = tex;
	m_uvs = uvs;
	m_pivot = pivot;
	m_dimensions = dimensions;
	m_scale = scale;
}

Sprite * Sprite::GetScaledSprite(Vector2 scale)
{
	return new Sprite(m_tex, m_uvs, m_pivot, m_dimensions, scale);
}

Plane * Sprite::GetBounds(const Vector3 & pos, const Vector3 & right, const Vector3 & up)
{
	float nx = -1.f * (m_pivot.x * m_dimensions.x) * m_scale.x;
	float fx = nx + (m_dimensions.x* m_scale.x);
	float ny = -1.f * (m_pivot.y * m_dimensions.y)* m_scale.y;
	float fy = ny + (m_dimensions.y * m_scale.y);

	Vector3 botLeft = pos + (nx * right) + (ny * up);
	Vector3 botRight = pos + (fx * right) + (ny * up);
	Vector3 topLeft = pos + (nx * right) + (fy * up);
	Vector3 topRight = pos + (fx * right) + (fy * up);

	return new Plane(botLeft, botRight, topRight, topLeft);
}

