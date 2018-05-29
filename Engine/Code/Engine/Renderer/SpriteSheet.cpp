#include "SpriteSheet.hpp"
#include "Engine/Math/Renderer.hpp"


SpriteSheet::SpriteSheet(const Texture& texture, int tilesWide, int tilesHigh):
m_spriteSheetTexture (&texture){
	m_spriteLayout = IntVector2(tilesWide, tilesHigh);
}

AABB2 SpriteSheet::GetTexCoordsForSpriteCoords(const IntVector2 & spriteCoords) const
{
	IntVector2 flipCoords = IntVector2(spriteCoords.x, m_spriteLayout.y - 1 - spriteCoords.y);
	Vector2 spriteCoordsFloat = flipCoords.GetVector2();
	Vector2 spriteLayoutFloat = m_spriteLayout.GetVector2();
	Vector2 verySmallAmountIn = Vector2(.001f,.001f);
	Vector2 mins = Vector2(spriteCoordsFloat.x/spriteLayoutFloat.x, spriteCoordsFloat.y/ spriteLayoutFloat.y) + verySmallAmountIn;
	Vector2 maxs = Vector2((spriteCoordsFloat.x + 1.f)/spriteLayoutFloat.x, (spriteCoordsFloat.y+1.f)/ spriteLayoutFloat.y) - verySmallAmountIn;

	return AABB2(mins, maxs);
}

AABB2 SpriteSheet::GetTexCoordsForSpriteIndex(int spriteIndex) const
{
	IntVector2 spriteCoords = GetCoordinatesFromIndex(spriteIndex, m_spriteLayout.x);

	return GetTexCoordsForSpriteCoords(spriteCoords);
}

//void SpriteSheet::DrawSpriteAtTexCoords(const AABB2 bounds, AABB2 texCoords, RGBA tint)
//{
//	DrawTexturedAABB2(bounds, m_spriteSheetTexture, texCoords.mins, texCoords.maxs, tint);
//}

int SpriteSheet::GetNumSprites() const
{
	return m_spriteLayout.x * m_spriteLayout.y;
}

const Texture* SpriteSheet::GetTexture() const
{
	return m_spriteSheetTexture;
}

IntVector2 SpriteSheet::GetDimensions() const
{
	return m_spriteLayout;
}

