#include "StoryGraph.hpp"
#include "Game/Game.hpp"

StoryGraph::StoryGraph()
{

}

void StoryGraph::UpdateNodePositions()
{
	for(int i = 0; i < m_graph.m_nodes.size(); i++){
		StoryNode* node = m_graph.m_nodes[i];
		if (node == m_startNode){
			node->m_data.SetPosition(Vector2(.05f, .5f));
		}
		else if (node == m_endNode){
			node->m_data.SetPosition(Vector2(.95f, .5f));
		} else {
			Vector2 nodeForce = CalculateNodeForces(node);
			Vector2 newNodePos = (nodeForce * .016) + node->m_data.GetPosition();
			node->m_data.SetPosition(newNodePos);
		}
	}
}

void StoryGraph::RunNodeAdjustments()
{
	for (int i = 0; i < NUM_NODE_ITERATIONS; i++){
		UpdateNodePositions();
	}
}

std::vector<StoryNode*> StoryGraph::FindPath(StoryHeuristicCB heuristic)
{

	return std::vector<StoryNode*>();
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
	Vector2 nodePos = node->m_data.GetPosition();
	int numNodesConnectedTo = 0;
	for (StoryNode* graphNode : m_graph.m_nodes){
		if (graphNode != node ){
			Vector2 graphNodePos = graphNode->m_data.GetPosition();
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
		Vector2 endPos = otherEnd->m_data.GetPosition();
		Vector2 difference = endPos - nodePos;
		if (difference.GetLength() > MAX_NODE_DISTANCE){
			direction += difference;
			numNodesConnectedTo++;
		}
	}
	for (StoryEdge* edge : node->m_inboundEdges){
		StoryNode* otherEnd = edge->GetStart();
		Vector2 startPos = otherEnd->m_data.GetPosition();
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
	Vector2 nodePos = node->m_data.GetPosition();
	int numNodesConnectedTo = 0;

	for (StoryNode* graphNode : m_graph.m_nodes){
		if (graphNode != node ){
			Vector2 graphNodePos = graphNode->m_data.GetPosition();
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
		Vector2 endPos = otherEnd->m_data.GetPosition();
		Vector2 diff = nodePos - endPos;
		//diff.y = (diff.y * 1.1f);
		if (diff.GetLength() < MIN_NODE_DISTANCE){
			direction += diff;
			numNodesConnectedTo++;
		}
	}
	for (StoryEdge* edge : node->m_inboundEdges){
		StoryNode* otherEnd = edge->GetStart();
		Vector2 startPos = otherEnd->m_data.GetPosition();
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
		RenderNode(node, node->m_data.GetPosition());
	}
	


}

/*
=====================================
Wrappers for DirectedGraph functions
=====================================
*/
StoryNode * StoryGraph::AddNode(StoryData data)
{
	return m_graph.AddNode(data);
}

StoryNode * StoryGraph::AddNode(StoryNode * newNode)
{
	return m_graph.AddNode(newNode);
}

StoryEdge * StoryGraph::AddEdge(StoryNode * start, StoryNode * end, float cost)
{
	return m_graph.AddEdge(start, end, cost);
}

void StoryGraph::RemoveAndDeleteNode(StoryData data)
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

StoryNode * StoryGraph::AddStart(StoryData data)
{
	StoryNode* node = nullptr;
	if (!m_graph.ContainsNode(data)){
		node = m_graph.AddNode(data);
	} else {
		node = m_graph.GetNode(data.GetName());
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

StoryNode * StoryGraph::AddEnd(StoryData data)
{
	StoryNode* node = nullptr;
	if (!m_graph.ContainsNode(data)){
		node = m_graph.AddNode(data);
	} else {
		node = m_graph.GetNode(data.GetName());
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

bool StoryGraph::ContainsNode(StoryData data) const
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




void StoryGraph::RenderNode(StoryNode * node, Vector2 position) const
{
	std::string nodeName = node->GetName();


	AABB2 nodeBox = AABB2 (position, NODE_SIZE, NODE_SIZE);
	

	g_theRenderer->DrawAABB2(nodeBox, RGBA::BLANCHEDALMOND);
	nodeBox.AddPaddingToSides(-.001f, -.001f);
	if (!g_theGame->IsDevMode()){
		g_theRenderer->DrawTextInBox2D(nodeName, nodeBox, Vector2(.5f, .5f), .05f, TEXT_DRAW_SHRINK_TO_FIT, RGBA::BLACK);
	} else {
		std::string nodeData = node->GetData();
		g_theRenderer->DrawTextInBox2D(nodeData, nodeBox, Vector2(.5f, 1.f), .01f, TEXT_DRAW_SHRINK_TO_FIT, RGBA::BLACK);
	}
}
	

void StoryGraph::RenderEdge(StoryEdge * edge) const
{
	Vector2 startPos = edge->GetStart()->m_data.GetPosition();
	Vector2 endPos = edge->GetEnd()->m_data.GetPosition();
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
	g_theRenderer->DrawLine2D(startPos, endPos, RGBA::WHITE);
	g_theRenderer->DrawLine2D(halfPoint, halfPoint - angledLeft, RGBA::WHITE);
	g_theRenderer->DrawLine2D(halfPoint, halfPoint - angledRight, RGBA::WHITE);
	
	std::string cost = Stringf("%f", edge->GetCost());
	g_theRenderer->DrawText2D(cost, halfPoint - Vector2(.002f, 0.f), .008f, RGBA::YELLOW);
}
