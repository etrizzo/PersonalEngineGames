#include "Game/QuestReward.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Item.hpp"
#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Stats.hpp"
#include "Game/Quest.hpp"
#include "Game/Party.hpp"

QuestReward * QuestReward::CreateQuestReward(const tinyxml2::XMLElement * rewardElement )
{
	std::string rewardName = ParseXmlAttribute(*rewardElement, "type", "NO_REWARD");
	QuestReward* reward = nullptr;
	if (rewardName == "giveItem"){
		reward  = (QuestReward*) new QuestReward_Item(rewardElement);
	}
	if (rewardName == "giveStats"){
		reward  = (QuestReward*) new QuestReward_Stats(rewardElement);
	}
	if (rewardName == "gainAlly"){
		reward = (QuestReward*) new QuestReward_Ally(rewardElement);
	}
	return reward;
}

QuestReward_Item::QuestReward_Item(const tinyxml2::XMLElement * questRewardElement)
{
	std::string itemDefName = ParseXmlAttribute(*questRewardElement, "item", "NO_ITEM");
	m_itemToGive = ItemDefinition::GetItemDefinition(itemDefName);
}

void QuestReward_Item::GiveReward(Quest* quest)
{
	UNUSED(quest);
	Item* itemToAdd = new Item(m_itemToGive, nullptr, Vector2::ZERO);
	g_theGame->m_party->AddItemToInventory(itemToAdd);
}

QuestReward_Stats::QuestReward_Stats(const tinyxml2::XMLElement * questRewardElement)
{
	m_statsToGive = new Stats();
	int strength = ParseXmlAttribute(*questRewardElement, "statStrength", 0);
	int speed = ParseXmlAttribute(*questRewardElement, "statSpeed", 0);
	int defense = ParseXmlAttribute(*questRewardElement, "statDefense", 0);

	m_statsToGive->SetStat(STAT_STRENGTH, strength);
	m_statsToGive->SetStat(STAT_MOVEMENT, speed);
	m_statsToGive->SetStat(STAT_DEFENSE, defense);
}

void QuestReward_Stats::GiveReward(Quest* quest)
{
	UNUSED(quest);
	g_theGame->m_party->GetPlayerCharacter()->m_stats.Add(*m_statsToGive);
	g_theGame->m_party->GetPlayerCharacter()->m_baseStats.Add(*m_statsToGive);
}

QuestReward_Ally::QuestReward_Ally(const tinyxml2::XMLElement * questRewardElement)
{
	UNUSED(questRewardElement);
}

void QuestReward_Ally::GiveReward(Quest* quest)
{
	if (quest != nullptr){
		g_theGame->m_party->AddActorToParty(quest->m_questGiver);
	}
}
