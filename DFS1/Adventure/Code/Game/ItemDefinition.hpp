#pragma once
#include "Game/EntityDefinition.hpp"
#include "Game/Stats.hpp"



class ItemDefinition: public EntityDefinition{
public:
	ItemDefinition(tinyxml2::XMLElement* itemElement);
	~ItemDefinition();

	EQUIPMENT_SLOT m_equipSlot = NOT_EQUIPPABLE;
	Stats m_minStats;
	Stats m_maxStats;

	static std::map< std::string, ItemDefinition* >		s_definitions;
	static ItemDefinition* GetItemDefinition(std::string definitionName);
private:
	void SetEquipSlot(std::string equipSlotName);
	void ParseStats(tinyxml2::XMLElement* statsElement);
};