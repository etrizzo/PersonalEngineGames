#pragma once
#include "Game/GameCommon.hpp"


struct Act
{
	Act(tinyxml2::XMLElement* actElement);
	std::string m_name		= "";
	int			m_number	= 0;
};



//
////you're a class act AAAAY LMAO jk get your shit together
//class Act
//{
//public:
//	Act();
//
//	//Adds a start or end node to connect to this act and updates them as well
//	bool ConnectToAct(Act* newAct);
//
//	void GenerateEventAndOutcomePair();
//	
//	std::vector<StoryNode*> m_nodes;		//these are references to nodes/edges that exist in the storygraph
//	std::vector<StoryEdge*> m_edges;
//
//	//collection of all start and end points in the act.
//	std::vector<StoryNode*> m_startNodes	= std::vector<StoryNode*>();		
//	std::vector<StoryNode*> m_endNodes		= std::vector<StoryNode*>();
//};