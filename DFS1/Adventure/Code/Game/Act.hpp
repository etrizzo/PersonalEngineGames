#pragma once
#include "Game/GameCommon.hpp"

//you're a class act AAAAY LMAO jk get your shit together
class Act
{
public:
	Act();


	bool ConnectToAct(Act* newAct);
	
	std::vector<StoryNode*> m_nodes;		//these are references to nodes/edges that exist in the storygraph
	std::vector<StoryEdge*> m_edges;

	//collection of all start and end points in the act.
	std::vector<StoryNode*> m_startNodes	= std::vector<StoryNode*>();		
	std::vector<StoryNode*> m_endNodes		= std::vector<StoryNode*>();
};