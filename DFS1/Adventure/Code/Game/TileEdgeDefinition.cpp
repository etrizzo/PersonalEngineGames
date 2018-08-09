#include "TileEdgeDefinition.hpp"

std::map<std::string, TileEdgeDefinition*>	TileEdgeDefinition::s_definitions;


TileEdgeDefinition::TileEdgeDefinition(tinyxml2::XMLElement * tileDefElement)
{
	m_name = ParseXmlAttribute(*tileDefElement, "name", "NO_NAME");
	IntVector2 topLeft = ParseXmlAttribute(*tileDefElement, "topLeftSpriteCoord", IntVector2(0,0));
	for (int y = 0; y < 5; y++){
		for (int x = 0; x < 3; x++){
			m_spriteCoords.push_back(topLeft + IntVector2(x,y));
		}
	}

	s_definitions.insert(std::pair<std::string, TileEdgeDefinition*>(m_name, this));
}

AABB2 TileEdgeDefinition::GetTexCoordsForEdge(eEdgeType edgeType)
{
	return g_tileSpriteSheet->GetTexCoordsForSpriteCoords(m_spriteCoords[edgeType]);
}

TileEdgeDefinition * TileEdgeDefinition::GetEdgeDefinition(std::string name)
{
	auto containsDef = s_definitions.find((std::string)name); 
	TileEdgeDefinition* edgeDef = nullptr;
	if (containsDef != s_definitions.end()){
		edgeDef = containsDef->second;
	}
	return edgeDef;
}
