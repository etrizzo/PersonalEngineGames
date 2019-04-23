#pragma once;
#include "Game/DataSet.hpp"

class Character;


#define START_NODE_POSITION (Vector2(.1f, .5f))
#define END_NODE_POSITION (Vector2(1.66f, .5f))
#define NODE_SIZE (.04f)
#define NODE_FONT_SIZE (.0083f)
#define EDGE_FONT_SIZE (.01f)
#define EDGE_ARROW_SIZE (.02f)
#define PULL_WEIGHT (0.2f)
#define PUSH_WEIGHT (.9f)
#define MIN_NODE_DISTANCE (.175f)
#define MAX_NODE_DISTANCE (.3f)
#define NUM_NODE_ITERATIONS (2000)
#define MIN_DISTANCE_TO_MOVE (.06f)
#define REROLL_REPEAT_PLOT_NODE_CHANCE (.95f)
#define REROLL_REPEAT_DETAIL_NODE_CHANCE (.95f)
#define MAX_REPEAT_REROLLS (30)

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
	void LoadDataSet(std::string setName);
	void SelectCharactersForGraph(int numCharacters = 0);

	
	/*
	=====================
	Render Functions
	=====================
	*/

	void RenderGraph(const AABB2& screenBounds) const;
	void RenderDebugInfo(const AABB2& screenBounds) const;
	void UpdateNodePositions();
	void RunNodeAdjustments();
	void RenderPath() const;
	void RenderAnalysis(const AABB2& screenBounds) const;
	Disc2 GetNodeBounds(StoryNode* node) const;
	OBB2 GetEdgeBounds(StoryEdge* edge) const;

	/*
	=====================
	Input
	=====================
	*/

	void HandleInput(const AABB2& bounds);

	/*
	=====================
	Generation Functions
	=====================
	*/

	void RunGenerationFinal();
	
	bool GenerateInitialNodesForGraph();
	void AddASingleSetOfNodes();

	StoryNode* TryToAddEventNodeAtEdge(StoryDataDefinition* definitionToPlace, StoryEdge* edge);
	bool TryToAddOutcomesAtEventNode(StoryDataDefinition* definitionToPlace, StoryNode* eventnode);

	//looks at the nodes we have right now and compares to the expected number of nodes in the dataset
	bool AreAllActsFinished() const;

	void RunGenerationPairs(int numPairs);
	void RunGenerationPlotAndDetail(int numPlotNodes = 3, int desiredSize = 10);
	void RunGenerationByActs(int numPairsToAdd);		//targets edges with the largest act range
	
	bool HavePlacedAllEndings() const;

	void GenerateSkeleton(int numPlotNodes);
	//void AddPlotNodes(int numPlotNodes);
	StoryNode* AddSingleEventNode();
	StoryNode* AddEventNodeAtEdge(StoryEdge* edge);
	void AddOutcomeNodesToEventNode(StoryNode* plotNode);
	void AddDetailNodesToDesiredSize(int desiredSize = 10);
	void GenerateStartAndEnd();
	bool TryToAddOutcomeNodeAtEdge(StoryEdge* edge, int maxTries = 10);
	bool FindEdgeForNewEventNodeAndAdd(StoryNode* newPlotNode);
	//bool AddOutcomeNode(StoryNode* newDetailNode);
	bool CreateAndAddOutcomeNodeAtEdge(StoryDataDefinition* dataDefinition, StoryEdge* edgeToAddAt, std::vector<Character*> charactersForNode, bool onlyAddEndings = false);
	StoryNode* CreateAndAddEventNodeAtEdge(StoryDataDefinition* dataDefinition, StoryEdge* edgeToAddAt, std::vector<Character*> charactersForNode);
	bool AddBranchAroundNode(StoryNode* existingNode, StoryNode* nodeToAdd, bool branchToFutureNodesIfNecessary);

	std::vector<StoryEdge*> GetPrioritizedEdges();

	//looks backwards from the end node and adds nodes to edges that don't have an end.
	// returns true if successful, returns false if the graph was unsalvagable (indicating that whoever called this should regenerate).
	bool AddEndingsToGraph(int maxTries = 20);

	bool AddEndingsToActBoundaryEdge(StoryNode* nextActStartingNode, int maxTries = 20);
	void RemoveBranchesWithNoEnding(StoryNode* nextActStartingNode);
	//if no paths had an ending and the graph is now just an end node, this will return true
	bool IsGraphInvalid();

	StoryNode* TryToAddEndNodeAtEdge(StoryEdge* edgeState, bool isOutcome);

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
	Analaysis
	=========
	*/
	void AnalyzeGraph();
	void IdentifyBranches();
	void IdentifyEndings();
	void IdentifyBottlenecks();
	void CleanAnalysisData();
	void DetermineGraphType();


	/*
	=========
	Accessors
	=========
	*/
	Character* GetCharacter(unsigned int index) const;
	Character* GetCharacterByName(std::string name) const;
	unsigned int GetNumCharacters() const;
	StoryNode* GetStart() const			{ return m_startNode; }
	StoryNode* GetEnd() const			{ return m_endNode; }


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

	void Reset();

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
	bool StoryRequirementsMet(StoryDataDefinition* node, StoryEdge* atEdge);
	bool TryToSetCharactersForNode(StoryNode* node, StoryEdge* atEdge);
	std::vector<Character*> GetCharactersForNode(StoryDataDefinition* nodeDefinition, StoryEdge* atEdge);

	std::vector<Character*> m_characters			= std::vector<Character*>();

	void RenderEdge(StoryEdge* edge, RGBA color = RGBA::WHITE) const;





	DirectedGraph<StoryData*, StoryState*> m_graph = DirectedGraph<StoryData*, StoryState*>();

protected:
	DataSet* m_dataSet		= nullptr;

	StoryNode* m_startNode							= nullptr;
	StoryNode* m_endNode							= nullptr;

	//returns a list of all edges whos range >= 1
	std::vector<StoryNode*> GetActStartingNodes() const;

	//finds a good spot for a new pair of nodes.
	StoryEdge* GetEdgeForNewEventNode(StoryNode* newNode,  float minFitness = 2.f)	const;

	//find the edge in the graph right now which has the largest act range
	StoryEdge* GetEdgeWithLargestActRange(bool lookingForEndings) const;

	
	
	bool m_analysisHasRun = false;
	std::vector<StoryNode*> m_branchingNodes = std::vector<StoryNode*>();
	std::vector<StoryNode*> m_mergingNodes	 = std::vector<StoryNode*>();
	std::vector<StoryNode*> m_endingNodes	 = std::vector<StoryNode*>();
	std::string m_graphType = "Undetermined";

	std::vector<StoryNode*> m_pathFound				= std::vector<StoryNode*>();
	std::string m_pathString						= "";
	

	//float m_nodeSize							= .05f;
	StoryNode* m_hoveredNode					= nullptr;
	StoryEdge* m_hoveredEdge					= nullptr;
	StoryEdge* m_selectedEdge					= nullptr;

	float m_branchChance						= DEFAULT_BRANCH_CHANCE_ON_FAIL;
	
	
	Vector2 CalculateNodeForces(StoryNode* node) const;
	Vector2 CalculateNodePull(StoryNode* node) const;
	Vector2 CalculateNodePush(StoryNode* node) const;
	void RenderNode(StoryNode* node, Vector2 position, RGBA color = RGBA::BLANCHEDALMOND) const;
	
	void UpdateDepths();
	void SetNodePositionsByDepth();

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

	//std::vector<StoryDataDefinition*> m_usedPlotNodes;
	//std::vector<StoryDataDefinition*> m_usedDetailNodes;
public:
	
};


StoryState* ShortestPathHeuristic(StoryEdge* edge);

StoryState* RandomPathHeuristic(StoryEdge* edge);
StoryState* CalculateChanceHeuristic(StoryEdge* edge);


bool CompareEdgesByPriority(StoryEdge* a, StoryEdge* b);

bool IsEventToOutcomeEdge(StoryEdge* edge);