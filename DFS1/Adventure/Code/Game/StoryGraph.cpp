#include "StoryGraph.hpp"
#include "Game/Game.hpp"
#include "Game/Character.hpp"
#include "Game/StoryState.hpp"
#include "Game/CharacterRequirementSet.hpp"
#include "Game/ActionDefinition.hpp"

StoryGraph::StoryGraph()
{

}


void StoryGraph::LoadDataSet(std::string setName)
{
	m_dataSet = DataSet::GetDataSet(setName);
	if (m_dataSet == nullptr)
	{
		ConsolePrintf(RGBA::RED, "No dataset found: %s", setName.c_str());
	}
}

void StoryGraph::SelectCharactersForGraph(int numCharacters)
{
	m_characters.clear();
	if (numCharacters == 0){
		numCharacters = g_gameConfigBlackboard.GetValue("numCharacters", (int) m_dataSet->m_characters.size());
	}
	if (numCharacters >=  m_dataSet->m_characters.size()){
		for(Character* character :  m_dataSet->m_characters){
			m_characters.push_back(character);
		}
	} else {
		//select randomly
		int numSelected = 0;
		while (numSelected < numCharacters){
			int idx = GetRandomIntLessThan( m_dataSet->m_characters.size());
			if (!Contains(m_characters,  m_dataSet->m_characters[idx])){
				m_characters.push_back( m_dataSet->m_characters[idx]);
				numSelected++;
			}
		}
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
			if (nodeForce.GetLengthSquared() > (MIN_DISTANCE_TO_MOVE * MIN_DISTANCE_TO_MOVE)){
				Vector2 newNodePos = (nodeForce * .016) + node->m_data->GetPosition();
				node->m_data->SetPosition(newNodePos);
			}
		}
	}
}

void StoryGraph::RunNodeAdjustments()
{
	for (int i = 0; i < NUM_NODE_ITERATIONS; i++){
		UpdateNodePositions();
	}
}



void StoryGraph::HandleInput()
{
	if (!g_theInput->IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
		m_hoveredNode = nullptr;
		m_hoveredEdge = nullptr;
		for(StoryNode* node : m_graph.m_nodes){
			Disc2 nodeBounds = GetNodeBounds(node);
			if (nodeBounds.IsPointInside(g_theGame->m_normalizedMousePos)){
				m_hoveredNode = node;
				break;
			}
		}
		if (m_hoveredNode == nullptr){
			for(StoryEdge* edge : m_graph.m_edges){
				OBB2 edgeBounds = GetEdgeBounds(edge);
				if (edgeBounds.IsPointInside(g_theGame->m_normalizedMousePos)){
					m_hoveredEdge = edge;
					break;
				}
			}
		}
	} else {
		if (m_hoveredNode != nullptr){
			m_hoveredNode->m_data->m_graphPosition = g_theGame->m_normalizedMousePos;
		}
	}
	if (g_theInput->WasMouseButtonJustPressed(MOUSE_BUTTON_LEFT)){
		if (m_hoveredNode == nullptr){
			m_selectedEdge = m_hoveredEdge;
		}
		//if (m_hoveredEdge != nullptr){
		//	m_selectedEdge = m_hoveredEdge;
		//}
	}
}

void StoryGraph::RunGenerationPairs(int numPairs)
{
	Clear();

	//m_graph.RunGeneration(NUM_PLOT_NODES_TO_GENERATE,NUM_DETAIL_NODES_TO_GENERATE + NUM_PLOT_NODES_TO_GENERATE);
	GenerateStartAndEnd();
	//generate node pairs
	for (int i = 0; i < numPairs; i++){
		//add an event node
		StoryNode* newNode = AddSingleEventNode();
		//add an outcome node that works off of that event node
		AddOutcomeNodesToEventNode(newNode);
	}
	RunNodeAdjustments();
}

void StoryGraph::RunGenerationPlotAndDetail(int numPlotNodes, int desiredSize)
{
	GenerateSkeleton(numPlotNodes);
	RunNodeAdjustments();
	AddDetailNodesToDesiredSize(desiredSize);
}

void StoryGraph::GenerateSkeleton(int numPlotNodes)
{
	ClearSavedState();
	GenerateStartAndEnd();
	StoryNode* addNode = nullptr;
	StoryData* newData = nullptr;
	while(GetNumNodes() < numPlotNodes){
		//re-roll if you get a duplicate node.
		StoryDataDefinition* sourceNode = m_dataSet->GetRandomEventNode();
		bool alreadyUsed = false;
		if (Contains(m_usedPlotNodes, sourceNode)){
			alreadyUsed = true;
			if (CheckRandomChance(REROLL_REPEAT_PLOT_NODE_CHANCE)){
				sourceNode =  m_dataSet->GetRandomEventNode();
			}
		}

		//clone data
		newData = new StoryData(sourceNode);
		addNode = new StoryNode(newData);
		//try to add the cloned data, if it doesn't work, clean up
		if (!FindEdgeForNewEventNodeAndAdd(addNode)){
			delete newData;
			delete addNode;
			newData = nullptr;
			addNode = nullptr;
		} else {
			if (!alreadyUsed){
				//add node to used plot node list
				m_usedPlotNodes.push_back(sourceNode);
			}
		}
	}
}


StoryNode * StoryGraph::AddSingleEventNode()
{
	
	bool added = false;
	StoryNode* addNode = nullptr;
	StoryData* newData = nullptr;
	while(!added){
		//re-roll if you get a duplicate node.
		StoryDataDefinition* sourceNode = m_dataSet->GetRandomEventNode();
		bool alreadyUsed = true;
		int rerollRepeatTries = 0;
		while (alreadyUsed && rerollRepeatTries < MAX_REPEAT_REROLLS){
			if (Contains(m_usedPlotNodes, sourceNode)){
				alreadyUsed = true;
				if (CheckRandomChance(REROLL_REPEAT_DETAIL_NODE_CHANCE)){
					sourceNode = m_dataSet->GetRandomEventNode();
				} else {
					alreadyUsed = false;		//let it slide based on chance
				}
			} else {
				alreadyUsed = false;
			}
		}

		//clone data
		newData = new StoryData(sourceNode);
		addNode = new StoryNode(newData);
		//try to add the cloned data, if it doesn't work, clean up
		if (!FindEdgeForNewEventNodeAndAdd(addNode)){
			delete newData;
			delete addNode;
			newData = nullptr;
			addNode = nullptr;
		} else {
			added = true;
			if (!alreadyUsed){
				//add node to used plot node list
				m_usedPlotNodes.push_back(sourceNode);
			}
		}
	}
	return addNode;
}

StoryNode * StoryGraph::AddEventNodeAtEdge(StoryEdge * edge)
{
	bool added = false;
	int tries = 0;
	int maxTries = 100;
	StoryNode* addedNode = nullptr;
	while (tries < maxTries && !added){
		//reroll if you get a duplicate node.
		StoryDataDefinition* sourceNode = m_dataSet->GetEventNodeWithWeights(edge->GetCost(), 3.f);

		//try to add the node
		if (StoryRequirementsMet(sourceNode, edge)){
			//if the story requirements are met, check character requirements
			std::vector<Character*> charsForNode = GetCharactersForNode(sourceNode, edge);
			//if you found characters for the node, add the node.
			if (charsForNode.size() != 0){
				addedNode = CreateAndAddEventNodeAtEdge(sourceNode, edge, charsForNode);		//always does it
				if (addedNode != nullptr){
					added = true;
				} else {
					tries++;
				}
			} else {
				tries++;
			}
		} else {
			tries++;
		}
	}

	if (added)
	{
		UpdateDepths();
	}

	return addedNode;
}

void StoryGraph::AddOutcomeNodesToEventNode(StoryNode * plotNode)
{
	ClearSavedState();
	std::vector<StoryEdge*> edges = plotNode->m_outboundEdges;
	int startingSize = edges.size();
	for (int i = 0; i < startingSize; i++){
		StoryEdge* edge = edges[i];
		if (!edge->GetCost()->m_isVoid){
			TryToAddDetailNodeAtEdge(edge, 10);
		}
	}
}


void StoryGraph::AddDetailNodesToDesiredSize(int desiredSize)
{
	std::vector<StoryEdge*> skeletonEdges = std::vector<StoryEdge*>();
	for (StoryEdge* edge : m_graph.m_edges){
		skeletonEdges.push_back(edge);
	}
	ClearSavedState();
	bool canAdd = true;
	//go through EVERY edge and try to place a detail node until there are no edges that can add OR you have too many nodes
	canAdd = false;
	int startingSize = skeletonEdges.size();
	for (int i = 0; i < startingSize; i++){
		StoryEdge* edge = skeletonEdges[i];
		if (!edge->GetCost()->m_isVoid){
			if (TryToAddDetailNodeAtEdge(edge, 10)){
				canAdd = true;
			}
		}
	}

}

void StoryGraph::GenerateStartAndEnd()
{
	if (m_startNode == nullptr && m_endNode == nullptr){
		AddStart(new StoryNode(new StoryData("START")));
		AddEnd(new StoryNode(new StoryData("END")));
		AddEdge(m_startNode, m_endNode, new StoryState(1.f, m_characters.size(), this));
	}
	
}

bool StoryGraph::TryToAddDetailNodeAtEdge(StoryEdge * edge, int maxTries)
{
	bool added = false;
	int tries = 0;
	while (tries < maxTries && !added){
		//reroll if you get a duplicate node.
		StoryDataDefinition* sourceNode = m_dataSet->GetOutcomeNodeWithWeights(edge->GetCost(), 3.f);
		bool alreadyUsed = true;
		int rerollRepeatTries = 0;
		while (alreadyUsed && rerollRepeatTries < MAX_REPEAT_REROLLS){
			if (Contains(m_usedDetailNodes, sourceNode)){
				alreadyUsed = true;
				if (CheckRandomChance(REROLL_REPEAT_DETAIL_NODE_CHANCE)){
					if (rerollRepeatTries < (float) MAX_REPEAT_REROLLS * .5f){
						sourceNode = m_dataSet->GetOutcomeNodeWithWeights(edge->GetCost(), 3.f);
					} else {
						sourceNode = m_dataSet->GetOutcomeNodeWithWeights(edge->GetCost(), 1.f);
					}
				} else {
					alreadyUsed = false;		//let it slide based on chance
				}
			} else {
				alreadyUsed = false;
			}
		}

		//try to add the node
		if (StoryRequirementsMet(sourceNode, edge)){
			//if the story requirements are met, check character requirements
			std::vector<Character*> charsForNode = GetCharactersForNode(sourceNode, edge);
			//if you found characters for the node, add the node.
			if (charsForNode.size() != 0){
				if (CreateAndAddOutcomeNodeAtEdge(sourceNode, edge, charsForNode)){
					added = true;
				} else {
					tries++;
				}
			} else {
				tries++;
			}
		} else {
			tries++;
		}
	}

	if (added)
	{
		UpdateDepths();
	}

	return added;
	//StoryNode* addNode = nullptr;
	//StoryData* newData = nullptr;
	//int tries = 0;
	//bool added = false;
	//while(tries < 100 && !added){
	//	//reroll if you get a duplicate node.
	//	StoryDataDefinition* sourceNode = StoryGraph::GetRandomDetailNode();
	//	bool alreadyUsed = false;
	//	if (Contains(m_usedDetailNodes, sourceNode)){
	//		alreadyUsed = true;
	//		if (CheckRandomChance(REROLL_REPEAT_DETAIL_NODE_CHANCE)){
	//			sourceNode =  StoryGraph::GetRandomDetailNode();
	//		}
	//	}

	//	//clone data
	//	newData = new StoryData(sourceNode, 0);
	//	addNode = new StoryNode(newData);
	//	//try to add the cloned data, if it doesn't work, clean up
	//	added = AddDetailNode(addNode);
	//	if (!added){
	//		delete newData;
	//		delete addNode;
	//		newData = nullptr;
	//		addNode = nullptr;
	//	} else {
	//		//add node to used detail node list
	//		if (!alreadyUsed){
	//			m_usedDetailNodes.push_back(sourceNode);
	//		}
	//		//add all actions to this branch
	//		for (int i = 1; i < sourceNode->m_actions.size(); i++){
	//			StoryData* branchData = new StoryData(sourceNode, i);
	//			StoryNode* branchNode = new StoryNode(branchData);
	//			AddBranchAroundNode(addNode, branchNode, true);
	//		}
	//	}
	//	tries++;
	//}
	//return added;	
}

bool StoryGraph::FindEdgeForNewEventNodeAndAdd(StoryNode* newPlotNode)
{

	//walk along story until you can place the node
	std::queue<StoryEdge*> openEdges;
	//openEdges.push(m_startNode->m_outboundEdges[0]);
	StoryEdge* startEdge = GetEdgeForNewEventNode();
	if (startEdge->GetCost()->m_isLocked){
		return false;
	}
	openEdges.push(startEdge);
	StoryNode* nodeToAddAfter; 
	StoryEdge* edgeToAddAt = nullptr;
	bool foundSpot = false;
	while (!openEdges.empty() && !foundSpot){
		edgeToAddAt = openEdges.back();
		openEdges.pop();
		//if the story meets the requirements of the new nodes at that edge, continue.
		//if not, add the end of the edge to the open list.
		if (!NodeRequirementsAreMet(newPlotNode, edgeToAddAt)){
			//try again

			//if you didn't branch and find a spot, add all your outbound edges to the open edges list
			/*if (!foundSpot){
				for (StoryEdge* edge : edgeToAddAt->GetEnd()->m_outboundEdges){
					openEdges.push(edge);
				}
			}*/
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

//bool StoryGraph::AddOutcomeNode(StoryNode * newDetailNode)
//{
//
//	//walk along story until you can place the node
//	std::queue<StoryEdge*> openEdges;
//	openEdges.push(m_graph.m_edges[GetRandomIntLessThan(m_graph.GetNumEdges())]);
//	//openEdges.push(m_startNode->m_outboundEdges[0]);
//	StoryNode* nodeToAddAfter; 
//	StoryEdge* edgeToAddAt = nullptr;
//	bool foundSpot = false;
//	while (!openEdges.empty()){
//		edgeToAddAt = openEdges.back();
//		openEdges.pop();
//		//if the story meets the requirements of the new nodes at that edge, continue.
//		//if not, add the end of the edge to the open list.
//		if (!NodeRequirementsAreMet(newDetailNode, edgeToAddAt)){
//			for (StoryEdge* edge : edgeToAddAt->GetEnd()->m_outboundEdges){
//				openEdges.push(edge);
//			}
//		} else {
//			//you have found a place that works
//			foundSpot = true;
//			break;
//		}
//	}
//
//	//add the plot node at the found node
//	if (foundSpot){
//
//		AddNodeAtEdge(newDetailNode, edgeToAddAt);
//	}
//
//	return foundSpot;
//}

bool StoryGraph::CreateAndAddOutcomeNodeAtEdge(StoryDataDefinition * dataDefinition, StoryEdge * edgeToAddAt, std::vector<Character*> charactersForNode)
{
	//at this point, we need to know what characters will be set for this node set.
	StoryNode* startNode = edgeToAddAt->GetStart();
	StoryNode* endNode = edgeToAddAt->GetEnd();
	StoryData* newData = nullptr;
	StoryNode* newNode = nullptr;
	bool addedAnyNodes = false;
	for (int i = 0; i < dataDefinition->m_actions.size(); i++){
		//check random chance to just ignore this action
		if (!CheckRandomChance(dataDefinition->m_actions[i]->m_chanceToPlaceAction)){
			continue;
		} else {
			//add a node for this action
			StoryNode* possibleEnd = endNode;
			newData = new StoryData(dataDefinition, i);
			newData->SetCharacters(charactersForNode);
			newNode = new StoryNode(newData);
			//add the action at this edge.

			//not sure what to do with cost...
			StoryState* incomingCost = new StoryState(*edgeToAddAt->GetCost());
			StoryState* outgoingCost = new StoryState(*edgeToAddAt->GetCost());
			outgoingCost->UpdateFromNode(newNode->m_data);
			//clamp the incoming act range to the node's max
			incomingCost->m_possibleActRange.max = Min(incomingCost->m_possibleActRange.max, newNode->m_data->m_definition->m_actRange.max);
			//verify that the node doesn't break at this edge.
					// (check that all characters meet the requirements for the end node)
			// get all reachable nodes
			// check reachable nodes for a suitable end node		
			std::vector<StoryNode*> possibleEndNodes = possibleEnd->GetReachableNodes();
			bool atEnd = false;
			int reachableIndex = 1;
			while (!(possibleEnd->m_data->IsCompatibleWithIncomingEdge(outgoingCost)) && !atEnd){
				if (reachableIndex < possibleEndNodes.size()){
					possibleEnd = possibleEndNodes[reachableIndex];
					reachableIndex++;
				} else {
					atEnd = true;
				}
			}

			if (!atEnd){
				addedAnyNodes = true;
				//add the node here
				AddEdge(startNode, newNode, incomingCost);
				AddEdge(newNode, possibleEnd, outgoingCost);

				//update all reachable nodes w/ this data
				std::vector<StoryNode*> reachableNodes = newNode->GetReachableNodes();

				for (StoryNode* reachable : reachableNodes){
					//edge->UpdateDataFromNode(data);
					reachable->UpdateData(newNode->m_data);
					for (StoryEdge* edge : reachable->m_outboundEdges){
						edge->GetCost()->UpdateFromNode(newNode->m_data);
					}
				}
			} else {
				delete newNode;
				delete newData;
				newNode = nullptr;
				newData = nullptr;
			}
		}
	}

	if (addedAnyNodes){
		//delete the old edge, w/o removing the nodes on the edge
		//edgeToAddAt->GetCost()->SetAsVoid();
		//m_edgesToVoid.push_back(edgeToAddAt);
		m_graph.RemoveEdge(edgeToAddAt);
		delete edgeToAddAt;
		return true;
	} else {
		return false;
	}
}

StoryNode * StoryGraph::CreateAndAddEventNodeAtEdge(StoryDataDefinition * dataDefinition, StoryEdge * edgeToAddAt, std::vector<Character*> charactersForNode)
{
	StoryData* newData = new StoryData(dataDefinition);
	newData->SetCharacters(charactersForNode);
	StoryNode* newNode = new StoryNode(newData);
	AddNodeAtEdge(newNode, edgeToAddAt);
	return newNode;
}



bool StoryGraph::AddBranchAroundNode(StoryNode* existingNode, StoryNode* nodeToAdd, bool branchToFutureNodesIfNecessary)
{
	StoryNode* start = existingNode->m_inboundEdges[0]->GetStart();
	StoryNode* end = existingNode->m_outboundEdges[0]->GetEnd();
	StoryState* incomingCost = new StoryState(*existingNode->m_inboundEdges[0]->GetCost());
	StoryState* outGoingCost = new StoryState(*existingNode->m_inboundEdges[0]->GetCost());

	StoryEdge* pretendEdge;
	//pretendEdge->GetCost()->UpdateFromNode(start->m_data);
	bool added = false;
	bool atEnd = false;
	std::vector<StoryNode*> reachableNodes = end->GetReachableNodes();
	int reachableIndex = 0;
	while (!added && !atEnd){
		incomingCost->UpdateFromNode(start->m_data);
		outGoingCost->UpdateFromNode(start->m_data);

		pretendEdge = new StoryEdge(start, end, incomingCost);
		//if you meet the node requirements, add edge here
		if (NodeRequirementsAreMet(nodeToAdd, pretendEdge)){
			outGoingCost->UpdateFromNode(nodeToAdd->m_data);
			StoryEdge* inEdge = AddEdge(start, nodeToAdd, incomingCost);
			StoryEdge* outEdge = AddEdge(nodeToAdd, end, outGoingCost);
			outEdge->GetCost()->UpdateFromNode(nodeToAdd->m_data);
			added = true;
			delete pretendEdge;
		} else {
			//if there are still reachable nodes, search the next reachable node.
			if (reachableIndex < reachableNodes.size()){
				//update end node (searched node)
				end = reachableNodes[reachableIndex];
				delete pretendEdge;
				incomingCost = new StoryState(*existingNode->m_inboundEdges[0]->GetCost());
				outGoingCost = new StoryState(*existingNode->m_inboundEdges[0]->GetCost());
				//pretendEdge = new StoryEdge(start, end, incomingCost);
				reachableIndex++;
			} else {
				atEnd = true;
			}
		}
	}
	return added;
}

bool StoryGraph::AddEndingsToEachBranch(int maxTries)
{
	bool allPathsHaveEndings = false;

	int maxAdds = 6;
	int addedNodes = 0;
	int tries = 0;
	while (!allPathsHaveEndings && addedNodes < maxAdds && tries < maxTries){
		tries++;
		allPathsHaveEndings = true;
		//look at all incoming edges to the end node
		for(StoryEdge* incomingEdge : m_endNode->m_inboundEdges)
		{
			//if you find a node before the end node that doesn't end the act, need to add to it
			StoryData* startNode = incomingEdge->GetStart()->m_data;
			if (!startNode->DoesNodeEndAct())
			{
				allPathsHaveEndings = false;
				//add event/outcome pairs until it does
				StoryNode* newNode = AddEventNodeAtEdge(incomingEdge);
				if (newNode !=nullptr){
					AddOutcomeNodesToEventNode(newNode);
					addedNodes++;
				}
				
				break;
			} 
		}
		
	}

	RemoveBranchesWithNoEnding();
	return !CheckForEmptyGraph();
}

void StoryGraph::RemoveBranchesWithNoEnding()
{
	bool allPathsHaveEndings = false;
	while (!allPathsHaveEndings){
		allPathsHaveEndings = true;
		//look at all incoming edges to the end node
		StoryNode* nodeToDelete = nullptr;
		for(StoryEdge* incomingEdge : m_endNode->m_inboundEdges)
		{
			//if you find a node before the end node that doesn't end the act, need to add to it
			nodeToDelete = incomingEdge->GetStart();
			//StoryData* nodeToDelete = incomingEdge->GetStart()->m_data;
			if (!nodeToDelete->m_data->DoesNodeEndAct())
			{
				allPathsHaveEndings = false;
				break;
			} else {
				nodeToDelete = nullptr;
			}
		}
		if (nodeToDelete != nullptr)
		{
			//remove this node and all nodes before it that don't have any other outgoing edges
			std::vector<StoryNode*> branchToDelete = std::vector<StoryNode*>();
			std::queue<StoryNode*> nodesToCheck = std::queue<StoryNode*>();

			nodesToCheck.push(nodeToDelete);

			//check every node that DEFINITELY ends at a dead-end (i.e., their only outbound edge goes to the false-end node we found earlier.
			while (!nodesToCheck.empty())
			{
				StoryNode* check = nodesToCheck.back();
				nodesToCheck.pop();
				for (StoryEdge* edge : check->m_inboundEdges)
				{
					StoryNode* inNode = edge->GetStart();
					if (inNode->m_outboundEdges.size() < 2){
						nodesToCheck.push(inNode);
					}
				}
				branchToDelete.push_back(check);
			}

			//Delete all the nodes we found that are end-less

			for(int i = 0; i < branchToDelete.size(); i++)
			{
				StoryNode* toDelete = branchToDelete[i];
				RemoveAndDeleteNode(toDelete);
			}

		}
	}
}

bool StoryGraph::CheckForEmptyGraph()
{
	if (m_graph.m_nodes.size() <= 1){
		return true;
	} else {
		return false;
	}
}

void StoryGraph::IdentifyBranchesAndAdd(int numBranchesToAdd)
{
	if (numBranchesToAdd < 0){
		numBranchesToAdd = (int) (GetNumNodes() * .25f);
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

			added = (CheckReachableNodesForBranch(reachableNodes, currentNode) != nullptr);
			if (added){
				branchesAdded++;
			}
			
		}
		tries++;

	}
}

bool StoryGraph::AttemptToAddBranchAfterFail(StoryNode * startingNode, StoryEdge * failedEdge, StoryNode* newNode)
{
	//if you can't add at a real edge, random chance to add a branch
	//branches are "fake" edges between the node and any reachable node,
	// with the same starting state as the edge you are trying to add at (?)
	// but now, the compare will happen on the new end node
	std::vector<StoryNode*> reachableNodes = startingNode->GetReachableNodes();
	StoryEdge* fakeEdge = CheckReachableNodesForBranch(reachableNodes, startingNode);
	if (fakeEdge == nullptr){
		return false;
	}
	//Add the new node onto the fake edge

	//copied from AddNodeAtEdge
	//m_graph.RemoveEdge(existingEdge);
	//not sure what to do with cost...
	if (NodeRequirementsAreMet(newNode, fakeEdge)){
		AddNodeAtEdge(newNode, fakeEdge);
		return true;
	};
	return false;


	//return false;
}

StoryEdge* StoryGraph::CheckReachableNodesForBranch(std::vector<StoryNode*> reachableNodes, StoryNode * startingNode)
{
	StoryEdge* added = nullptr;
	//for each future node, see if an outgoing edge would meet the story state requirements of that node.
	for (StoryNode* reachable : reachableNodes){
		//if so, add an edge and continue (tier 1)
		if (true){
			TODO("Check for cycles properly");
			if (!m_graph.ContainsEdge(startingNode, reachable) && !m_graph.ContainsEdge(reachable, startingNode)){
				StoryState* newState = new StoryState(GetRandomFloatInRange(0.f, 5.f), GetNumCharacters(), this);
				added = m_graph.AddEdge(startingNode, reachable, newState);
				return added;
			}
		}
	}

	return added;
}

void StoryGraph::AddNodeAtEdge(StoryNode * newNode, StoryEdge * existingEdge)
{
	m_graph.RemoveEdge(existingEdge);
	//not sure what to do with cost...
	StoryState* incomingCost = new StoryState(*existingEdge->GetCost());
	StoryState* outgoingCost = new StoryState(*existingEdge->GetCost());
	if (newNode->m_data->m_definition->m_shouldLockIncomingEdge){
		incomingCost->m_isLocked = true;
	}
	outgoingCost->UpdateFromNode(newNode->m_data);
	//clamp the incoming act range to the node's max
	incomingCost->m_possibleActRange.max = Min(incomingCost->m_possibleActRange.max, newNode->m_data->m_definition->m_actRange.max);
	AddEdge(existingEdge->GetStart(), newNode, incomingCost);
	AddEdge(newNode, existingEdge->GetEnd(), outgoingCost);
	delete existingEdge;

	//newNode->UpdateState(newNode->m_data->m_effects);
	std::vector<StoryNode*> reachableNodes = newNode->GetReachableNodes();

	for (StoryNode* reachable : reachableNodes){
		//edge->UpdateDataFromNode(data);
		reachable->UpdateData(newNode->m_data);
		for (StoryEdge* edge : reachable->m_outboundEdges){
			edge->GetCost()->UpdateFromNode(newNode->m_data);
		}
	}

	UpdateDepths();
	SetNodePositionsByDepth();

	
}

void StoryGraph::SetBranchChance(float branchChance)
{
	m_branchChance = branchChance;
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
			if (openNodes.empty()){
				//return the path
				break;
			} else {
				continue;
			}
			
		} else {
			//otherwise, get all out-bound neighbors.
			//std::vector<StoryNode*> outboundNeighbors;
			float cost = currentNode->GetShortestDistance();
			for (StoryEdge* edge : currentNode->m_outboundEdges){
				//outboundNeighbors.push_back(edge->GetEnd());
				StoryNode* neighbor = edge->GetEnd();
				//if the new path is faster than the old path, update cost
				StoryState* edgeState = heuristic(edge);
				float newCost = cost + (float) (edgeState->GetCost());
				if (newCost > 9000.f){
					ConsolePrintf("Big Cost :(");
				}
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
				} else if (neighbor->GetShortestDistance() > 9000.f){
					ConsolePrintf("Big Distance :(");
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
		if (currentNode->m_outboundEdges.size() == 0){
			ConsolePrintf("Node with no outbound edges?");
		}
		for(StoryEdge* edge : currentNode->m_outboundEdges){
			if (edge->GetEnd()->GetShortestDistance() < minCost){
				minNode = edge->GetEnd();
				minCost = minNode->GetShortestDistance();
			}
		}
		
		if (minNode == nullptr){
			ConsolePrintf(RGBA::RED, "path could not be found");
			return;
		}
		currentNode = minNode;
	}
	m_pathFound.push_back(m_endNode);
	
}

void StoryGraph::PrintPath() 
{
	if (m_pathFound.size() == 0){
		ConsolePrintf(RGBA::RED, "No path found. Run find_path");
		return;
	}
	m_pathString = "";
	//Create the story
	for (StoryNode* node : m_pathFound){
		m_pathString += node->GetName();
		ConsolePrintf(node->GetName().c_str());
	}

}

void StoryGraph::ClearSavedState()
{
	m_pathFound.clear();
	m_hoveredEdge = nullptr;
	m_hoveredNode = nullptr;
}

Character * StoryGraph::GetCharacter(unsigned int index) const
{
	if (index < m_characters.size()){
		return m_characters[index];
	}
	return nullptr;
}

Character * StoryGraph::GetCharacterByName(std::string name) const
{
	for (Character* character : m_characters){
		if (character->GetName() == name){
			return character;
		}
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
	//for (StoryNode* graphNode : m_graph.m_nodes){
	//	if (graphNode != node ){
	//		Vector2 graphNodePos = graphNode->m_data->GetPosition();
	//		Vector2 difference = graphNodePos - nodePos;
	//		float length = difference.GetLength();
	//		if ( length > MAX_NODE_DISTANCE){
	//			//difference.y = (difference.y * 1.1f);
	//			direction += difference;
	//			numNodesConnectedTo++;
	//		}
	//	}
	//}


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
	//std::string graphText = g_theGame->m_graph.ToString();
	g_theRenderer->DrawTextInBox2D(m_pathString, bounds, Vector2(0.f, 1.f), .01f, TEXT_DRAW_SHRINK_TO_FIT);

	for (StoryEdge* edge : m_graph.m_edges){
		RenderEdge(edge);
	}

	for (StoryNode* node : m_graph.m_nodes){
		RenderNode(node, node->m_data->GetPosition(), m_nodeDefaultColor);
	}
	RenderPath();

	RenderDebugInfo();
}

void StoryGraph::RenderDebugInfo() const
{
	AABB2 bounds = g_theGame->GetUIBounds();
	AABB2 selectedBox = bounds.GetPercentageBox(Vector2(.01f, .7f), Vector2(.2f, .99f));
	AABB2 hoveredBox = bounds.GetPercentageBox(Vector2(.8f, .7f), Vector2(.99f, .99f));
	if (m_selectedEdge != nullptr){
		g_theRenderer->DrawAABB2(selectedBox, m_edgeSelectColor.GetColorWithAlpha(100));
		g_theRenderer->DrawAABB2Outline(selectedBox, RGBA::WHITE);
		selectedBox.AddPaddingToSides(-.005f, -.005f);
		std::string selectedString = "Selected Edge:\n";
		selectedString += m_selectedEdge->GetCost()->GetDevString();
		g_theRenderer->DrawTextInBox2D(selectedString, selectedBox, Vector2::ALIGN_CENTER_LEFT, EDGE_FONT_SIZE, TEXT_DRAW_SHRINK_TO_FIT, RGBA::WHITE);
	}

	if (m_hoveredEdge != nullptr){
		if (m_selectedEdge == nullptr){
			hoveredBox = selectedBox;
		}
		g_theRenderer->DrawAABB2(hoveredBox, m_edgeHoverColor.GetColorWithAlpha(80));
		g_theRenderer->DrawAABB2Outline(hoveredBox, RGBA::WHITE.GetColorWithAlpha(128));
		hoveredBox.AddPaddingToSides(-.005f, -.005f);
		std::string hoveredString = "Hovered Edge:\n";
		hoveredString += m_hoveredEdge->GetCost()->GetDevString();
		g_theRenderer->DrawTextInBox2D(hoveredString, hoveredBox, Vector2::ALIGN_CENTER_LEFT, EDGE_FONT_SIZE, TEXT_DRAW_SHRINK_TO_FIT, RGBA::LIGHTGRAY);
	}
	//std::string characters = "Characters: \n";
	//for (Character* c : m_characters){
	//	characters += c->GetName();
	//	characters += "\n";
	//}
	//g_theRenderer->DrawTextInBox2D(characters, bounds, Vector2(1.f, 1.f), .01f, TEXT_DRAW_SHRINK_TO_FIT);
}

void StoryGraph::RenderPath() const
{
	if (m_pathFound.size() > 0){
		for (int i = 0; i < (int) m_pathFound.size(); i++){
			if (i > 0){
				RenderEdge(m_graph.GetEdge(m_pathFound[i - 1], m_pathFound[i]), m_pathColor);
			}
		}
		for (int i = 0; i < (int) m_pathFound.size(); i++){
			StoryNode* node = m_pathFound[i];
			RenderNode(node, node->m_data->GetPosition(), m_pathColor);
		}
	}
}

Disc2 StoryGraph::GetNodeBounds(StoryNode * node) const
{
	return Disc2(node->m_data->m_graphPosition, NODE_SIZE);
}

OBB2 StoryGraph::GetEdgeBounds(StoryEdge * edge) const
{
	Vector2 startPos = edge->GetStart()->m_data->GetPosition();
	Vector2 endPos = edge->GetEnd()->m_data->GetPosition();
	Vector2 direction = endPos - startPos;

	Vector2 halfPoint = startPos + (direction * .5f);

	OBB2 box = OBB2(halfPoint, direction, Vector2(direction.GetLength() * .5f, .005f));
	return box;
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
	StoryEdge* edge =  m_graph.AddEdge(start, end);
	edge->GetCost()->m_enddata = end->m_data;
	edge->GetCost()->m_startData  = start->m_data;
	return edge;
}

StoryEdge * StoryGraph::AddEdge(StoryNode * start, StoryNode * end, StoryState* cost)
{
	StoryEdge* edge =  m_graph.AddEdge(start, end, cost);
	edge->GetCost()->m_enddata = end->m_data;
	edge->GetCost()->m_startData  = start->m_data;
	return edge;
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
	m_startNode->m_data->SetPosition(START_NODE_POSITION);
	return m_startNode;
}

StoryNode * StoryGraph::AddStart(StoryNode * startNode)
{
	if (!m_graph.ContainsNode(startNode)){
		m_graph.AddNode(startNode);
	}
	m_startNode = startNode;
	m_startNode->m_data->SetPosition(START_NODE_POSITION);
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
	m_endNode->m_data->SetPosition(END_NODE_POSITION);
	return m_endNode;
}

StoryNode * StoryGraph::AddEnd(StoryNode * endNode)
{
	if (!m_graph.ContainsNode(endNode)){
		m_graph.AddNode(endNode);
	}
	m_endNode = endNode;
	m_endNode->m_data->SetPosition(END_NODE_POSITION);
	return m_endNode;
}

void StoryGraph::Clear()
{
	TODO("define whether directed graph should delete its nodes...");
	for (int i = 0; i < (int) m_graph.m_nodes.size(); i++){
		delete m_graph.m_nodes[i];
		m_graph.m_nodes[i] = nullptr;
	}
	for (int i = 0; i < (int) m_graph.m_edges.size(); i++){
		delete m_graph.m_edges[i];
		m_graph.m_edges[i] = nullptr;
	}

	m_graph.Clear();
	m_pathFound.clear();

	m_usedPlotNodes.clear();
	m_usedDetailNodes.clear();
	m_startNode = nullptr;
	m_endNode = nullptr;

	m_selectedEdge = nullptr;
	m_hoveredEdge = nullptr;
	m_hoveredNode = nullptr;
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

bool StoryGraph::NodeRequirementsAreMet(StoryNode * newNode, StoryEdge* atEdge)
{
	if (!DoRangesOverlap(newNode->m_data->m_definition->m_actRange, atEdge->GetCost()->m_possibleActRange))
	{
		return false;
	}
	//keep trying to match valid characters with 
	if (StoryRequirementsMet(newNode->m_data->m_definition, atEdge)){
		if (TryToSetCharactersForNode(newNode, atEdge)){
			return true;
		}	
	}
	return false;
}

bool StoryGraph::StoryRequirementsMet(StoryDataDefinition * node, StoryEdge * atEdge)
{
	if (node->DoesEdgeMeetStoryRequirements(atEdge->GetCost())){
		//meets the requirements for the incoming edge - now we just need to check if the node after you will be invalidated by the new node
		StoryState* newEdge = new StoryState(*atEdge->GetCost());
		newEdge->UpdateFromNodeDefinition(node);
		if (atEdge->GetEnd()->m_data->IsCompatibleWithIncomingEdge(newEdge))
		{
			return true;
		} else {
			return false;
		}
	}
	
	return false;
}

bool StoryGraph::TryToSetCharactersForNode(StoryNode * node, StoryEdge * atEdge)
{
	std::vector<Character*> charsInOrder = GetCharactersForNode(node->m_data->m_definition, atEdge);
	if (charsInOrder.size() < node->m_data->m_definition->m_numCharacters){
		// no characters fit on the node :(
		return false;
	} else {
		node->m_data->SetCharacters(charsInOrder);
		return true;
	}
}

std::vector<Character*> StoryGraph::GetCharactersForNode(StoryDataDefinition* nodeDefinition, StoryEdge * atEdge)
{
	//checks all characters and sees if they meet the requirements of the node.
	// for now, just take the first characters that fit w/o duplicates.
	//StoryData* data = node->m_data;
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
		for (unsigned int nodeSlot = 0; nodeSlot < nodeDefinition->GetNumCharacters(); nodeSlot++){
			characterRequirementList.push_back(nodeDefinition->DoesCharacterMeetSlotRequirementsAtEdge(m_characters[i], nodeSlot, atEdge));	
		}
		charMeetsRequirementsArray.push_back(characterRequirementList);
	}

	std::vector<std::vector<Character*>> possiblePermutations;
	//make an empty array
	std::vector<Character*> charsInOrder = ClearCharacterArray(nodeDefinition->GetNumCharacters());

	//try to assign characters to each character
	//try every possible permutation for now... we don't have that many characters :/
	//for (unsigned int permutation = 0; permutation < nodeDefinition->GetNumCharacters(); permutation++){
	for (unsigned int permutation = 0; permutation < m_characters.size(); permutation++){
		//for every character we have to assign, check every character in the story to see if they meet the reqs
		for (unsigned int nodeSlot = 0; nodeSlot < nodeDefinition->GetNumCharacters(); nodeSlot++){
			bool charSet = false;
			for (unsigned int storyChar = 0; storyChar < m_characters.size(); storyChar++ ){
				int charSlotIndex = (storyChar + permutation) % m_characters.size();
				if (!charSet && !usedChars[charSlotIndex]){
					//if the character hasn't already been used, see if it meets requirements.
					//int nodeSlotIndex = (nodeSlot + permutation) % nodeDefinition->GetNumCharacters();
					int nodeSlotIndex = nodeSlot;
					if (charMeetsRequirementsArray[charSlotIndex][nodeSlotIndex]){
						//this character meets the requirement for this permutation, mark it.
						charsInOrder[nodeSlotIndex] = m_characters[charSlotIndex];
						//node->m_data->SetCharacter(nodeSlotIndex, m_characters[storyChar]);
						usedChars[charSlotIndex] = true;
						charSet = true;
					}
				}
			}
		}


		//if you reach the end of a permutation and all characters are set, cache it off.
		// then wipe and start over.
		if (AreAllCharactersSet(charsInOrder)){
			possiblePermutations.push_back(charsInOrder);
		}
		charsInOrder = ClearCharacterArray(nodeDefinition->GetNumCharacters());
		for (int i = 0; i < (int) m_characters.size(); i++){
			usedChars[i] = false;
		}
	}
	
	//if we've saved any permutations that work, choose a random one.
	if (possiblePermutations.size() > 0){
		return possiblePermutations[GetRandomIntLessThan(possiblePermutations.size())];
	}
	//if not, there's no permutation of characters that satisfy the node right now.
	return std::vector<Character*>();
}

StoryEdge * StoryGraph::GetEdgeForNewEventNode() const
{
	int numEdges = m_graph.m_edges.size();
	StoryEdge* eventEdge = nullptr;
	while (eventEdge == nullptr){
		eventEdge = m_graph.m_edges[GetRandomIntLessThan(numEdges)];
		if (eventEdge->GetStart()->m_data->m_type == PLOT_NODE && eventEdge->GetEnd()->m_data->m_type == DETAIL_NODE){
			//we are in between an event node and it's subsequent outcome node
			//reroll
			eventEdge = nullptr;
		}
	}
	return eventEdge;
}




void StoryGraph::RenderNode(StoryNode * node, Vector2 position, RGBA color) const
{
	if (node->m_data->m_type == DETAIL_NODE){
		color = RGBA::BEEFEE;
	}

	std::string nodeName = node->GetName();
	AABB2 nodeBox = AABB2 (position, NODE_SIZE, NODE_SIZE);
	
	//determine color of node - mix w/ hovered state, and whether or not it's part of hovered/selected edge
	if (node == m_hoveredNode){
		color = Interpolate(color, m_nodeHoverColor, .25f);
	} else {
		if (m_selectedEdge != nullptr){
			if (m_selectedEdge->GetStart() == node || m_selectedEdge->GetEnd() == node){
				color = Interpolate(color, m_edgeSelectColor, .3f);
			}
		}
		if (m_hoveredEdge != nullptr){
			if (m_hoveredEdge->GetStart() == node || m_hoveredEdge->GetEnd() == node){
				color = Interpolate(color, m_edgeHoverColor, .3f);
			}
		} 
	}

	if (node->m_data->m_type == PLOT_NODE){
		g_theRenderer->DrawDisc2(nodeBox.GetCenter(), NODE_SIZE * 1.05, color);
	} else {
		g_theRenderer->DrawAABB2(nodeBox, color);
	}
	nodeBox.AddPaddingToSides(-.001f, -.001f);
	if (!g_theGame->IsDevMode()){
		g_theRenderer->DrawTextInBox2D(nodeName, nodeBox, Vector2(.5f, .5f), NODE_FONT_SIZE, TEXT_DRAW_WORD_WRAP, m_nodeTextColor);
	} else {
		std::string nodeData = node->GetDataAsString();
		g_theRenderer->DrawTextInBox2D(nodeData, nodeBox, Vector2(.5f, .5f), NODE_FONT_SIZE, TEXT_DRAW_WORD_WRAP, m_nodeTextColor);
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
	angledLeft *= EDGE_ARROW_SIZE;
	angledRight *= EDGE_ARROW_SIZE;

	TODO("Find a way to improve placement of edge text.");
	//float pos = RangeMapFloat(edge->GetCost()->GetCost(), 0.f, 5.f, .4f, .6f);

	//direction *= pos;
	Vector2 halfPoint = startPos + (direction * .5f);

	OBB2 box = GetEdgeBounds(edge);
	RGBA boxColor = m_edgeDefaultColor;
	if (edge == m_hoveredEdge){
		boxColor = m_edgeHoverColor;
	} else if (edge == m_selectedEdge){
		boxColor = m_edgeSelectColor;
	}
	if (edge->GetCost()->m_isLocked){
		boxColor = boxColor.GetColorWithAlpha(.5f);
	}
	g_theRenderer->DrawOBB2(box, boxColor);

	//Vector2 secondThirdPoint = firstThirdPoint + direction;
	g_theRenderer->DrawLine2D(startPos, endPos, color, color);
	g_theRenderer->DrawLine2D(halfPoint, halfPoint - angledLeft, color, color);
	g_theRenderer->DrawLine2D(halfPoint, halfPoint - angledRight, color, color);
	

	AABB2 costBox = AABB2(halfPoint + Vector2(0.f, EDGE_FONT_SIZE * .5f), .01, .008);

	std::string cost = edge->GetCost()->m_possibleActRange.ToString();
	g_theRenderer->DrawTextInBox2D(cost, costBox, Vector2::HALF, EDGE_FONT_SIZE, TEXT_DRAW_WORD_WRAP, m_edgeTextColor);
	//g_theRenderer->DrawText2D(cost, halfPoint - Vector2(.002f, 0.f), EDGE_FONT_SIZE, RGBA::YELLOW);
}

void StoryGraph::UpdateDepths()
{
	for(StoryNode* node: m_graph.m_nodes){
		node->ResetDepth();
	}
	m_startNode->SetDepthRecursively(0);
}

void StoryGraph::SetNodePositionsByDepth()
{
	int maxDepth = 0;
	for(StoryNode* node : m_graph.m_nodes){
		if (node->m_depth > maxDepth){
			maxDepth = node->m_depth;
		}
	}

	for(StoryNode* node : m_graph.m_nodes){
		if (node != m_startNode && node != m_endNode){
			float depth = (float) node->m_depth;
			float perc = depth / (float) maxDepth;

			Vector2 startToEnd = m_endNode->m_data->m_graphPosition - m_startNode->m_data->m_graphPosition;
			Vector2 displacement = perc * startToEnd;
			displacement.y = GetRandomFloatInRange(-.2f, .2f);

			node->m_data->m_graphPosition = m_startNode->m_data->m_graphPosition + displacement;
		}
	}
}

std::vector<Character*> StoryGraph::ClearCharacterArray(int numCharacters)
{
	std::vector<Character*> chars = std::vector<Character*>();
	for (int i = 0; i < numCharacters; i++){
		chars.push_back(nullptr);
	}
	return chars;
}

bool StoryGraph::AreAllCharactersSet(const std::vector<Character*>& chars) const
{
	for (Character* character : chars){
		if (character == nullptr){
			return false;
		}
	}
	return true;
}

StoryState* ShortestPathHeuristic(StoryEdge * edge)
{
	return edge->GetCost();
}

StoryState * RandomPathHeuristic(StoryEdge * edge)
{
	return new StoryState(GetRandomFloatInRange(0.f, 10.f), 1, nullptr);
}

StoryState * CalculateChanceHeuristic(StoryEdge * edge)
{
	edge->GetCost()->SetStartAndEnd(edge->GetStart(), edge->GetEnd());
	float baseChance = edge->GetCost()->UpdateAndGetChance();
	float cost = 2000.f;
	if (CheckRandomChance(baseChance)){
		cost = 1;
	} else {
		cost = 100 * (1 - baseChance);
	}
	if (cost >= 9000.f){
		ConsolePrintf("BigBoy");
	}
	edge->GetCost()->m_cost = cost;
	//edge->GetCost()->m_characterStates.size();
	TODO("Fix story state on heuristic to use the right graph");
	return new StoryState(cost, edge->GetCost()->m_characterStates.size(), nullptr) ;
}
