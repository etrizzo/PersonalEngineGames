#pragma once
#include "Game/GameCommon.hpp"

class ClothingSet;

class ClothingSetDefinition {
public:
	ClothingSetDefinition(tinyxml2::XMLElement* setElement);

	std::string m_name;

	//all textures loaded in 2D array by RenderSlot
	//i.e. m_texturesByClothingType[0] is all body textures
	std::vector< std::vector<Texture*> > m_texturesByClothingType;
	std::vector<bool> m_torsoUsesLegs;		//parallel array to m_texturesByClothing[TORSO] to keep track of if it uses leg texture or nah


	ClothingSet* GetRandomSet() const;

	Texture* GetRandomOfType(RENDER_SLOT slot) const;

	//Texture* GetRandomBody();		//Todo: need to tie together body/ears
	Texture* GetRandomTorso() const;
	Texture* GetRandomLegs() const;
	Texture* GetRandomHair() const;
	Texture* GetRandomHat() const;

	static std::map< std::string, ClothingSetDefinition* >		s_definitions;
	static ClothingSetDefinition* GetDefinition(std::string definitionName);

private:
	Texture* GetTexture(RENDER_SLOT slot, int index) const;

	void ParseBodys	 (tinyxml2::XMLElement* setElement);
	void ParseTorsos (tinyxml2::XMLElement* setElement);
	void ParseLegs	 (tinyxml2::XMLElement* setElement);
	void ParseHairs	 (tinyxml2::XMLElement* setElement);
	void ParseHats	 (tinyxml2::XMLElement* setElement);

	//parses a single body element - ears and base
	void ParseBody(tinyxml2::XMLElement* bodyElement);
};