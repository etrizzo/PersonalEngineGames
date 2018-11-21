#pragma once;
#include "Game/StoryData.hpp"

class Character;


#define START_NODE_POSITION (Vector2(.1f, .5f))
#define END_NODE_POSITION (Vector2(1.66f, .5f))
#define NODE_SIZE (.04f)
#define NODE_FONT_SIZE (.0083f)
#define EDGE_FONT_SIZE (.01f)
#define EDGE_ARROW_SIZE (.02f)
#define PULL_WEIGHT (0.1f)
#define PUSH_WEIGHT (.99f)
#define MIN_NODE_DISTANCE (.175f)
#define MAX_NODE_DISTANCE (.4f)
#define NUM_NODE_ITERATIONS (2000)
#define MIN_DISTANCE_TO_MOVE (.06f)
#define REROLL_REPEAT_PLOT_NODE_CHANCE (.9f)
#define REROLL_REPEAT_DETAIL_NODE_CHANCE (.85f)
#define MAX_REPEAT_REROLLS (20)

//comparison for 
typedef StoryState* (*StoryHeuristicCB) (StoryEdge* edge);

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
	void RenderDebugInfo() const;
	void UpdateNodePositions();
	void RunNodeAdjustments();
	void RenderPath() const;
	Disc2 GetNodeBounds(StoryNode* node) const;
	OBB2 GetEdgeBounds(StoryEdge* edge) const;

	/*
	=====================
	Input
	=====================
	*/

	void HandleInput();

	/*
	=====================
	Generation Functions
	=====================
	*/
	void RunGeneration(int numPlotNodes = 3, int desiredSize = 10);

	void GenerateSkeleton(int numPlotNodes);
	void AddPlotNodes(int numPlotNodes);
	StoryNode* AddSinglePlotNode();
	void AddOutcomeNodesToPlotNode(StoryNode* plotNode);
	void AddDetailNodesToDesiredSize(int desiredSize = 10);
	void GenerateStartAndEnd();
	bool TryToAddDetailNodeAtEdge(StoryEdge* edge, int maxTries = 10);
	bool AddPlotNode(StoryNode* newPlotNode);
	bool AddDetailNode(StoryNode* newDetailNode);
	bool AddDetailNode(StoryDataDefinition* dataDefinition, StoryEdge* edgeToAddAt, std::vector<Character*> charactersForNode);
	bool AddBranchAroundNode(StoryNode* existingNode, StoryNode* nodeToAdd, bool branchToFutureNodesIfNecessary);

	//by default, adds 1/4 * (numNodes) branches
	void IdentifyBranchesAndAdd(int numBranchesToAdd = -1);
	bool AttemptToAddBranchAfterFail(StoryNode* startingNode, StoryEdge* failedEdge, StoryNode* newNode);
	StoryEdge* CheckReachableNodesForBranch(std::vector<StoryNode*> reachableNodes, StoryNode* startingNode);


	//adds a new node on the edge between two existing nodes (insert between nodes)
	void AddNodeAtEdge(StoryNode* newNode, StoryEdge* existingEdge);

	void SetBranchChance(float branchChance = DEFAULT_BRANCH_CHANCE_ON_FAIL);

	void FindPath( StoryHeuristicCB heuristic );
	void PrintPath() ;

	void ClearSavedState();

	/*
	=========
	Accessors
	=========
	*/
	Character* GetCharacter(unsigned int index) const;
	Character* GetCharacterByName(std::string name) const;
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
	std::vector<Character*> GetCharactersForNode(StoryDataDefinition* nodeDefinition, StoryEdge* atEdge);

protected:
	StoryNode* m_startNode							= nullptr;
	StoryNode* m_endNode							= nullptr;

	DirectedGraph<StoryData*, StoryState*> m_graph	= DirectedGraph<StoryData*, StoryState*>();
	std::vector<Character*> m_characters			= std::vector<Character*>();

	std::vector<StoryNode*> m_pathFound				= std::vector<StoryNode*>();
	std::string m_pathString						= "";
	
	std::vector<StoryEdge*> m_edgesToVoid = std::vector<StoryEdge*>();


	//float m_nodeSize							= .05f;
	StoryNode* m_hoveredNode					= nullptr;
	StoryEdge* m_hoveredEdge					= nullptr;
	StoryEdge* m_selectedEdge					= nullptr;

	float m_branchChance						= DEFAULT_BRANCH_CHANCE_ON_FAIL;
	
	
	Vector2 CalculateNodeForces(StoryNode* node) const;
	Vector2 CalculateNodePull(StoryNode* node) const;
	Vector2 CalculateNodePush(StoryNode* node) const;
	void RenderNode(StoryNode* node, Vector2 position, RGBA color = RGBA::BLANCHEDALMOND) const;
	void RenderEdge(StoryEdge* edge, RGBA color = RGBA::WHITE) const;


	std::vector<Character*> ClearCharacterArray(int numCharacters);
	bool AreAllCharactersSet(const std::vector<Character*>& chars) const;

	/*
	==============
	Visual Tweaks
	==============
	*/
	RGBA m_edgeDefaultColor = RGBA::DARKGRAY;
	RGBA m_edgeHoverColor = RGBA::CYAN;
	RGBA m_edgeSelectColor = RGBA::BLUE;

	RGBA m_nodeDefaultColor = RGBA::BLANCHEDALMOND;
	RGBA m_nodeHoverColor = RGBA::MAGENTA;

	RGBA m_nodeTextColor = RGBA::RED;
	RGBA m_edgeTextColor = RGBA::YELLOW;

	RGBA m_pathColor = RGBA::GREEN;

	std::vector<StoryDataDefinition*> m_usedPlotNodes;
	std::vector<StoryDataDefinition*> m_usedDetailNodes;
public:
	static std::vector<StoryDataDefinition*> s_plotNodes;
	static std::vector<StoryDataDefinition*> s_detailNodes;
	static StoryDataDefinition* GetRandomPlotNode();
	static StoryDataDefinition* GetRandomDetailNode();

	static StoryDataDefinition* GetDetailNodeWithWeights(StoryState* edge, float minFitness = 2.f);
	static float CalculateEdgeFitnessForData(StoryState* edge, StoryDataDefinition* data);
};


StoryState* ShortestPathHeuristic(StoryEdge* edge);

StoryState* RandomPathHeuristic(StoryEdge* edge);
StoryState* CalculateChanceHeuristic(StoryEdge* edge);