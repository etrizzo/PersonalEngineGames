#pragma once
#include "Engine/Core/EngineCommon.hpp"

class DirectedEdge;

class Node
{
public:
	Node();
	~Node();
	std::string m_name;

	std::vector<DirectedEdge*> m_outboundEdges;
	std::vector<DirectedEdge*> m_incomingEdges;
	
};

class DirectedEdge
{
public:
	DirectedEdge(Node* start, Node* end);
	DirectedEdge(){};
	~DirectedEdge();
	Node* m_start;
	Node* m_end;
	float m_cost;
};

class DirectedGraph
{
public:
	DirectedGraph(){};
	~DirectedGraph();

	std::vector<Node*> m_nodes;
	std::vector<DirectedEdge*> m_edges;

};