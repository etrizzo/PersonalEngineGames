#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/RGBA.hpp"

class Renderer;
class Sprite;

class SpriteSheet
{
public:

	SpriteSheet( Texture& texture, int tilesWide, int tilesHigh);
	AABB2 GetTexCoordsForSpriteCoords(const IntVector2& spriteCoords) const;
	AABB2 GetTexCoordsForSpriteIndex(int spriteIndex) const;
	//void DrawSpriteAtTexCoords(const AABB2 bounds, AABB2 texCoords, RGBA tint = RGBA());
	int GetNumSprites() const;
	Texture* GetTexture() const;

	Sprite* GetSprite(int x, int y, Renderer* r);		//must pass in renderer you have loaded sprites into
	Sprite* GetSprite(int i, Renderer* r);

	IntVector2 GetDimensions() const;
	std::string GetPath() const { return m_spriteSheetTexture->GetPath(); };
	void SetTexture(Texture* tex);

private:
	Texture* m_spriteSheetTexture;
	IntVector2 m_spriteLayout;


};
