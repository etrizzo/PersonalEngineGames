#pragma once
#include "Game/GameCommon.hpp"

enum eEdgeType{
	EDGE_SINGLE_LARGE,
	EDGE_CONCAVE_TOPLEFT,
	EDGE_CONCAVE_TOPRIGHT,

	EDGE_SINGLE_SMALL,
	EDGE_CONCAVE_BOTTOMLEFT,
	EDGE_CONCAVE_BOTTOMRIGHT,

	EDGE_CONVEX_TOPLEFT,
	EDGE_PURE_TOP,
	EDGE_CONVEX_TOPRIGHT,

	EDGE_PURE_LEFT,
	EDGE_EMPTY_TILE,
	EDGE_PURE_RIGHT,

	EDGE_CONVEX_BOTTOMLEFT,
	EDGE_PURE_BOTTOM,
	EDGE_CONVEX_BOTTOMRIGHT,
	
	NUM_EDGE_TYPES
};

class TileEdgeDefinition{
public:
	TileEdgeDefinition() {};
	TileEdgeDefinition(tinyxml2::XMLElement* tileDefElement);

	std::string m_name;
	std::vector<IntVector2> m_spriteCoords;

	AABB2 GetTexCoordsForEdge(eEdgeType edgeType);

	static std::map <std::string, TileEdgeDefinition*>	s_definitions;
	static TileEdgeDefinition* GetEdgeDefinition(std::string name);
};