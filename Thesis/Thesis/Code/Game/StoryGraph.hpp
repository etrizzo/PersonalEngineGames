#pragma once;
#include "Engine/DataTypes/DirectedGraph.hpp"
#include "Game/StoryData.hpp"
#include "Game/StoryStructure.hpp"

class Character;

typedef Node<StoryData> StoryNode ;
typedef DirectedEdge<StoryData> StoryEdge;

#define NODE_SIZE (.05f)
#define PULL_WEIGHT (.51f)
#define PUSH_WEIGHT (.89f)
#define MIN_NODE_DISTANCE (.2f)
#define MAX_NODE_DISTANCE (.35f)
#define NUM_NODE_ITERATIONS (2000)

//comparison for 
typedef bool (*StoryHeuristicCB) (StoryNode* a, StoryNode* b, StoryStructure* currentStructure);

// wrapper for DirectedGraph which stores all project-specific information
// such as characters, story structure, etc.
// This will be where the bulk of generation happens.
class StoryGraph{
public:
	StoryGraph();
	
	/*
	=====================
	Render Functions
	=====================
	*/

	void RenderGraph() const;
	void UpdateNodePositions();
	void RunNodeAdjustments();

	/*
	=====================
	Generation Functions
	=====================
	*/
	std::vector<StoryNode*> FindPath( StoryHeuristicCB heuristic );

	/*
	=====================================
	Wrappers for DirectedGraph functions
	=====================================
	*/
	//Modifiers
	StoryNode* AddNode(StoryData data);
	StoryNode* AddNode(StoryNode* newNode);
	StoryEdge* AddEdge(StoryNode* start, StoryNode* end, float cost = 0.f);
	void RemoveAndDeleteNode(StoryData data);
	void RemoveAndDeleteNode(StoryNode* node);
	void RemoveAndDeleteEdge(StoryNode* start, StoryNode* end);
	void RemoveAndDeleteEdge(StoryEdge* edge);

	StoryNode* AddStart(StoryData data);
	StoryNode* AddStart(StoryNode* startNode);
	StoryNode* AddEnd(StoryData data);
	StoryNode* AddEnd(StoryNode* endNode);

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
	StoryStructure m_targetStructure		= StoryStructure();

	//float m_nodeSize						= .05f;
	
	
	Vector2 CalculateNodeForces(StoryNode* node) const;
	Vector2 CalculateNodePosition(StoryNode* node);
	Vector2 CalculateNodePull(StoryNode* node) const;
	Vector2 CalculateNodePush(StoryNode* node) const;
	void RenderNode(StoryNode* node, Vector2 position) const;
	void RenderEdge(StoryEdge* edge) const;


};