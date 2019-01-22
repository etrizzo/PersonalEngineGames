#include "ClothingLayer.hpp"

ClothingLayer::ClothingLayer()
{
	m_slot = NUM_RENDER_SLOTS;
	m_texture = nullptr;
	m_tint = RGBA::WHITE;
}

ClothingLayer::ClothingLayer(ClothingLayer * layer)
{
	m_slot = layer->m_slot;
	m_texture = layer->m_texture;
	m_shouldRenderHair = layer->m_shouldRenderHair;
	m_shouldRenderLegs = layer->m_shouldRenderLegs;
	m_portraitTint = layer->m_portraitTint;
	m_portraitCoords = layer->m_portraitCoords;

	if (layer->m_colorCB != nullptr){
		m_tint = layer->m_colorCB();
		m_portraitTint = m_tint;
	} else {
		m_tint = layer->m_tint;
	}
}

ClothingLayer::ClothingLayer(RENDER_SLOT slot, Texture * tex, RGBA tint, bool renderLegs, bool renderHair, SpawnColorCB colorCB)
{
	m_slot = slot;
	m_texture = tex;
	m_tint = tint;
	m_colorCB = colorCB;

	m_shouldRenderLegs = renderLegs;
	m_shouldRenderHair = renderHair;
}

PortraitLayer::PortraitLayer(ePortraitSlot slot, RGBA tint, SpawnColorCB colorCB, SpriteSheet* spritesheet)
{
	m_slot = slot;
	m_tint = tint;
	if (colorCB != nullptr){
		m_tint = colorCB();
	}
	m_spriteSheet = spritesheet;
	if (m_spriteSheet == nullptr){
		m_spriteSheet = g_portraitSpriteSheet;
	}
	m_texture = m_spriteSheet->GetTexture();
}

void PortraitLayer::SetUVsFromSpriteCoords(IntVector2 spriteCoords)
{
	m_uvs = m_spriteSheet->GetTexCoordsForSpriteCoords(spriteCoords);
}
