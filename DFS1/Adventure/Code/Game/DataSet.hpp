#pragma once
#include "Game/GameCommon.hpp"
#include "Game/StoryData.hpp"

class Character;

class DataSet
{
public:
	DataSet(tinyxml2::XMLElement* datasetElement);

	std::string m_name;
	std::vector<Character*> m_characters				= std::vector<Character*>();
	std::vector<StoryDataDefinition*> m_eventNodes		= std::vector<StoryDataDefinition*>();
	std::vector<StoryDataDefinition*> m_outcomeNodes	= std::vector<StoryDataDefinition*>();

	//initialization
	void ParseActs(tinyxml2::XMLElement* actElement);
	void ParseCharacters(tinyxml2::XMLElement* charElement);
	void ParseEvents(tinyxml2::XMLElement* eventElement);
	void ParseOutcomes(tinyxml2::XMLElement* outcomeElement);
	void ReadEventNodesFromXML(std::string filePath);
	void ReadOutcomeNodesFromXML(std::string filePath);
	void ReadCharactersFromXML(std::string filePath);

	
	StoryDataDefinition* GetRandomEventNode();
	StoryDataDefinition* GetRandomOutcomeNode();
	
	StoryDataDefinition* GetOutcomeNodeWithWeights(StoryState* edge, float minFitness = 2.f);
	StoryDataDefinition* GetEventNodeWithWeights(StoryState* edge, float minFitness = 2.f);
	float CalculateEdgeFitnessForData(StoryState* edge, StoryDataDefinition* data);


	static std::map<std::string, DataSet*> s_dataSets;
	static DataSet* GetDataSet(std::string dataSetName);
};