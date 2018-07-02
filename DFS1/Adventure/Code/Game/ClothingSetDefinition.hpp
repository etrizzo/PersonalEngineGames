#pragma once
#include "Game/GameCommon.hpp"
#include "Game/ClothingLayer.hpp"

class ClothingSet;

class ClothingSetDefinition {
public:
	ClothingSetDefinition(tinyxml2::XMLElement* setElement);

	std::string m_name;

	//all textures loaded in 2D array by RenderSlot
	//i.e. m_texturesByClothingType[0] is all body textures
	std::vector< std::vector<ClothingLayer*> > m_layersByClothingType;
	//std::vector< std::vector<Texture*> > m_texturesByClothingType;
	//std::vector<std::vector<RGBA>> m_tintsByClothingType;
	//std::vector<bool> m_torsoUsesLegs;		//parallel array to m_texturesByClothing[TORSO] to keep track of if it uses leg texture or nah


	ClothingSet* GetRandomSet() const;

	ClothingLayer* GetRandomOfType(RENDER_SLOT slot) const;

	//Texture* GetRandomBody();		//Todo: need to tie together body/ears
	ClothingLayer* GetRandomTorso() const;
	ClothingLayer* GetRandomLegs() const;
	ClothingLayer* GetRandomHair() const;
	ClothingLayer* GetRandomHat() const;

	static std::map< std::string, ClothingSetDefinition* >		s_definitions;
	static ClothingSetDefinition* GetDefinition(std::string definitionName);

	RGBA GetColorFromXML(std::string text);

private:
	ClothingLayer* GetLayer(RENDER_SLOT slot, int index) const;
	Texture* GetTexture(RENDER_SLOT slot, int index) const;
	RGBA GetTint(RENDER_SLOT slot, int index) const;

	void ParseBodys	 (tinyxml2::XMLElement* setElement);
	void ParseTorsos (tinyxml2::XMLElement* setElement);
	void ParseLegs	 (tinyxml2::XMLElement* setElement);
	void ParseHairs	 (tinyxml2::XMLElement* setElement);
	void ParseHats	 (tinyxml2::XMLElement* setElement);

	//parses a single body element - ears and base
	void ParseBody(tinyxml2::XMLElement* bodyElement);
};