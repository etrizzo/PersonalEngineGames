#include "MapDefinition.hpp"
#include "BlockDefinition.hpp"
#include "Game/MapGenStep.hpp"

std::map<std::string, MapDefinition*>	MapDefinition::s_definitions;

MapDefinition::MapDefinition(tinyxml2::XMLElement* mapDefElement)
{
	m_name							= ParseXmlAttribute(*mapDefElement, "name", "NO_NAME");
	std::string defTileName			= ParseXmlAttribute(*mapDefElement, "defaultTile", "NO_DEFAULT_TILE");
	m_width							= ParseXmlAttribute(*mapDefElement, "width", m_width);
	m_height						= ParseXmlAttribute(*mapDefElement, "height", m_height);
	
	m_defaultTile = BlockDefinition::GetTileDefinition(defTileName);
	MakeGenerationSteps(mapDefElement->FirstChildElement("GenerationSteps"));
}

	void MapDefinition::MakeGenerationSteps(tinyxml2::XMLElement* mapGenElement)
	{
		for (tinyxml2::XMLElement* mapGenStepElement = mapGenElement->FirstChildElement(); mapGenStepElement != NULL; mapGenStepElement = mapGenStepElement->NextSiblingElement()){
			MapGenStep* newGenStep = MapGenStep::CreateMapGenStep(*mapGenStepElement);
			m_generationSteps.push_back(newGenStep);
		}
	}
