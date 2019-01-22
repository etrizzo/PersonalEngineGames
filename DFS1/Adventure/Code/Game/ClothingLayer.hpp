#pragma once
#include "Game/GameCommon.hpp"

//const std::string DEFAULT_PORTRAIT_SHEET = "\Images\HUMANOIDS\PORTRAITS\portrait_f.png";

class ClothingLayer{
public:
	ClothingLayer();
	ClothingLayer(ClothingLayer* layer);
	ClothingLayer(RENDER_SLOT slot, Texture* tex, RGBA tint = RGBA::WHITE, bool renderLegs = true, bool renderHair = true, SpawnColorCB colorCB = nullptr);

	inline Texture* GetTexture() const {return m_texture; };
	inline RGBA GetTint() const {return m_tint; };
	inline RENDER_SLOT GetSlot() const {return m_slot; };

	inline bool ShouldRenderLegs() const { return m_shouldRenderLegs; };
	inline bool ShouldRenderHair() const { return m_shouldRenderHair; };

	Texture* m_texture;
	RGBA m_tint;		//instanced
	RGBA m_portraitTint = RGBA::WHITE;
	IntVector2 m_portraitCoords = IntVector2(0,0);
	RENDER_SLOT m_slot;
	SpawnColorCB m_colorCB = nullptr;		//definition (lol)

	//keep track of this for all clothing types, but only really used for torso and hat
	//could maybe just combine this into some sort of dependency bool but names are good
	bool m_shouldRenderLegs = true;
	bool m_shouldRenderHair = true;


};



class PortraitLayer{
public:
	PortraitLayer(ePortraitSlot slot, RGBA tint = RGBA::WHITE, SpawnColorCB colorCB = nullptr, SpriteSheet* spritesheet = nullptr);


	void SetUVsFromSpriteCoords(IntVector2 spriteCoords);

	AABB2 m_uvs = AABB2::ZERO_TO_ONE;
	SpriteSheet* m_spriteSheet = nullptr;
	Texture* m_texture = nullptr;
	RGBA m_tint = RGBA::WHITE;
	ePortraitSlot m_slot = NUM_PORTRAIT_SLOTS;
	SpawnColorCB m_colorCB = nullptr;
};