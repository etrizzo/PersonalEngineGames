#include "BitmapFont.hpp"

AABB2 BitmapFont::GetUVsForGlyph(int glyphUnicode) const
{

	
	IntVector2 dimensions = m_spriteSheet.GetDimensions();
	IntVector2 coords = GetCoordinatesFromIndex( glyphUnicode, dimensions.x );
	//coords.y = dimensions.y - 1 - coords.y;
	return m_spriteSheet.GetTexCoordsForSpriteCoords(coords);
}

float BitmapFont::GetStringWidth(const std::string & asciiText, float cellHeight, float aspectScale)
{
	int stringLength = (int) asciiText.size();
	return stringLength * cellHeight * aspectScale;
}

const Texture * BitmapFont::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}


BitmapFont::BitmapFont(const std::string & fontName, SpriteSheet & glyphSheet, float baseAspect):
	m_spriteSheet (glyphSheet)
{
	m_fontName = fontName;
	m_spriteSheet = glyphSheet;
	m_baseAspect = baseAspect;
}
