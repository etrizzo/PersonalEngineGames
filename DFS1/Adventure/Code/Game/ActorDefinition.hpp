#pragma once
#include "Game/GameCommon.hpp"
#include "EntityDefinition.hpp"
#include "Game/Stats.hpp"
#include "Game/DialogueSetDefinition.hpp"

class Item;
class ClothingSetDefinition;




class ActorDefinition: public EntityDefinition{
public:
	ActorDefinition(tinyxml2::XMLElement* actorElement);
	~ActorDefinition();


	std::string m_startingFaction;
	Stats m_minStats;
	Stats m_maxStats;
	std::vector<ClothingSetDefinition*> m_clothingSetDefs;

	eAIBehavior m_defaultBehavior;

	bool m_isAggressive = false;
	std::vector<Texture*> m_layerTextures;
	std::vector<DialogueSetDefinition*> m_dialogueDefinitions;

	DialogueSetDefinition* GetRandomDialogueDefinition() const;


	static std::map< std::string, ActorDefinition* >		s_definitions;
	static ActorDefinition* GetActorDefinition(std::string definitionName);



protected:
	void ParseStats(tinyxml2::XMLElement* statsElement);
	void ParseLayersElement(tinyxml2::XMLElement* layersElement);
	void ParseLayer(tinyxml2::XMLElement* layer, RENDER_SLOT slot);

};