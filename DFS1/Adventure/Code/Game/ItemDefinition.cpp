#include "ItemDefinition.hpp"
#include "Game/Stats.hpp"

std::map<std::string, ItemDefinition*>	ItemDefinition::s_definitions;

ItemDefinition::ItemDefinition(tinyxml2::XMLElement * itemElement)
	:EntityDefinition(itemElement)
{
	std::string equipSlotName = ParseXmlAttribute(*itemElement, "equipSlot", "NONE");
	std::string equipTextureName = ParseXmlAttribute(*itemElement, "equipTexture", "NONE");
	if (equipTextureName != "NONE"){
		m_equipTexture = Texture::CreateOrGetTexture(equipTextureName);
		m_showHair = ParseXmlAttribute(*itemElement, "showHair", m_showHair);
	} else {
		m_equipTexture = nullptr;
	}
	SetEquipSlot(equipSlotName);
	ParseStats(itemElement->FirstChildElement("Stats"));	
}

ItemDefinition::~ItemDefinition()
{

}

ItemDefinition * ItemDefinition::GetItemDefinition(std::string definitionName)
{
	auto containsDef = s_definitions.find((std::string)definitionName); 
	ItemDefinition* itemDef = nullptr;
	if (containsDef != s_definitions.end()){
		itemDef = containsDef->second;
	} else {
		ERROR_AND_DIE("No item named: " + definitionName);
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
