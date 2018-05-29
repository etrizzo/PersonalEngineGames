#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"


class BitmapFont{
	friend class Renderer;

public:
	AABB2 GetUVsForGlyph( int glyphUnicode ) const; // pass ‘A’ or 65 for A, etc.
	float GetGlyphAspect( int glyphUnicode ) const { glyphUnicode; return m_baseAspect; } // will change later
	float GetStringWidth( const std::string& asciiText, float cellHeight, float aspectScale );

	const Texture* GetTexture() const;
	std::string m_fontName;

private:
	explicit BitmapFont( const std::string& fontName, SpriteSheet& glyphSheet,
		float baseAspect ); // private, can only called by Renderer (friend class)
	SpriteSheet&	m_spriteSheet; // used internally; assumed to be a 16x16 glyph sheet
	float			m_baseAspect = 1.0f; // used as the base aspect ratio for all glyphs

};