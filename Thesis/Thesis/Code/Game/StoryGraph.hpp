#pragma once;
#include "Game/StoryData.hpp"
#include "Game/StoryStructure.hpp"

class Character;


#define START_NODE_POSITION (Vector2(.15f, .5f))
#define END_NODE_POSITION (Vector2(1.25f, .5f))
#define NODE_SIZE (.04f)
#define NODE_FONT_SIZE (.008f)
#define PULL_WEIGHT (.51f)
#define PUSH_WEIGHT (.89f)
#define MIN_NODE_DISTANCE (.15f)
#define MAX_NODE_DISTANCE (.3f)
#define NUM_NODE_ITERATIONS (2000)

//comparison for 
typedef StoryState* (*StoryHeuristicCB) (StoryEdge* edge, StoryStructure* currentStructure);

// wrapper for DirectedGraph which stores all project-specific information
// such as characters, story structure, etc.
// This will be where the bulk of generation happens.
class StoryGraph{
public:
	StoryGraph();

	/*
	===========================
	Initialization Functions
	===========================
	*/
	void ReadPlotNodesFromXML(std::string filePath);
	void ReadDetailNodesFromXML(std::string filePath);
	void ReadCharactersFromXML(std::string filePath);

	
	/*
	=====================
	Render Functions
	=====================
	*/

	void RenderGraph() const;
	void UpdateNodePositions();
	void RunNodeAdjustments();
	void RenderPath() const;

	/*
	=====================
	Generation Functions
	=====================
	*/
	void RunGeneration(int numPlotNodes = 3, int desiredSize = 10);

	void GenerateSkeleton(int numPlotNodes);
	void GenerateStartAndEnd();
	bool TryToAddDetailNode();
	bool AddPlotNode(StoryNode* newPlotNode);
	bool AddDetailNode(StoryNode* newDetailNode);

	//by default, adds 1/4 * (numNodes) branches
	void IdentifyBranchesAndAdd(int numBranchesToAdd = -1);

	//adds a new node on the edge between two existing nodes (insert between nodes)
	void AddNodeAtEdge(StoryNode* newNode, StoryEdge* existingEdge);


	void FindPath( StoryHeuristicCB heuristic );

	/*
	=========
	Accessors
	=========
	*/
	Character* GetCharacter(unsigned int index) const;
	unsigned int GetNumCharacters() const;


	/*
	=====================================
	Wrappers for DirectedGraph functions
	=====================================
	*/
	//Modifiers
	StoryNode* AddNode(StoryData* data);
	StoryNode* AddNode(StoryNode* newNode);
	StoryEdge* AddEdge(StoryNode* start, StoryNode* end);
	StoryEdge* AddEdge(StoryNode* start, StoryNode* end, StoryState* cost);
	void RemoveAndDeleteNode(StoryData* data);
	void RemoveAndDeleteNode(StoryNode* node);
	void RemoveAndDeleteEdge(StoryNode* start, StoryNode* end);
	void RemoveAndDeleteEdge(StoryEdge* edge);

	StoryNode* AddStart(StoryData* data);
	StoryNode* AddStart(StoryNode* startNode);
	StoryNode* AddEnd(StoryData* data);
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
	bool ContainsNode(StoryData* data)					const;
	bool ContainsEdge(StoryEdge* edge)					const;
	bool ContainsEdge(StoryNode* start, StoryNode* end)	const;

	bool NodeRequirementsAreMet(StoryNode* node, StoryEdge* atEdge);
	bool StoryRequirementsMet(StoryNode* node, StoryEdge* atEdge);
	bool TryToSetCharactersForNode(StoryNode* node, StoryEdge* atEdge);

protected:
	StoryNode* m_startNode							= nullptr;
	StoryNode* m_endNode							= nullptr;;
	DirectedGraph<StoryData*, StoryState*> m_graph	= DirectedGraph<StoryData*, StoryState*>();
	std::vector<Character*> m_characters			= std::vector<Character*>();
	StoryStructure m_targetStructure				= StoryStructure();

	std::vector<StoryNode*> m_pathFound				= std::vector<StoryNode*>();

	//float m_nodeSize						= .05f;
	
	
	Vector2 CalculateNodeForces(StoryNode* node) const;
	Vector2 CalculateNodePosition(StoryNode* node);
	Vector2 CalculateNodePull(StoryNode* node) const;
	Vector2 CalculateNodePush(StoryNode* node) const;
	void RenderNode(StoryNode* node, Vector2 position, RGBA color = RGBA::BLANCHEDALMOND) const;
	void RenderEdge(StoryEdge* edge, RGBA color = RGBA::WHITE) const;

public:
	static std::vector<StoryNode*> s_plotNodes;
	static std::vector<StoryNode*> s_detailNodes;
	static StoryNode* GetRandomPlotNode();
	static StoryNode* GetRandomDetailNode();


};


StoryState* ShortestPathHeuristic(StoryEdge* edge,  StoryStructure* currentStructure);