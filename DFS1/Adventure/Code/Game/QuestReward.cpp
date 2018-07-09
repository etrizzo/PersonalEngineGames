#include "QuestReward.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Item.hpp"
#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Stats.hpp"
#include "Game/QuestReward.hpp"

QuestReward_Item::QuestReward_Item(const tinyxml2::XMLElement * questRewardElement)
{
	std::string itemDefName = ParseXmlAttribute(*questRewardElement, "item", "NO_ITEM");
	m_itemToGive = ItemDefinition::GetItemDefinition(itemDefName);
}

void QuestReward_Item::GiveReward()
{
	Item* itemToAdd = new Item(m_itemToGive, nullptr, Vector2::ZERO);
	g_theGame->m_player->AddItemToInventory(itemToAdd);
}

QuestReward_Stats::QuestReward_Stats(const tinyxml2::XMLElement * questRewardElement)
{
}

void QuestReward_Stats::GiveReward()
{
}

QuestReward * QuestReward::CreateQuestReward(const tinyxml2::XMLElement * rewardElement)
{
	std::string rewardName = ParseXmlAttribute(*rewardElement, "type", "NO_REWARD");
	QuestReward* reward = nullptr;
	if (rewardName == "giveItem"){
		reward  = (QuestReward*) new QuestReward_Item(rewardElement);
	}
	if (rewardName == "giveStats"){
		reward  = (QuestReward*) new QuestReward_Stats(rewardElement);
	}
	/*if (rewardName == "CollectItem"){
		newCondition  = (VictoryCondition*) new VictoryCondition_CollectItem(conditionElement, nullptr);
	}*/
	return reward;
}
