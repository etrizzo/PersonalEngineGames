#include "StoryGraph.hpp"
#include "Game/Game.hpp"
#include "Game/Character.hpp"
#include "Game/StoryState.hpp"
#include "Game/CharacterRequirementSet.hpp"

std::vector<StoryNode*> StoryGraph::s_plotNodes = std::vector<StoryNode*>();
std::vector<StoryNode*> StoryGraph::s_detailNodes = std::vector<StoryNode*>();

StoryGraph::StoryGraph()
{

}

void StoryGraph::ReadPlotNodesFromXML(std::string filePath)
{
	tinyxml2::XMLDocument nodeDoc;
//	std::string filePath = "Data/Data/" + fileName;
	nodeDoc.LoadFile(filePath.c_str());
	for (tinyxml2::XMLElement* nodeElement = nodeDoc.FirstChildElement("PlotGrammar"); nodeElement != NULL; nodeElement = nodeElement->NextSiblingElement("PlotGrammar")){
		StoryData* data = new StoryData(nodeElement, PLOT_NODE);
		s_plotNodes.push_back(new StoryNode(data));
	}
	
}

void StoryGraph::ReadDetailNodesFromXML(std::string filePath)
{
	tinyxml2::XMLDocument nodeDoc;
	//	std::string filePath = "Data/Data/" + fileName;
	nodeDoc.LoadFile(filePath.c_str());
	for (tinyxml2::XMLElement* nodeElement = nodeDoc.FirstChildElement("DetailGrammar"); nodeElement != NULL; nodeElement = nodeElement->NextSiblingElement("DetailGrammar")){
		StoryData* data = new StoryData(nodeElement, DETAIL_NODE);
		s_detailNodes.push_back(new StoryNode(data));
	}
}

void StoryGraph::ReadCharactersFromXML(std::string filePath)
{
	tinyxml2::XMLDocument charDoc;
	//	std::string filePath = "Data/Data/" + fileName;
	charDoc.LoadFile(filePath.c_str());
	for (tinyxml2::XMLElement* charElement = charDoc.FirstChildElement("Character"); charElement != NULL; charElement = charElement->NextSiblingElement("Character")){
		Character* newChar = new Character();
		newChar->InitFromXML(charElement);
		m_characters.push_back(newChar);
	}
}

void StoryGraph::UpdateNodePositions()
{
	for(int i = 0; i < m_graph.m_nodes.size(); i++){
		StoryNode* node = m_graph.m_nodes[i];
		if (node == m_startNode){
			node->m_data->SetPosition(START_NODE_POSITION);
		}
		else if (node == m_endNode){
			node->m_data->SetPosition(END_NODE_POSITION);
		} else {
			Vector2 nodeForce = CalculateNodeForces(node);
			Vector2 newNodePos = (nodeForce * .016) + node->m_data->GetPosition();
			node->m_data->SetPosition(newNodePos);
		}
	}
}

void StoryGraph::RunNodeAdjustments()
{
	for (int i = 0; i < NUM_NODE_ITERATIONS; i++){
		UpdateNodePositions();
	}
}



void StoryGraph::RunGeneration(int numPlotNodes, int desiredSize)
{
	GenerateSkeleton(numPlotNodes);
	bool canAdd = true;
	while(m_graph.GetNumNodes() < desiredSize && canAdd){
		canAdd = TryToAddDetailNode();
	}
}

void StoryGraph::GenerateSkeleton(int numPlotNodes)
{
	TODO("Clean up cloning of nodes.");
	GenerateStartAndEnd();
	StoryNode* addNode = nullptr;
	StoryData* newData = nullptr;
	while(GetNumNodes() < numPlotNodes){
		//clone data
		newData = new StoryData(StoryGraph::GetRandomPlotNode()->m_data);
		addNode = new StoryNode(newData);
		//try to add the cloned data, if it doesn't work, clean up
		if (!AddPlotNode(addNode)){
			delete newData;
			delete addNode;
			newData = nullptr;
			addNode = nullptr;
		}
	}

	IdentifyBranchesAndAdd(2);
}

void StoryGraph::GenerateStartAndEnd()
{
	TODO("figure out how this should work.");
	AddStart(new StoryNode(new StoryData("START", 1.f)));
	AddEnd(new StoryNode(new StoryData("END", 1.f)));
	AddEdge(m_startNode, m_endNode, new StoryState(1.f, m_characters.size()));
}

bool StoryGraph::TryToAddDetailNode()
{
	StoryNode* addNode = nullptr;
	StoryData* newData = nullptr;
	int tries = 0;
	bool added = false;
	while(tries < 100 && !added){
		//clone data
		newData = new StoryData(StoryGraph::GetRandomDetailNode()->m_data);
		addNode = new StoryNode(newData);
		//try to add the cloned data, if it doesn't work, clean up
		added = AddDetailNode(addNode);
		if (!added){
			delete newData;
			delete addNode;
			newData = nullptr;
			addNode = nullptr;
		}
		tries++;
	}
	return added;	
}

bool StoryGraph::AddPlotNode(StoryNode* newPlotNode)
{
	//walk along story until you can place the node
	std::queue<StoryEdge*> openEdges;
	openEdges.push(m_startNode->m_outboundEdges[0]);
	StoryNode* nodeToAddAfter; 
	StoryEdge* edgeToAddAt = nullptr;
	bool foundSpot = false;
	while (!openEdges.empty()){
		edgeToAddAt = openEdges.back();
		openEdges.pop();
		//if the story meets the requirements of the new nodes at that edge, continue.
		//if not, add the end of the edge to the open list.
		if (!NodeRequirementsAreMet(newPlotNode, edgeToAddAt)){
			for (StoryEdge* edge : edgeToAddAt->GetEnd()->m_outboundEdges){
				openEdges.push(edge);
			}
		} else {
			foundSpot = true;
			break;
		}

	}

	//add the plot node at the found node
	if (foundSpot){
		AddNodeAtEdge(newPlotNode, edgeToAddAt);
	}

	return foundSpot;
}

bool StoryGraph::AddDetailNode(StoryNode * newDetailNode)
{
	//walk along story until you can place the node
	std::queue<StoryEdge*> openEdges;
	openEdges.push(m_startNode->m_outboundEdges[0]);
	StoryNode* nodeToAddAfter; 
	StoryEdge* edgeToAddAt = nullptr;
	bool foundSpot = false;
	while (!openEdges.empty()){
		edgeToAddAt = openEdges.back();
		openEdges.pop();
		//if the story meets the requirements of the new nodes at that edge, continue.
		//if not, add the end of the edge to the open list.
		if (!NodeRequirementsAreMet(newDetailNode, edgeToAddAt)){
			for (StoryEdge* edge : edgeToAddAt->GetEnd()->m_outboundEdges){
				openEdges.push(edge);
			}
		} else {
			foundSpot = true;
			break;
		}
	}

	//add the plot node at the found node
	if (foundSpot){
		AddNodeAtEdge(newDetailNode, edgeToAddAt);
	}

	return foundSpot;
}

void StoryGraph::IdentifyBranchesAndAdd(int numBranchesToAdd)
{
	if (numBranchesToAdd < 0){
		numBranchesToAdd = (int) GetNumNodes() * .25;
	}

	int branchesAdded = 0;
	int tries = 0;
	while (branchesAdded < numBranchesToAdd && tries < 100){
		//make a queue of nodes to check, starting at random node
		std::queue<StoryNode*> checks = std::queue<StoryNode*>();
		StoryNode* randomNode = m_graph.m_nodes[GetRandomIntLessThan(GetNumNodes())];
		checks.push(randomNode);
		
		StoryNode* currentNode = checks.back();
		bool added = false;
		while (!added && currentNode != nullptr && currentNode != m_endNode && !checks.empty()){
			currentNode = checks.back();
			checks.pop();
			//identify all "future" nodes (any node that can be reached by this node)
					//note: right now, just one layer deep
			std::vector<StoryNode*> reachableNodes = std::vector<StoryNode*>();
			for(StoryEdge* outbound : currentNode->m_outboundEdges){
				checks.push(outbound->GetEnd());
				for(StoryEdge* secondLayer : outbound->GetEnd()->m_outboundEdges){
					reachableNodes.push_back(secondLayer->GetEnd());
				}
			}
			//for each future node, see if an outgoing edge would meet the story state requirements of that node.
			for (StoryNode* reachable : reachableNodes){
				//if so, add an edge and continue (tier 1)
				if (true){
					StoryState* newState = new StoryState(GetRandomFloatInRange(0.f, 5.f), GetNumCharacters());
					m_graph.AddEdge(currentNode, reachable, newState);
					branchesAdded++;
					added = true;
					break;
				}
			}
			
		}
		tries++;

	}
}

void StoryGraph::AddNodeAtEdge(StoryNode * newNode, StoryEdge * existingEdge)
{
	m_graph.RemoveEdge(existingEdge);
	//not sure what to do with cost...
	StoryState* newCost = new StoryState(GetRandomFloatInRange(0.f, 5.f), m_characters.size());
	m_graph.AddEdge(existingEdge->GetStart(), newNode, newCost);
	m_graph.AddEdge(newNode, existingEdge->GetEnd(), newCost);
}

void StoryGraph::FindPath(StoryHeuristicCB heuristic)
{
	for (StoryNode* node : m_graph.m_nodes){
		node->SetShortestDistance(9999.f);
	}
	std::vector<StoryNode*> openNodes = std::vector<StoryNode*>();
	openNodes.push_back(m_startNode);
	m_startNode->SetShortestDistance(0.f);

	//assign shortest distance to nodes, with heuristic
	StoryNode* currentNode;
	while (!openNodes.empty()){
		currentNode = openNodes.back();
		openNodes.pop_back();

		//break if the end node is found
		if (currentNode == m_endNode){
			//return the path
			break;
		} else {
			//otherwise, get all out-bound neighbors.
			//std::vector<StoryNode*> outboundNeighbors;
			float cost = currentNode->GetShortestDistance();
			for (StoryEdge* edge : currentNode->m_outboundEdges){
				//outboundNeighbors.push_back(edge->GetEnd());
				StoryNode* neighbor = edge->GetEnd();
				//if the new path is faster than the old path, update cost
				StoryState* edgeState = heuristic(edge, &m_targetStructure);
				float newCost = cost + (float) (edgeState->GetCost());
				if (neighbor->GetShortestDistance() > newCost){
					neighbor->SetShortestDistance(newCost);
					// if the neighbor isn't in your list of open nodes, add it
					bool contains = false;
					for (StoryNode* node : openNodes){
						if (node == neighbor){
							contains = true;
						}
					}
					if (!contains){
						openNodes.insert(openNodes.begin(), neighbor);
					}
				}
			}
		}
	}

	////traverse nodes in order 
	m_pathFound = std::vector<StoryNode*>();
	currentNode = m_startNode;
	while (currentNode != m_endNode){
		m_pathFound.push_back(currentNode);
		StoryNode* minNode = nullptr;
		float minCost = 9999.f;
		for(StoryEdge* edge : currentNode->m_outboundEdges){
			if (edge->GetEnd()->GetShortestDistance() < minCost){
				minNode = edge->GetEnd();
				minCost = minNode->GetShortestDistance();
			}
		}
		currentNode = minNode;
	}
	m_pathFound.push_back(m_endNode);
	
}

Character * StoryGraph::GetCharacter(unsigned int index) const
{
	if (index < m_characters.size()){
		return m_characters[index];
	}
	return nullptr;
}

unsigned int StoryGraph::GetNumCharacters() const
{
	return (unsigned int) m_characters.size();
}

Vector2 StoryGraph::CalculateNodeForces(StoryNode * node) const
{
	Vector2 pull = (PULL_WEIGHT * CalculateNodePull(node));
	Vector2 push = (PUSH_WEIGHT *CalculateNodePush(node));
	return pull + push;
}

Vector2 StoryGraph::CalculateNodePull(StoryNode * node) const
{
	//find average direction to all connected nodes
	Vector2 direction = Vector2::ZERO;
	Vector2 nodePos = node->m_data->GetPosition();
	int numNodesConnectedTo = 0;
	for (StoryNode* graphNode : m_graph.m_nodes){
		if (graphNode != node ){
			Vector2 graphNodePos = graphNode->m_data->GetPosition();
			Vector2 difference = graphNodePos - nodePos;
			float length = difference.GetLength();
			if ( length > MAX_NODE_DISTANCE){
				//difference.y = (difference.y * 1.1f);
				direction += difference;
				numNodesConnectedTo++;
			}
		}
	}


	for (StoryEdge* edge : node->m_outboundEdges){
		StoryNode* otherEnd = edge->GetEnd();
		Vector2 endPos = otherEnd->m_data->GetPosition();
		Vector2 difference = endPos - nodePos;
		if (difference.GetLength() > MAX_NODE_DISTANCE){
			direction += difference;
			numNodesConnectedTo++;
		}
	}
	for (StoryEdge* edge : node->m_inboundEdges){
		StoryNode* otherEnd = edge->GetStart();
		Vector2 startPos = otherEnd->m_data->GetPosition();
		Vector2 difference = startPos - nodePos;
		if (difference.GetLength() > MAX_NODE_DISTANCE){
			direction += difference;
			numNodesConnectedTo++;
		}
	}
	Vector2 average = Vector2::ZERO;
	if (numNodesConnectedTo != 0){
		average = direction * (1.f / (float) numNodesConnectedTo);
	}

	return average;
}

Vector2 StoryGraph::CalculateNodePush(StoryNode * node) const
{
	//find average direction to all connected nodes

	Vector2 direction = Vector2::ZERO;
	Vector2 nodePos = node->m_data->GetPosition();
	int numNodesConnectedTo = 0;

	for (StoryNode* graphNode : m_graph.m_nodes){
		if (graphNode != node ){
			Vector2 graphNodePos = graphNode->m_data->GetPosition();
			Vector2 difference = nodePos - graphNodePos;
			float length = difference.GetLength();
			if (length < MIN_NODE_DISTANCE ){
				//difference.y = (difference.y * 1.1f);
				direction += difference;
				numNodesConnectedTo++;
			}
		}
	}

	for (StoryEdge* edge : node->m_outboundEdges){
		StoryNode* otherEnd = edge->GetEnd();
		Vector2 endPos = otherEnd->m_data->GetPosition();
		Vector2 diff = nodePos - endPos;
		//diff.y = (diff.y * 1.1f);
		if (diff.GetLength() < MIN_NODE_DISTANCE){
			direction += diff;
			numNodesConnectedTo++;
		}
	}
	for (StoryEdge* edge : node->m_inboundEdges){
		StoryNode* otherEnd = edge->GetStart();
		Vector2 startPos = otherEnd->m_data->GetPosition();
		Vector2 diff = nodePos - startPos;
		//diff.y = (diff.y * 1.1f);
		if (diff.GetLength() < MIN_NODE_DISTANCE){
			direction += diff;
			numNodesConnectedTo++;
		}
	}

	Vector2 average = Vector2::ZERO;
	if (numNodesConnectedTo != 0){
		average = direction * (1.f / (float) numNodesConnectedTo);
	}

	return average;
}

void StoryGraph::RenderGraph() const
{
	AABB2 bounds = g_theGame->GetUIBounds();
	std::string graphText = g_theGame->m_graph.ToString();
	g_theRenderer->DrawTextInBox2D(graphText, bounds, Vector2(0.f, 1.f), .01f, TEXT_DRAW_SHRINK_TO_FIT);

	for (StoryEdge* edge : m_graph.m_edges){
		RenderEdge(edge);
	}

	for (StoryNode* node : m_graph.m_nodes){
		RenderNode(node, node->m_data->GetPosition());
	}
	RenderPath();

	std::string characters = "Characters: \n";
	for (Character* c : m_characters){
		characters += c->GetName();
		characters += "\n";
	}
	g_theRenderer->DrawTextInBox2D(characters, bounds, Vector2(1.f, 1.f), .01f, TEXT_DRAW_SHRINK_TO_FIT);
}

void StoryGraph::RenderPath() const
{
	if (m_pathFound.size() > 0){
		for (int i = 0; i < (int) m_pathFound.size(); i++){
			if (i > 0){
				RenderEdge(m_graph.GetEdge(m_pathFound[i - 1], m_pathFound[i]), RGBA::GREEN);
			}
		}
		for (int i = 0; i < (int) m_pathFound.size(); i++){
			StoryNode* node = m_pathFound[i];
			if (i > 0){
				RenderEdge(m_graph.GetEdge(m_pathFound[i - 1], node), RGBA::GREEN);
			}
			RenderNode(node, node->m_data->GetPosition(), RGBA::GREEN);
		}
	}
}

/*
=====================================
Wrappers for DirectedGraph functions
=====================================
*/
StoryNode * StoryGraph::AddNode(StoryData* data)
{
	return m_graph.AddNode(data);
}

StoryNode * StoryGraph::AddNode(StoryNode * newNode)
{
	return m_graph.AddNode(newNode);
}

StoryEdge * StoryGraph::AddEdge(StoryNode * start, StoryNode * end)
{
	return m_graph.AddEdge(start, end);
}

StoryEdge * StoryGraph::AddEdge(StoryNode * start, StoryNode * end, StoryState* cost)
{
	return m_graph.AddEdge(start, end, cost);
}

void StoryGraph::RemoveAndDeleteNode(StoryData* data)
{
	StoryNode* nodeInGraph = m_graph.RemoveNode(data);
	if (nodeInGraph != nullptr){
		delete nodeInGraph;
	}
}

void StoryGraph::RemoveAndDeleteNode(StoryNode * node)
{
	StoryNode* nodeInGraph = m_graph.RemoveNode(node);
	if (nodeInGraph != nullptr){
		delete nodeInGraph;
	}
}

void StoryGraph::RemoveAndDeleteEdge(StoryNode * start, StoryNode * end)
{
	StoryEdge* foundEdge = m_graph.RemoveEdge(start, end);
	if (foundEdge != nullptr){
		delete foundEdge;
	}
}

void StoryGraph::RemoveAndDeleteEdge(StoryEdge * edge)
{
	StoryEdge* foundEdge = m_graph.RemoveEdge(edge);
	if (foundEdge != nullptr){
		delete foundEdge;
	}
}

StoryNode * StoryGraph::AddStart(StoryData* data)
{
	StoryNode* node = nullptr;
	if (!m_graph.ContainsNode(data)){
		node = m_graph.AddNode(data);
	} else {
		node = m_graph.GetNode(data->GetName());
	}
	m_startNode = node;
	return m_startNode;
}

StoryNode * StoryGraph::AddStart(StoryNode * startNode)
{
	if (!m_graph.ContainsNode(startNode)){
		m_graph.AddNode(startNode);
	}
	m_startNode = startNode;
	return m_startNode;
}

StoryNode * StoryGraph::AddEnd(StoryData* data)
{
	StoryNode* node = nullptr;
	if (!m_graph.ContainsNode(data)){
		node = m_graph.AddNode(data);
	} else {
		node = m_graph.GetNode(data->GetName());
	}
	m_endNode = node;
	return m_endNode;
}

StoryNode * StoryGraph::AddEnd(StoryNode * endNode)
{
	if (!m_graph.ContainsNode(endNode)){
		m_graph.AddNode(endNode);
	}
	m_endNode = endNode;
	return m_endNode;
}

void StoryGraph::Clear()
{
	TODO("define whether directed graph should delete its nodes...")
	m_graph.Clear();
}

StoryEdge * StoryGraph::GetEdge(StoryNode * start, StoryNode * end) const
{
	return m_graph.GetEdge(start,end);
}

StoryNode * StoryGraph::GetNode(std::string name) const
{
	return m_graph.GetNode(name);
}

unsigned int StoryGraph::GetNumNodes() const
{
	return m_graph.GetNumNodes();
}

unsigned int StoryGraph::GetNumEdges() const
{
	return m_graph.GetNumEdges();
}

std::string StoryGraph::ToString() const
{
	return m_graph.ToString();
}

bool StoryGraph::ContainsNode(StoryNode * node) const
{
	return m_graph.ContainsNode(node);
}

bool StoryGraph::ContainsNode(StoryData* data) const
{
	return m_graph.ContainsNode(data);
}

bool StoryGraph::ContainsEdge(StoryEdge * edge) const
{
	return m_graph.ContainsEdge(edge);
}

bool StoryGraph::ContainsEdge(StoryNode * start, StoryNode * end) const
{
	return m_graph.ContainsEdge(start, end);
}

bool StoryGraph::NodeRequirementsAreMet(StoryNode * node, StoryEdge* atEdge)
{
	int tries = 0;
	//keep trying to match valid characters with 
	if (StoryRequirementsMet(node, atEdge)){
		if (TryToSetCharactersForNode(node, atEdge)){
			return true;
		}	
	}
	return false;
}

bool StoryGraph::StoryRequirementsMet(StoryNode * node, StoryEdge * atEdge)
{
	TODO("Check node's story requirements against edge's storystate");
	return true;
}

bool StoryGraph::TryToSetCharactersForNode(StoryNode * node, StoryEdge * atEdge)
{
	//checks all characters and sees if they meet the requirements of the node.
	// for now, just take the first characters that fit w/o duplicates.
	StoryData* data = node->m_data;
	//CharacterRequirementSet reqs = node->m_data->m_characterReqs;

	// keep track of which characters meet requirements in which story node slots 
	//	- may want to change this to a score not a bool later.
	/* EX:
		character	|	0	|	1	|	2		<-- character slot on story node
		=================================
		Debbie		|	N	|	Y	|	Y
		Jerry		|	Y	|	N	|	Y
		Andy		|	N	|	N	|	Y
	*/
	std::vector<std::vector<bool>> charMeetsRequirementsArray;

	//keep track of which characters already used
	std::vector<bool> usedChars;
	for (int i = 0; i < m_characters.size(); i++){
		//init usedChars to be false
		usedChars.push_back(false);

		//create character requirement array
		std::vector<bool> characterRequirementList;
		for (unsigned int nodeSlot = 0; nodeSlot < data->GetNumCharacters(); nodeSlot++){
			characterRequirementList.push_back(data->DoesCharacterMeetSlotRequirementsAtEdge(m_characters[i], nodeSlot, atEdge));	
		}
		charMeetsRequirementsArray.push_back(characterRequirementList);
	}

	//try to assign characters to each character
	//try every possible permutation for now... we don't have that many characters :/
	for (unsigned int permutation = 0; permutation < data->GetNumCharacters(); permutation++){
		//for every character we have to assign, check every character in the story to see if they meet the reqs
		for (unsigned int nodeSlot = 0; nodeSlot < data->GetNumCharacters(); nodeSlot++){
			bool charSet = false;
			for (unsigned int storyChar = 0; storyChar < m_characters.size(); storyChar++ ){
				if (!charSet && !usedChars[storyChar]){
					//if the character hasn't already been used, see if it meets requirements.
					int nodeSlotIndex = (nodeSlot + permutation) % data->GetNumCharacters();
					if (charMeetsRequirementsArray[storyChar][nodeSlotIndex]){
						//this character meets the requirement for this permutation, mark it.
						node->m_data->SetCharacter(nodeSlotIndex, m_characters[storyChar]);
						usedChars[storyChar] = true;
						charSet = true;
					}
				}
			}
		}

		//if you reach the end of a permutation and not all node characters are set, 
		// wipe and start over.
		if (!node->m_data->AreAllCharactersSet()){
			node->m_data->ClearCharacters();
			for (int i = 0; i < (int) m_characters.size(); i++){
				usedChars[i] = false;
			}
		} else {
			//otherwise, the node's characters are set! yeaaaaaay
			return true;
		}
	}

	//if you reach this point, there's no permutation of characters that satisfy the node rightnow.
	return false;
}




void StoryGraph::RenderNode(StoryNode * node, Vector2 position, RGBA color) const
{
	std::string nodeName = node->GetName();


	AABB2 nodeBox = AABB2 (position, NODE_SIZE, NODE_SIZE);
	

	g_theRenderer->DrawAABB2(nodeBox, color);
	nodeBox.AddPaddingToSides(-.001f, -.001f);
	if (!g_theGame->IsDevMode()){
		g_theRenderer->DrawTextInBox2D(nodeName, nodeBox, Vector2(.5f, .5f), NODE_FONT_SIZE, TEXT_DRAW_WORD_WRAP, RGBA::RED);
	} else {
		std::string nodeData = node->GetData();
		g_theRenderer->DrawTextInBox2D(nodeData, nodeBox, Vector2(.5f, 1.f), NODE_FONT_SIZE, TEXT_DRAW_WORD_WRAP, RGBA::BLACK);
	}
}
	

void StoryGraph::RenderEdge(StoryEdge * edge, RGBA color) const
{
	Vector2 startPos = edge->GetStart()->m_data->GetPosition();
	Vector2 endPos = edge->GetEnd()->m_data->GetPosition();
	Vector2 direction = endPos - startPos;

	float orientation = direction.GetOrientationDegrees();
	Vector2 angledLeft  = Vector2::MakeDirectionAtDegrees(orientation  + 30.f);
	Vector2 angledRight = Vector2::MakeDirectionAtDegrees(orientation  - 30.f);

	angledLeft.NormalizeAndGetLength();
	angledRight.NormalizeAndGetLength();
	angledLeft *= .01f;
	angledRight *= .01f;

	direction *= .5f;
	Vector2 halfPoint = startPos + direction;
	//Vector2 secondThirdPoint = firstThirdPoint + direction;
	g_theRenderer->DrawLine2D(startPos, endPos, color);
	g_theRenderer->DrawLine2D(halfPoint, halfPoint - angledLeft, color);
	g_theRenderer->DrawLine2D(halfPoint, halfPoint - angledRight, color);
	
	std::string cost = Stringf("%f", edge->GetCost()->GetCost());
	g_theRenderer->DrawText2D(cost, halfPoint - Vector2(.002f, 0.f), .008f, RGBA::YELLOW);
}

StoryNode * StoryGraph::GetRandomPlotNode()
{
	int i = GetRandomIntLessThan(StoryGraph::s_plotNodes.size());
	return StoryGraph::s_plotNodes[i];
}

StoryNode * StoryGraph::GetRandomDetailNode()
{
	int i = GetRandomIntLessThan(StoryGraph::s_detailNodes.size());
	return StoryGraph::s_detailNodes[i];
}

StoryState* ShortestPathHeuristic(StoryEdge * edge, StoryStructure * currentStructure)
{
	return edge->GetCost();
}
