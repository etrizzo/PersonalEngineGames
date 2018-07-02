#include "ClothingLayer.hpp"

ClothingLayer::ClothingLayer()
{
	m_slot = NUM_RENDER_SLOTS;
	m_texture = nullptr;
	m_tint = RGBA::WHITE;
}

ClothingLayer::ClothingLayer(RENDER_SLOT slot, Texture * tex, RGBA tint, bool renderLegs, bool renderHair)
{
	m_slot = slot;
	m_texture = tex;
	m_tint = tint;

	m_shouldRenderLegs = renderLegs;
	m_shouldRenderHair = renderHair;
}


