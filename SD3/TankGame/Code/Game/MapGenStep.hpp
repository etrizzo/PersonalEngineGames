//#pragma once
//#include "Game/Map.hpp"
//
//class MapGenStep
//{
//public:
//	MapGenStep( const tinyxml2::XMLElement& genStepXmlElement );
//	~MapGenStep();
//	void RunIterations(Map& map);
//	virtual void Run( Map& map ) = 0; // "pure virtual", MUST be overridden by subclasses
//
//public:
//	static MapGenStep* CreateMapGenStep( const tinyxml2::XMLElement& genStepXmlElement );
//
//protected:
//	std::string		m_name;
//	IntRange m_iterations = IntRange(1);
//	float m_chanceToRun = 1.f;
//};
//
//
//class MapGenStep_FillAndEdge: MapGenStep{
//
//public:
//	MapGenStep_FillAndEdge( const tinyxml2::XMLElement& generationStepElement);
//
//	void Run( Map& map );
//
//	TileDefinition*		m_fillTileDef = nullptr;
//	TileDefinition*		m_edgeTileDef = nullptr;
//	float				m_edgeThickness = 1;
//};