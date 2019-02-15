#pragma once
#include "Game/GameCommon.hpp"

class ActorDefinition;
class TileDefinition;

class VillageDefinition
{
public:
	VillageDefinition(tinyxml2::XMLElement* villageElement);

	std::string m_definitionName						= "NO_NAME";

	std::string m_nameSourceFile						= "NO_FILE";		//necessary? really just reading it in the one time....
	Strings m_villageNames								= Strings();
	Strings m_unusedVillageNames						= Strings();		//shuffled version of village names, where we remove one every time we make a village
	std::vector<ActorDefinition*> m_residentDefinitions = std::vector<ActorDefinition*>();
	IntRange m_numResidents								= IntRange(0);		//might be better if set in the map generation step instead of on definition....
	Strings m_dataSetForGraphFilenames					= Strings();		//the datasets that can be used in this village's story, chooses one at spawn and creates graph from it
	Strings m_factions									= Strings();		//factions that this village could be

	TileDefinition* m_baseTileDefinition				= nullptr;

	std::string GetRandomFaction() const;
	ActorDefinition* GetRandomResidentDefinition() const;
	std::string GetRandomVillageNameAndCrossOff();
	

	static std::map< std::string, VillageDefinition* >			s_definitions;
	static VillageDefinition* GetVillageDefinition(std::string definitionName);

private:
	void ReadVillageNamesFromSourceFile();
	void LoadUnusedVillageNamesFromBaseAndShuffle();
};