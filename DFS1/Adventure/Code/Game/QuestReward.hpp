#pragma once
#include "Game/GameCommon.hpp"


class Stats;
class ItemDefinition;

class QuestReward{
public:
	QuestReward(){};
	QuestReward(tinyxml2::XMLElement* questRewardElement) {};

	virtual void GiveReward() = 0;

	static QuestReward* CreateQuestReward( const tinyxml2::XMLElement* conditionElement );
};

class QuestReward_Item : public QuestReward{
public:
	QuestReward_Item(const tinyxml2::XMLElement* questRewardElement);

	 void GiveReward() override;
	 ItemDefinition* m_itemToGive;
};

class QuestReward_Stats : public QuestReward{
public:
	QuestReward_Stats(const tinyxml2::XMLElement* questRewardElement);

	void GiveReward() override;
	Stats* m_statsToGive;
};