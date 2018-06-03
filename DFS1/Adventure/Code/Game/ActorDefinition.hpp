#pragma once
#include "Game/GameCommon.hpp"
#include "EntityDefinition.hpp"
#include "Game/Stats.hpp"
class Item;

class ActorDefinition: public EntityDefinition{
public:
	ActorDefinition(tinyxml2::XMLElement* actorElement);
	~ActorDefinition();


	std::string m_startingFaction;
	Stats m_minStats;
	Stats m_maxStats;


	static std::map< std::string, ActorDefinition* >		s_definitions;
	static ActorDefinition* GetActorDefinition(std::string definitionName);
protected:
	void ParseStats(tinyxml2::XMLElement* statsElement);

};