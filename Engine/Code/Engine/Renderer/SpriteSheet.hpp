#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/RGBA.hpp"


class SpriteSheet
{
public:

	SpriteSheet(const Texture& texture, int tilesWide, int tilesHigh);
	AABB2 GetTexCoordsForSpriteCoords(const IntVector2& spriteCoords) const;
	AABB2 GetTexCoordsForSpriteIndex(int spriteIndex) const;
	//void DrawSpriteAtTexCoords(const AABB2 bounds, AABB2 texCoords, RGBA tint = RGBA());
	int GetNumSprites() const;
	const Texture* GetTexture() const;
	IntVector2 GetDimensions() const;
	std::string GetPath() const { return m_spriteSheetTexture->GetPath(); };

private:
	const Texture* m_spriteSheetTexture;
	IntVector2 m_spriteLayout;

};
