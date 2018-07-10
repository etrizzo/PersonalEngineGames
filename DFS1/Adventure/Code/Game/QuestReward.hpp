#pragma once
#include "Game/GameCommon.hpp"


class Stats;
class ItemDefinition;
class Quest;

class QuestReward{
public:
	QuestReward(){};
	QuestReward(tinyxml2::XMLElement* questRewardElement) {};

	virtual void GiveReward(Quest* quest) = 0;

	static QuestReward* CreateQuestReward( const tinyxml2::XMLElement* conditionElement);
};

class QuestReward_Item : public QuestReward{
public:
	QuestReward_Item(const tinyxml2::XMLElement* questRewardElement);

	 void GiveReward(Quest* quest) override;
	 ItemDefinition* m_itemToGive;
};

class QuestReward_Stats : public QuestReward{
public:
	QuestReward_Stats(const tinyxml2::XMLElement* questRewardElement);

	void GiveReward(Quest* quest) override;
	Stats* m_statsToGive;
};


class QuestReward_Ally : public QuestReward{
public:
	QuestReward_Ally(const tinyxml2::XMLElement* questRewardElement);

	void GiveReward(Quest* quest) override;
	Stats* m_statsToGive;
};
