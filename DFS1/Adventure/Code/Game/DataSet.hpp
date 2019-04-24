#pragma once
#include "Game/GameCommon.hpp"
#include "Game/StoryData.hpp"
#include "Game/Act.hpp"

class Character;


class DataSet
{
public:
	DataSet(tinyxml2::XMLElement* datasetElement);

	std::string m_name;
	std::vector<Character*> m_characters				= std::vector<Character*>();
	std::vector<StoryDataDefinition*> m_eventNodes		= std::vector<StoryDataDefinition*>();
	std::vector<StoryDataDefinition*> m_outcomeNodes	= std::vector<StoryDataDefinition*>();
	std::vector<Act> m_actsInOrder						= std::vector<Act>();


	std::vector<StoryDataDefinition*> m_usedEventNodes;
	std::vector<StoryDataDefinition*> m_usedOutcomeNodes;

	//for reference - all nodes that have an act-ending action
	std::vector<StoryDataDefinition*> m_actEndingNodes	= std::vector<StoryDataDefinition*>();
	std::vector<StoryDataDefinition*> m_unusedEndNodes = std::vector<StoryDataDefinition*>();

	void MarkEventNodeUsed(StoryDataDefinition* eventNodeDef);
	void MarkOutcomeNodeUsed(StoryDataDefinition* outcomeNodeDef);

	// decides priority based on which acts to place next & number of times a node has been used already
	// equal priority nodes are shuffled in place
	std::vector<StoryDataDefinition*> GetPrioritizedEventNodes();
	std::vector<StoryDataDefinition*> GetPrioritizedOutcomes();

	int CalculateNodePriority(StoryDataDefinition* def) const;

	void ResetUsedEndNodes();
	void ResetDataSet();

	StoryDataDefinition* GetRandomEventNode();
	StoryDataDefinition* GetRandomOutcomeNode();
	int GetActNumberForName(std::string name) const;
	int GetNumActs() const;
	int GetFinalActNumber() const;

	bool DoesActMeetNumNodeRequirement(int actNumber, int numNodes) const;


	StoryDataDefinition* GetOutcomeNodeWithWeights(StoryState* edge, float minFitness = .75f);
	StoryDataDefinition* GetEventNodeWithWeights(StoryState* edge, float minFitness = .75f);

	void RemoveEndingFromUnusedEndings(StoryDataDefinition* ending);

	StoryDataDefinition* GetEndingNode(StoryState* edge);

	//calculate the approximate fitness of a node that doesn't have characters assigned yet
	float CalculateEdgeFitnessForNewDataZeroToOne(StoryState* edge, StoryDataDefinition* data);
	//calculate the fitness for a node that already exists
	float CalculateEdgeFitnessForData(StoryState* edge, StoryDataDefinition* data);

	float GetNodeLikelihoodToLeadToEnding(StoryDataDefinition* nodeDef);


	static std::map<std::string, DataSet*> s_dataSets;
	static DataSet* GetDataSet(std::string dataSetName);

	IntRange m_numCharactersToUse = IntRange(1);

private:

	//initialization
	void ParseActs(tinyxml2::XMLElement* actElement);
	void ParseCharacters(tinyxml2::XMLElement* charElement);
	void ParseEvents(tinyxml2::XMLElement* eventElement);
	void ParseOutcomes(tinyxml2::XMLElement* outcomeElement);
	void ReadActsFromXML(std::string filePath);
	void ReadEventNodesFromXML(std::string filePath);
	void ReadOutcomeNodesFromXML(std::string filePath);
	void ReadCharactersFromXML(std::string filePath);

};

bool CompareActsByNumber(const Act& first, const Act& second);

struct SortableNode
{
	StoryDataDefinition* definition;
	int priority;

	bool operator<(const SortableNode& compare) const;
};

bool CompareNodesByPriority(const SortableNode& first, const SortableNode& second);

