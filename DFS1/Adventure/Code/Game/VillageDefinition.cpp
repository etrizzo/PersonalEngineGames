#include "VillageDefinition.hpp"
#include "Game/ActorDefinition.hpp"

std::map<std::string, VillageDefinition*>	VillageDefinition::s_definitions;

VillageDefinition::VillageDefinition(tinyxml2::XMLElement * villageElement)
{
	m_definitionName = ParseXmlAttribute(*villageElement, "definitionName", m_definitionName);
	m_nameSourceFile = ParseXmlAttribute(*villageElement, "nameFile", m_nameSourceFile);
	
	//parse residents
	tinyxml2::XMLElement* residentElement = villageElement->FirstChildElement("Residents");
	if (residentElement != nullptr)
	{
		m_numResidents = ParseXmlAttribute(*residentElement, "number", m_numResidents);

		//add actor definitions for each child actor type
		tinyxml2::XMLElement* actorElement = residentElement->FirstChildElement("Actor");
		while (actorElement != nullptr)
		{
			std::string actorDefName = ParseXmlAttribute(*actorElement, "name", "NO_ACTOR");
			m_residentDefinitions.push_back(ActorDefinition::GetActorDefinition(actorDefName));
			actorElement = actorElement->NextSiblingElement("Actor");
		}
	}

	//parse story graph datasets
	tinyxml2::XMLElement* storysetElement = villageElement->FirstChildElement("StorySets");
	if (storysetElement != nullptr){
		tinyxml2::XMLElement* storyElement = storysetElement->FirstChildElement("StorySet");
		while(storyElement != nullptr){
			std::string setFile  = ParseXmlAttribute(*storyElement, "dataSet", "NO_DATASET");
			m_dataSetForGraphFilenames.push_back(setFile);
			storyElement = storyElement->NextSiblingElement("StoryElement");
		}
	}

	//parse factions
	tinyxml2::XMLElement* factionsElement = villageElement->FirstChildElement("Factions");
	if (factionsElement != nullptr){
		tinyxml2::XMLElement* factionElement = factionsElement->FirstChildElement("Faction");
		while(factionElement != nullptr){
			std::string factionName  = ParseXmlAttribute(*factionElement, "name", "NO_FACTION");
			m_factions.push_back(factionName);
			factionElement = factionElement->NextSiblingElement("Faction");
		}
	}
		
	ReadVillageNamesFromSourceFile();
	LoadUnusedVillageNamesFromBaseAndShuffle();
}

std::string VillageDefinition::GetRandomFaction() const
{
	if (m_factions.size() == 0){
		return "none";
	}
	return m_factions[GetRandomIntLessThan(m_factions.size())];
}

ActorDefinition * VillageDefinition::GetRandomResidentDefinition() const
{
	if (m_residentDefinitions.size() == 0){
		return nullptr;
	}
	return m_residentDefinitions[GetRandomIntLessThan(m_residentDefinitions.size())];
}

std::string VillageDefinition::GetRandomVillageNameAndCrossOff()
{
	if (m_unusedVillageNames.size() == 0){
		LoadUnusedVillageNamesFromBaseAndShuffle();
		if (m_unusedVillageNames.size() == 0){
			return "NoName";
		}
	}
	std::string nameToUse = m_unusedVillageNames.back();
	m_unusedVillageNames.pop_back();
	return nameToUse;
}

std::string VillageDefinition::GetRandomDataSetName() const
{
	return m_dataSetForGraphFilenames[GetRandomIntLessThan(m_dataSetForGraphFilenames.size())];
}

VillageDefinition * VillageDefinition::GetVillageDefinition(std::string definitionName)
{
	auto containsDef = s_definitions.find((std::string)definitionName); 
	VillageDefinition* villageDef = nullptr;
	if (containsDef != s_definitions.end()){
		villageDef = containsDef->second;
	} else {
		ERROR_AND_DIE("No village definition named: " + definitionName);
	}
	return villageDef;
}

void VillageDefinition::ReadVillageNamesFromSourceFile()
{
	FILE *fp = nullptr;
	fopen_s( &fp, m_nameSourceFile.c_str(), "r" );
	char lineCSTR [1000];
	std::string line;
	int MAX_LINE_LENGTH = 1000;

	ASSERT_OR_DIE(fp != nullptr, "NO VILLAGE NAME FILE FOUND");
	while (fgets( lineCSTR, MAX_LINE_LENGTH, fp ) != NULL)
	{
		line = "";
		line.append(lineCSTR);
		Strip(line, '\n');
		m_villageNames.push_back(line);
	}
}

void VillageDefinition::LoadUnusedVillageNamesFromBaseAndShuffle()
{
	//copy base village names into the vector of unused village names
	m_unusedVillageNames = Strings(m_villageNames);

	//shuffle the list
	for (int i = 0; i < (int) m_unusedVillageNames.size(); i++){
		int randomSwapIndex = GetRandomIntLessThan((int) m_unusedVillageNames.size());
		std::swap(m_unusedVillageNames[i], m_unusedVillageNames[randomSwapIndex]);
	}
}
