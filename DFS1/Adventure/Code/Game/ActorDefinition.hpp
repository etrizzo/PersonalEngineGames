#pragma once
#include "Game/GameCommon.hpp"
#include "EntityDefinition.hpp"
#include "Game/Stats.hpp"

class Item;
class ClothingSetDefinition;

class ActorDefinition: public EntityDefinition{
public:
	ActorDefinition(tinyxml2::XMLElement* actorElement);
	~ActorDefinition();


	std::string m_startingFaction;
	Stats m_minStats;
	Stats m_maxStats;
	ClothingSetDefinition* m_clothingSetDef;


	static std::map< std::string, ActorDefinition* >		s_definitions;
	static ActorDefinition* GetActorDefinition(std::string definitionName);

	std::vector<Texture*> m_layerTextures;

protected:
	void ParseStats(tinyxml2::XMLElement* statsElement);
	void ParseLayersElement(tinyxml2::XMLElement* layersElement);
	void ParseLayer(tinyxml2::XMLElement* layer, RENDER_SLOT slot);

};