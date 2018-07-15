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

	if (layer->m_colorCB != nullptr){
		m_tint = layer->m_colorCB();
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


