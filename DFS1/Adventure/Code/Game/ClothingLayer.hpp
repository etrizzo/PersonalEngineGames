#pragma once
#include "Game/GameCommon.hpp"

class ClothingLayer{
public:
	ClothingLayer();
	ClothingLayer(RENDER_SLOT slot, Texture* tex, RGBA tint = RGBA::WHITE, bool renderLegs = true, bool renderHair = true);

	inline Texture* GetTexture() const {return m_texture; };
	inline RGBA GetTint() const {return m_tint; };
	inline RENDER_SLOT GetSlot() const {return m_slot; };

	inline bool ShouldRenderLegs() const { return m_shouldRenderLegs; };
	inline bool ShouldRenderHair() const { return m_shouldRenderHair; };

	Texture* m_texture;
	RGBA m_tint;
	RENDER_SLOT m_slot;

	//keep track of this for all clothing types, but only really used for torso and hat
	//could maybe just combine this into some sort of dependency bool but names are good
	bool m_shouldRenderLegs = true;
	bool m_shouldRenderHair = true;


};