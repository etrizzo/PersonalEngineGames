#include "Engine/DataTypes/DirectedGraph.hpp"
#include "Game/StoryData.hpp"
#include "Game/StoryStructure.hpp"

class Character;

typedef Node<StoryData> StoryNode ;
typedef DirectedEdge<StoryData> StoryEdge;

// wrapper for DirectedGraph which stores all project-specific information
// such as characters, story structure, etc.
// This will be where the bulk of generation happens.
class StoryGraph{
	StoryGraph();
	
	/*
	=====================
	Render Functions
	=====================
	*/
	void RenderGraph() const;

	/*
	=====================
	Generation Functions
	=====================
	*/


	/*
	=====================================
	Wrappers for DirectedGraph functions
	=====================================
	*/
	//Modifiers
	StoryNode* AddNode(StoryData data);
	StoryNode* AddNode(StoryNode* newNode);
	StoryEdge* AddEdge(StoryNode* start, StoryNode* end);
	void RemoveAndDeleteNode(StoryData data);
	void RemoveAndDeleteNode(StoryNode* node);
	void RemoveAndDeleteEdge(StoryNode* start, StoryNode* end);
	void RemoveAndDeleteEdge(StoryEdge* edge);

	void Clear();

	//Getters
	StoryEdge* GetEdge(StoryNode* start, StoryNode* end)	const;
	StoryNode* GetNode(std::string name)					const;	//How to uniquely identify nodes?	StoryNode* GetNode(StoryData data);	
	unsigned int GetNumNodes() const;
	unsigned int GetNumEdges() const;
	std::string ToString() const;

	//Checks
	bool ContainsNode(StoryNode* node)					const;
	bool ContainsNode(StoryData data)					const;
	bool ContainsEdge(StoryEdge* edge)					const;
	bool ContainsEdge(StoryNode* start, StoryNode* end)	const;

	
	

protected:
	StoryNode* m_startNode					= nullptr;
	StoryNode* m_endNode					= nullptr;;
	DirectedGraph<StoryData> m_graph		= DirectedGraph<StoryData>();
	std::vector<Character*> m_characters	= std::vector<Character*>();
	StoryStructure m_structure				= StoryStructure();

};