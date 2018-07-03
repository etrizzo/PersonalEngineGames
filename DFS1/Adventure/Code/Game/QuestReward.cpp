#include "QuestReward.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Item.hpp"
#include "Game/Player.hpp"
#include "Game/Game.hpp"

QuestReward_Item::QuestReward_Item(tinyxml2::XMLElement * questRewardElement)
{
	std::string itemDefName = ParseXmlAttribute(*questRewardElement, "item", "NO_ITEM");
	m_itemToGive = ItemDefinition::GetItemDefinition(itemDefName);
}

void QuestReward_Item::GiveReward()
{
	Item* itemToAdd = new Item(m_itemToGive, nullptr, Vector2::ZERO);
	g_theGame->m_player->AddItemToInventory(itemToAdd);
}

QuestReward_Stats::QuestReward_Stats(tinyxml2::XMLElement * questRewardElement)
{
}

void QuestReward_Stats::GiveReward()
{
}
