#pragma once
#include "Game/GameCommon.hpp"

class ItemDefinition;

class QuestReward{
public:
	QuestReward(){};
	QuestReward(tinyxml2::XMLElement* questRewardElement) {};

	virtual void GiveReward() = 0;
};

class QuestReward_Item : public QuestReward{
public:
	QuestReward_Item(tinyxml2::XMLElement* questRewardElement);

	 void GiveReward() override;
	 ItemDefinition* m_itemToGive;
};

class QuestReward_Stats : public QuestReward{
public:
	QuestReward_Stats(tinyxml2::XMLElement* questRewardElement);

	void GiveReward() override;
	Stats* m_statsToGive;
};