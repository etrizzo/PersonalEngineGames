//#pragma once
//#include "Game/GameCommon.hpp"
//
//class MapGenStep;
//class TileDefinition;
//
//class MapDefinition{
//public:
//	explicit MapDefinition(tinyxml2::XMLElement* mapDefElement);
//
//	std::string m_name;
//	TileDefinition* m_defaultTile;
//	IntRange m_width = IntRange(0);
//	IntRange m_height = IntRange(0);
//	
//	std::vector<MapGenStep*> m_generationSteps;
//
//	static std::map< std::string, MapDefinition* >		s_definitions;
//
//
//private:
//	void MakeGenerationSteps(tinyxml2::XMLElement* mapGenElement);
//};