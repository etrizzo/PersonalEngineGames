#pragma once
#include "Game/GameCommon.hpp"

class ClothingSetDefinition;

//enum RENDER_SLOT{
//	BODY_SLOT,		//for rendering actors
//	LEGS_SLOT,
//	CHEST_SLOT,
//	HEAD_SLOT,
//	EARS_SLOT,
//	HAT_SLOT,
//	WEAPON_SLOT,
//	NUM_RENDER_SLOTS
//};

class ClothingSet {
public:
	ClothingSet();
	//ClothingSet(ClothingSetDefinition* def);

	//all textures loaded in 2D array by RenderSlot
		//i.e. m_texturesByClothingType[0] is all body textures
	std::vector<Texture*> m_defaultTextures;
	std::vector<Texture*> m_currentTextures;
	std::vector<RGBA> m_currentTints;
	std::vector<RGBA> m_defaultTints;

	void InitTexture(RENDER_SLOT slot, Texture* texture, RGBA tint = RGBA::WHITE);
	void SetDefaultTexture(RENDER_SLOT slot);
	void SetTexture(RENDER_SLOT slot, Texture* newTexture, RGBA tint = RGBA::WHITE);
	void SetTexture(RENDER_SLOT slot, ClothingSetDefinition* setDefinitionName, RGBA tint= RGBA::WHITE);
	Texture* GetTexture(RENDER_SLOT slot) const;
	Texture* GetTexture(int i) const;	//for getting from a for loop
	RGBA GetTint(RENDER_SLOT slot) const;
	RGBA GetTint(int i) const;

	//Texture* GetBodyTexture() const;
	//Texture* GetLegTexture() const;
	//Texture* GetTorsoTexture() const;
	//Texture* GetHairTexture() const;
	//Texture* GetEarsTexture() const;
	//Texture* GetHatTexture() const;
	//Texture* GetWeaponTexture() const;

};