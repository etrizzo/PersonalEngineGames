#include "StoryGraph.hpp"
#include "Game/Game.hpp"

void StoryGraph::RenderGraph() const
{
	AABB2 bounds = g_theGame->GetUIBounds();
	std::string graphText = g_theGame->m_graph.ToString();
	g_theRenderer->DrawTextInBox2D(graphText, bounds, Vector2(0.f, 1.f), .05f, TEXT_DRAW_SHRINK_TO_FIT);
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

StoryEdge * StoryGraph::AddEdge(StoryNode * start, StoryNode * end)
{
	return m_graph.AddEdge(start, end);
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
