#include "DataSet.hpp"

std::map<std::string, DataSet*> DataSet::s_dataSets = std::map<std::string, DataSet*>();

DataSet::DataSet(tinyxml2::XMLElement * datasetElement)
{
	m_name = ParseXmlAttribute(*datasetElement, "name", "NO_NAME");
	
	//parse acts
	tinyxml2::XMLElement* actElement = datasetElement->FirstChildElement("Acts");
	ParseActs(actElement);
	//parse characters
	tinyxml2::XMLElement* characterElement = datasetElement->FirstChildElement("Characters");
	ParseCharacters(characterElement);
	//parse events
	tinyxml2::XMLElement* eventElement = datasetElement->FirstChildElement("EventNodes");
	ParseEvents(eventElement);
	//parse outcomes
	tinyxml2::XMLElement* outcomeElement = datasetElement->FirstChildElement("OutcomeNodes");
	ParseOutcomes(outcomeElement);
}

void DataSet::ParseActs(tinyxml2::XMLElement * actElement)
{
	tinyxml2::XMLElement* fileElement = actElement->FirstChildElement("File");
	while (fileElement != nullptr)
	{
		std::string fileName = ParseXmlAttribute(*fileElement, "name", "NO_FILE");
		//read acts
		ReadActsFromXML(fileName);
		fileElement = fileElement->NextSiblingElement("File");
	}

	std::sort(m_actsInOrder.begin(), m_actsInOrder.end(), CompareActsByNumber);
}

void DataSet::ParseCharacters(tinyxml2::XMLElement * charElement)
{
	tinyxml2::XMLElement* fileElement = charElement->FirstChildElement("File");
	while (fileElement != nullptr)
	{
		std::string fileName = ParseXmlAttribute(*fileElement, "name", "NO_FILE");
		ReadCharactersFromXML(fileName);
		fileElement = fileElement->NextSiblingElement("File");
	}
}

void DataSet::ParseEvents(tinyxml2::XMLElement * eventElement)
{
	tinyxml2::XMLElement* fileElement = eventElement->FirstChildElement("File");
	while (fileElement != nullptr)
	{
		std::string fileName = ParseXmlAttribute(*fileElement, "name", "NO_FILE");
		ReadEventNodesFromXML(fileName);
		fileElement = fileElement->NextSiblingElement("File");
	}
}

void DataSet::ParseOutcomes(tinyxml2::XMLElement * outcomeElement)
{
	tinyxml2::XMLElement* fileElement = outcomeElement->FirstChildElement("File");
	while (fileElement != nullptr)
	{
		std::string fileName = ParseXmlAttribute(*fileElement, "name", "NO_FILE");
		ReadOutcomeNodesFromXML(fileName);
		fileElement = fileElement->NextSiblingElement("File");
	}
}

void DataSet::ReadActsFromXML(std::string filePath)
{
	tinyxml2::XMLDocument actDoc;
	//	std::string filePath = "Data/Data/" + fileName;
	actDoc.LoadFile(filePath.c_str());
	for (tinyxml2::XMLElement* actElement = actDoc.FirstChildElement("Act"); actElement != NULL; actElement = actElement->NextSiblingElement("Act")){
		Act act = Act(actElement);
		m_actsInOrder.push_back(act);
	}
}

void DataSet::ReadEventNodesFromXML(std::string filePath)
{
	tinyxml2::XMLDocument nodeDoc;
	//	std::string filePath = "Data/Data/" + fileName;
	nodeDoc.LoadFile(filePath.c_str());
	for (tinyxml2::XMLElement* nodeElement = nodeDoc.FirstChildElement("EventNode"); nodeElement != NULL; nodeElement = nodeElement->NextSiblingElement("EventNode")){
		StoryDataDefinition* data = new StoryDataDefinition( PLOT_NODE);
		data->InitFromXML(nodeElement);
		m_eventNodes.push_back(data);
	}
}

void DataSet::ReadOutcomeNodesFromXML(std::string filePath)
{
	tinyxml2::XMLDocument nodeDoc;
	//	std::string filePath = "Data/Data/" + fileName;
	nodeDoc.LoadFile(filePath.c_str());
	for (tinyxml2::XMLElement* nodeElement = nodeDoc.FirstChildElement("OutcomeNode"); nodeElement != NULL; nodeElement = nodeElement->NextSiblingElement("OutcomeNode")){
		StoryDataDefinition* data = new StoryDataDefinition( DETAIL_NODE);
		data->InitFromXML(nodeElement);
		m_outcomeNodes.push_back(data);
	}
}

void DataSet::ReadCharactersFromXML(std::string filePath)
{
	tinyxml2::XMLDocument charDoc;
	//	std::string filePath = "Data/Data/" + fileName;
	charDoc.LoadFile(filePath.c_str());
	for (tinyxml2::XMLElement* charElement = charDoc.FirstChildElement("Character"); charElement != NULL; charElement = charElement->NextSiblingElement("Character")){
		Character* newChar = new Character();
		newChar->InitFromXML(charElement);
		m_characters.push_back(newChar);
	}
}

StoryDataDefinition * DataSet::GetRandomEventNode()
{
	int i = GetRandomIntLessThan(m_eventNodes.size());
	return m_eventNodes[i];
}

StoryDataDefinition * DataSet::GetRandomOutcomeNode()
{
	int i = GetRandomIntLessThan(m_outcomeNodes.size());
	return m_outcomeNodes[i];
}

int DataSet::GetActNumberForName(std::string name) const
{
	for (int i = 0; i < (int) m_actsInOrder.size(); i++)
	{
		if (m_actsInOrder[i].m_name == name)
		{
			return m_actsInOrder[i].m_number;
		} else {
			return -1;
		}
	}
}

int DataSet::GetNumActs() const
{
	return (int) m_actsInOrder.size();
}

int DataSet::GetFinalActNumber() const
{
	int maxNum = 0;
	for (int i = 0; i < (int) m_actsInOrder.size(); i++)
	{
		maxNum = Max(maxNum, m_actsInOrder[i].m_number);
	}
	return maxNum;
}

StoryDataDefinition * DataSet::GetOutcomeNodeWithWeights(StoryState * edge, float minFitness)
{
	std::vector<StoryDataDefinition*> fitNodes = std::vector<StoryDataDefinition*>();
	std::vector<StoryDataDefinition*> defaultNodes = std::vector<StoryDataDefinition*>();
	for(StoryDataDefinition* data : m_outcomeNodes){
		if (DoRangesOverlap(data->m_actRange, edge->m_possibleActRange)) {
			float fitness = CalculateEdgeFitnessForData(edge, data);
			if (fitness >= minFitness) {
				if (fitness >= 1.f) {
					//try to populate the array with the most fit nodes being more likely
					int intFitness = (int)fitness;
					for (int i = 0; i < intFitness; i++) {
						fitNodes.push_back(data);
					}
				}
				else {
					//by default the node is added once to the array
					fitNodes.push_back(data);
				}
			}
			else {
				if (fitness >= 1.f) {
					//try to populate the array with the most fit nodes being more likely
					int intFitness = (int)fitness;
					for (int i = 0; i < intFitness; i++) {
						defaultNodes.push_back(data);
					}
				}
				else {
					//by default the node is added once to the array
					defaultNodes.push_back(data);
				}
			}
		}
	}

	//ASSERT_OR_DIE((defaultNodes.size() > 0 || fitNodes.size() > 0), "No possible outcome nodes for act");


	if (defaultNodes.size() == 0 && fitNodes.size() == 0)
	{
		return nullptr;
	}

	StoryDataDefinition* chosenNode = nullptr;
	do 
	{
		if (fitNodes.size() > 0){
			int i = GetRandomIntLessThan(fitNodes.size());
			chosenNode = fitNodes[i];
		} else {
			int i = GetRandomIntLessThan(defaultNodes.size());
			chosenNode = defaultNodes[i];
		}
	} while (!CheckRandomChance(chosenNode->m_chanceToPlaceData));
	return chosenNode;
}

StoryDataDefinition * DataSet::GetEventNodeWithWeights(StoryState * edge, float minFitness)
{
	std::vector<StoryDataDefinition*> fitNodes = std::vector<StoryDataDefinition*>();
	std::vector<StoryDataDefinition*> defaultNodes = std::vector<StoryDataDefinition*>();
	for(StoryDataDefinition* data : m_eventNodes){
		if (DoRangesOverlap(data->m_actRange, edge->m_possibleActRange)) {
			float fitness = CalculateEdgeFitnessForData(edge, data);
			if (fitness >= minFitness) {
				if (fitness >= 1.f) {
					//try to populate the array with the most fit nodes being more likely
					int intFitness = (int)fitness;
					for (int i = 0; i < intFitness; i++) {
						fitNodes.push_back(data);
					}
				}
				else {
					//by default the node is added once to the array
					fitNodes.push_back(data);
				}
			}
			else {
				if (fitness >= 1.f) {
					//try to populate the array with the most fit nodes being more likely
					int intFitness = (int)fitness;
					for (int i = 0; i < intFitness; i++) {
						defaultNodes.push_back(data);
					}
				}
				else {
					//by default the node is added once to the array
					defaultNodes.push_back(data);
				}
			}
		}
	}

	ASSERT_OR_DIE((defaultNodes.size() > 0 || fitNodes.size() > 0), "No possible event nodes for act");

	StoryDataDefinition* chosenNode = nullptr;
	do 
	{
		if (fitNodes.size() > 0){
			int i = GetRandomIntLessThan(fitNodes.size());
			chosenNode = fitNodes[i];
		} else {
			int i = GetRandomIntLessThan(defaultNodes.size());
			chosenNode = defaultNodes[i];
		}
	} while (!CheckRandomChance(chosenNode->m_chanceToPlaceData));
	return chosenNode;
}

float DataSet::CalculateEdgeFitnessForData(StoryState * edge, StoryDataDefinition * data)
{
	float fitness = 0.f;
	for (CharacterState* charState : edge->m_characterStates){
		Character* character = charState->m_character;
		CharacterRequirementSet* reqs = data->GetRequirementsForCharacter(character);
		if (reqs != nullptr){
			//if the edges' character has requirements already established in this node, check them.
			float charFitnessForRequirements = reqs->GetCharacterFitness(character, edge);
			fitness+=charFitnessForRequirements;
		} else {
			//it technically works so like +1 i guess
			fitness+=1.f;
		}
	}

	//also want to check story requirements....
	if(data->m_storyReqs->DoesEdgeMeetAllRequirements(edge)){
		fitness+= (float) data->m_storyReqs->m_requirements.size();
	} else {
		fitness = 0.f;
	}

	//if no characters violated requirements, return true.
	return fitness;
}

DataSet * DataSet::GetDataSet(std::string dataSetName)
{
	auto pair = DataSet::s_dataSets.find(dataSetName);
	if (pair != DataSet::s_dataSets.end())
	{
		return pair->second;
	}
	return nullptr;
}

bool CompareActsByNumber(const Act & first, const Act & second)
{
	return first.m_number < second.m_number;
}
