#pragma once
#include "Game/GameCommon.hpp"
class VictoryCondition;
class QuestReward;
class ActorDefinition;
class DialogueSetDefinition;
class DialogueSet;

class QuestDefinition{
public:
	QuestDefinition(tinyxml2::XMLElement* questElement);

	DialogueSet* GetDialogueSet(int index = 0);

	std::string m_name;
	std::vector<VictoryCondition*> m_conditions; 
	std::string m_text;
	bool m_isMainQuest;
	bool m_isSequential;
	QuestReward* m_questReward;
	ActorDefinition* m_giverDefinition;
	std::vector<DialogueSetDefinition*> m_dialogues;

	static std::map<std::string, QuestDefinition*> s_definitions;
	static QuestDefinition* GetQuestDefinition(std::string name);

protected:
	void ParseVictoryConditions(tinyxml2::XMLElement* stepsElement);
	void ParseReward(tinyxml2::XMLElement* rewardElement);
	void ParseDialogueSets(tinyxml2::XMLElement* dialogueElement);
};