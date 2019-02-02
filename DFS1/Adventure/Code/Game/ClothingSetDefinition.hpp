#pragma once
#include "Game/GameCommon.hpp"
#include "Game/ClothingLayer.hpp"

class ClothingSet;
class PortraitDefinition;

SpawnColorCB GetColorCallbackFromXML(std::string text);

class ClothingSetDefinition {
public:
	ClothingSetDefinition(tinyxml2::XMLElement* setElement);

	std::string m_name;
	int m_equipmentIndex = 0;

	//all textures loaded in 2D array by RenderSlot
	//i.e. m_texturesByClothingType[0] is all body textures
	std::vector< std::vector<ClothingLayer*> > m_layersByClothingType;

	ClothingSet* GetRandomSet() const;

	ClothingLayer* GetRandomOfType(RENDER_SLOT slot) const;

	//Texture* GetRandomBody();		//Todo: need to tie together body/ears
	ClothingLayer* GetRandomTorso() const;
	ClothingLayer* GetRandomLegs() const;
	ClothingLayer* GetRandomHair() const;
	ClothingLayer* GetRandomHat() const;


	static std::map< std::string, ClothingSetDefinition* >		s_definitions;
	static ClothingSetDefinition* GetDefinition(std::string definitionName);

	

	PortraitDefinition* m_portraitDef = nullptr;

private:
	ClothingLayer* GetLayer(RENDER_SLOT slot, int index) const;
	Texture* GetTexture(RENDER_SLOT slot, int index) const;
	RGBA GetTint(RENDER_SLOT slot, int index) const;

	void ParseBodys	 (tinyxml2::XMLElement* setElement);
	void ParseTorsos (tinyxml2::XMLElement* setElement);
	void ParseLegs	 (tinyxml2::XMLElement* setElement);
	void ParseHairs	 (tinyxml2::XMLElement* setElement);
	void ParseHats	 (tinyxml2::XMLElement* setElement);
	void ParseWeapons(tinyxml2::XMLElement* setElement);
	



	//parses a single body element - ears and base
	void ParseBody(tinyxml2::XMLElement* bodyElement);
};


struct sPortraitPieceDefinition{
	std::string m_name;
	IntVector2 m_spriteCoords;
	RGBA m_defaultTint = RGBA::WHITE;
	SpawnColorCB m_colorCallback = nullptr;

	RGBA GetColorForInstance() const;
};

//all that could make a portrait defined in data
class PortraitDefinition{
public:
	PortraitDefinition(tinyxml2::XMLElement* portraitElement);

	IntRange m_numFeaturesPossible = IntRange(0);
	float m_lipColorChance = 0.f;

	IntVector2 GetRandomFaceCoords() const;
	IntVector2 GetRandomNoseCoords() const;
	IntVector2 GetRandomBrowCoords() const;
	IntVector2 GetRandomFeatureCoords() const;

	std::vector<sPortraitPieceDefinition*> m_faces;
	std::vector<sPortraitPieceDefinition*> m_mouthLines;	//parse both sprites for the mouths
	std::vector<sPortraitPieceDefinition*> m_lipMasks;
	std::vector<sPortraitPieceDefinition*> m_noses;
	std::vector<sPortraitPieceDefinition*> m_brows;
	std::vector<sPortraitPieceDefinition*> m_eyeWhites;		//parse both for the eyes
	std::vector<sPortraitPieceDefinition*> m_eyePupils;
	std::vector<sPortraitPieceDefinition*> m_features;

protected:
	void ParseFaces(tinyxml2::XMLElement* portraitElement);
	void ParseMouths(tinyxml2::XMLElement* portraitElement);
	void ParseNoses(tinyxml2::XMLElement* portraitElement);
	void ParseBrows(tinyxml2::XMLElement* portraitElement);
	void ParseEyes(tinyxml2::XMLElement* portraitElement);
	void ParseFeatures(tinyxml2::XMLElement* portraitElement);
};