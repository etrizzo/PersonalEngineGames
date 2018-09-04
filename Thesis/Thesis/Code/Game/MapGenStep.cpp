//#include "MapGenStep.hpp"
//
//MapGenStep::MapGenStep(const tinyxml2::XMLElement & genStepXmlElement)
//{
//	m_name = genStepXmlElement.Name();
//	m_iterations = ParseXmlAttribute(genStepXmlElement, "iterations",m_iterations);
//	m_chanceToRun = ParseXmlAttribute(genStepXmlElement,  "chanceToRun", 1.f);
//}
//
//MapGenStep::~MapGenStep()
//{
//	
//}
//
//MapGenStep * MapGenStep::CreateMapGenStep(const tinyxml2::XMLElement & genStepXmlElement)
//{
//	std::string genName = genStepXmlElement.Name();
//	MapGenStep* newMapGenStep = nullptr;
//	if (genName == "FillAndEdge"){
//		newMapGenStep =(MapGenStep*) new MapGenStep_FillAndEdge(genStepXmlElement);
//	}
//
//
//	return newMapGenStep;
//}
//
//void MapGenStep::RunIterations(Map & map)
//{
//	if (CheckRandomChance(m_chanceToRun)){
//		int numIterations = m_iterations.GetRandomInRange();
//		for (int i = 0; i < numIterations; i++){
//			Run(map);
//		}
//	}
//}
//
//MapGenStep_FillAndEdge::MapGenStep_FillAndEdge( const tinyxml2::XMLElement& generationStepElement )
//	: MapGenStep( generationStepElement )
//{
//	m_fillTileDef = ParseXmlAttribute( generationStepElement, "fillTile", m_fillTileDef );
//	m_edgeTileDef = ParseXmlAttribute( generationStepElement, "edgeTile", m_edgeTileDef );
//	m_edgeThickness = ParseXmlAttribute( generationStepElement, "edgeThickness", m_edgeThickness );
//}
//
//void MapGenStep_FillAndEdge::Run(Map & map)
//{;
//	for(int tileIndex = 0; tileIndex < (int) map.m_tiles.size(); tileIndex++){
//		IntVector2 tileCoords = GetCoordinatesFromIndex(tileIndex, map.GetWidth());
//		if (tileCoords.x == 0 || tileCoords.y == 0 || tileCoords.x == map.GetWidth() || tileCoords.y == map.GetHeight()){
//			map.m_tiles[tileIndex].SetType(m_edgeTileDef);
//		} else {
//			map.m_tiles[tileIndex].SetType((m_fillTileDef));
//		}
//	}
//}
//
