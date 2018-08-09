#include "ItemDefinition.hpp"
#include "Game/Stats.hpp"
#include "Game/ClothingSet.hpp"
#include "Game/Game.hpp"

std::map<std::string, ItemDefinition*>	ItemDefinition::s_definitions;

ItemDefinition::ItemDefinition(tinyxml2::XMLElement * itemElement)
	:EntityDefinition(itemElement)
{
	std::string equipSlotName = ParseXmlAttribute(*itemElement, "equipSlot", "NONE");
	std::string clothingSetName = ParseXmlAttribute(*itemElement, "clothingSet", "NONE");
	SetEquipSlot(equipSlotName);

	if (clothingSetName != "NONE"){
		m_clothingSetDefinition = ClothingSetDefinition::GetDefinition(clothingSetName);
		m_showHair = ParseXmlAttribute(*itemElement, "showHair", m_showHair);
		m_defaultSet = m_clothingSetDefinition->GetRandomSet();
	} else {
		m_clothingSetDefinition = nullptr;
		Texture* equipTex = ParseXmlAttribute(*itemElement, "equipTexture", (Texture*) nullptr);
		RGBA tint = ParseXmlAttribute(*itemElement, "equipTint", RGBA::WHITE);
		m_defaultEquipLayer = new ClothingLayer(GetRenderSlotForEquipSlot(m_equipSlot), equipTex, tint);
	}
	ParseStats(itemElement->FirstChildElement("Stats"));	
}

ItemDefinition::~ItemDefinition()
{

}

Texture * ItemDefinition::GetEquipTexture() const
{
	if (m_defaultSet != nullptr){
		return m_defaultSet->GetTexture(GetRenderSlotForEquipSlot(m_equipSlot));
	} else {
		return m_defaultEquipLayer->GetTexture();
	}
}

ClothingLayer * ItemDefinition::GetEquipLayer() const
{
	if (m_defaultSet != nullptr){
		return m_defaultSet->GetLayer(GetRenderSlotForEquipSlot(m_equipSlot));
	} else {
		return m_defaultEquipLayer;
	}
}

ItemDefinition * ItemDefinition::GetItemDefinition(std::string definitionName)
{
	auto containsDef = s_definitions.find((std::string)definitionName); 
	ItemDefinition* itemDef = nullptr;
	if (containsDef != s_definitions.end()){
		itemDef = containsDef->second;
	} 
	return itemDef;
}

void ItemDefinition::SetEquipSlot(std::string equipSlotName)
{
	if (equipSlotName == "head"){
		m_equipSlot = EQUIP_SLOT_HEAD;
	} else if (equipSlotName == "chest"){
		m_equipSlot = EQUIP_SLOT_CHEST;
	} else if (equipSlotName == "legs"){
		m_equipSlot = EQUIP_SLOT_LEGS;
	} else if (equipSlotName == "weapon"){
		m_equipSlot = EQUIP_SLOT_WEAPON;
	} else {
		m_equipSlot = NOT_EQUIPPABLE;
	}
}

void ItemDefinition::ParseStats(tinyxml2::XMLElement * statsElement)
{
	m_minStats = Stats();
	m_maxStats = Stats();

	if (statsElement != nullptr){
		IntRange movementRange	= ParseXmlAttribute(*statsElement, "baseMovement", IntRange(0));
		IntRange strengthRange	= ParseXmlAttribute(*statsElement, "baseStrength", IntRange(0));
		IntRange defenseRange	= ParseXmlAttribute(*statsElement, "baseDefense", IntRange(0));
		m_minStats.SetStat(STAT_MOVEMENT, movementRange.min);
		m_maxStats.SetStat(STAT_MOVEMENT, movementRange.max);
		m_minStats.SetStat(STAT_STRENGTH, strengthRange.min);
		m_maxStats.SetStat(STAT_STRENGTH, strengthRange.max);
		m_minStats.SetStat(STAT_DEFENSE, defenseRange.min);
		m_maxStats.SetStat(STAT_DEFENSE, defenseRange.max);
	}
}
