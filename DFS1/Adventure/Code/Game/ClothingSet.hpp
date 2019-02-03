#pragma once
#include "Game/GameCommon.hpp"
#include "Game/ClothingLayer.hpp"

class ClothingSetDefinition;
class PortraitDefinition;

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

	std::vector<ClothingLayer*> m_defaultLayers;
	std::vector<ClothingLayer*> m_currentLayers;

	void InitLayer(RENDER_SLOT slot, ClothingLayer* layer);
	void SetDefaultLayer(RENDER_SLOT slot);
	void SetLayer(RENDER_SLOT slot, ClothingLayer* layer);

	ClothingLayer* GetLayer(RENDER_SLOT slot) const;
	Texture* GetTexture(RENDER_SLOT slot) const;
	Texture* GetTexture(int i) const;	//for getting from a for loop
	RGBA GetTint(RENDER_SLOT slot) const;
	RGBA GetTint(int i) const;


	RGBA m_portraitSkinTone = RGBA::CYAN;
	RGBA m_hairTint = RGBA::WHITE;
	IntVector2 m_hairPortraitCoords = IntVector2(0,0);
	IntVector2 m_earPortraitCoords = IntVector2(0,0);

	int m_equipmentIndex = 0;

	
	PortraitDefinition* m_portraitDef = nullptr;
	std::vector<std::vector<PortraitLayer*>> m_portraitLayers = std::vector<std::vector<PortraitLayer*>>();
	void InitPortrait();
	int GetNumPortraitLayersForSlot(ePortraitSlot slot) const;
	Texture* GetPortraitTexture(ePortraitSlot slot = FACE_PORTRAIT, int index=0) const;


protected:
	void InitPortraitBody();		//sets the face and ear layers according to set body layers
	void InitPortraitHair();		//sets the hair according to set head layers
	void InitPortraitFace();		// mouth, brows, nose, eyes...
	void InitPortraitFeatures();	//special features

	void InitPortraitMouth();
	void InitPortraitEyes();		//helpers to wrap the special functionality of eyes/mouth
};