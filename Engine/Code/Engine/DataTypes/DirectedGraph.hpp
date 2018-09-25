#pragma once
#include "Engine/Core/EngineCommon.hpp"

template<typename T>
class DirectedEdge;

template<typename T>
class Node
{
public:
	Node(){};
	Node(T data);
	~Node();
	T m_data;
	float m_shortestDistance;

	std::string GetName();	//all data types T must have GetName function
	std::string GetData();	//all data types T must have ToString function

	void AddOutboundEdge(DirectedEdge<T>* edge);
	void AddInboundEdge(DirectedEdge<T>* edge);

	float GetShortestDistance() const;
	void SetShortestDistance(float distance);

	std::vector<DirectedEdge<T>*> m_outboundEdges;
	std::vector<DirectedEdge<T>*> m_inboundEdges;
	
};

template<typename T>
class DirectedEdge
{
public:
	DirectedEdge(Node<T>* start, Node<T>* end, float cost = 1.f);
	DirectedEdge(){};
	~DirectedEdge();

	void SetCost(float cost);

	float GetCost() const;
	Node<T>* GetStart() const;
	Node<T>* GetEnd() const;

	std::string ToString() const;

private:
	Node<T>* m_start;
	Node<T>* m_end;
	float m_cost;
};

template<typename T>
class DirectedGraph
{
public:
	DirectedGraph(){};
	~DirectedGraph();

	Node<T>* AddNode(T data);
	Node<T>* AddNode(Node<T>* newNode);
	DirectedEdge<T>* AddEdge(Node<T>* start, Node<T>* end, float cost = 1.f);

	Node<T>* RemoveNode(T data);
	Node<T>* RemoveNode(Node<T>* node);
	DirectedEdge<T>* RemoveEdge(Node<T>* start, Node<T>* end);
	DirectedEdge<T>* RemoveEdge(DirectedEdge<T>* edge);

	void Clear();

	DirectedEdge<T>* GetEdge(Node<T>* start, Node<T>* end) const;
	Node<T>* GetNode(std::string name) const;

	unsigned int GetNumNodes() const;
	unsigned int GetNumEdges() const;

	bool ContainsNode(Node<T>* node) const;
	bool ContainsNode(T data) const;
	bool ContainsEdge(DirectedEdge<T>* edge) const;
	bool ContainsEdge(Node<T>* start, Node<T>* end) const;

	std::string ToString() const;		//string representation of the graph


	std::vector<Node<T>*> m_nodes;
	std::vector<DirectedEdge<T>*> m_edges;

};




template<typename T>
inline Node<T>::Node(T data)
{
	m_data = data;
}

template<typename T>
inline Node<T>::~Node()
{

}

template<typename T>
inline std::string Node<T>::GetName()
{
	return m_data.GetName();
}

template<typename T>
inline std::string Node<T>::GetData()
{
	return m_data.ToString();
}

template<typename T>
inline void Node<T>::AddOutboundEdge(DirectedEdge<T>* edge)
{
	m_outboundEdges.push_back(edge);
}

template<typename T>
inline void Node<T>::AddInboundEdge(DirectedEdge<T>* edge)
{
	m_inboundEdges.push_back(edge);
}

template<typename T>
inline float Node<T>::GetShortestDistance() const
{
	return m_shortestDistance;
}

template<typename T>
inline void Node<T>::SetShortestDistance(float distance)
{
	m_shortestDistance = distance;
}

template<typename T>
inline DirectedEdge<T>::DirectedEdge(Node<T> * start, Node<T> * end, float cost)
{
	m_start = start;
	m_end = end;
	m_cost = cost;
}

template<typename T>
inline DirectedEdge<T>::~DirectedEdge()
{
}

template<typename T>
void DirectedEdge<T>::SetCost(float cost)
{
	m_cost = cost;
}

template<typename T>
float DirectedEdge<T>::GetCost() const
{
	return m_cost;
}

template<typename T>
Node<T> * DirectedEdge<T>::GetStart() const
{
	return m_start;
}

template<typename T>
Node<T> * DirectedEdge<T>::GetEnd() const
{
	return m_end;
}

template<typename T>
std::string DirectedEdge<T>::ToString() const
{
	std::string s = Stringf("%s --%3.2f--> %s", m_start->GetName().c_str(), GetCost(), m_end->GetName().c_str());
	return s;
}

template<typename T>
DirectedGraph<T>::~DirectedGraph()
{
	Clear();
}

template<typename T>
inline Node<T>* DirectedGraph<T>::AddNode(T data)
{
	if (!ContainsNode(data)){
		Node<T>* n = new Node<T>(data);
		m_nodes.push_back(n);
		return n;
	}
	return nullptr;
}

template<typename T>
inline Node<T>* DirectedGraph<T>::AddNode(Node<T>* newNode)
{
	if (!ContainsNode(newNode)){
		m_nodes.push_back(newNode);
		return newNode;
	}
	return nullptr;
}

template<typename T>
inline DirectedEdge<T>* DirectedGraph<T>::AddEdge(Node<T> * start, Node<T> * end, float cost)
{
	if (!ContainsEdge(start, end)){
		if (!ContainsNode(start)){
			AddNode(start);
		}
		if (!ContainsNode(end)){
			AddNode(end);
		}

		DirectedEdge<T>* newEdge = new DirectedEdge<T>(start, end, cost);
		start->AddOutboundEdge(newEdge);
		end->AddInboundEdge(newEdge);
		m_edges.push_back(newEdge);
		return newEdge;
	}
	return nullptr;
}

template<typename T>
Node<T>* DirectedGraph<T>::RemoveNode(T data)
{
	for (int index = (int) m_nodes.size() - 1; index >= 0; index--){
		if (m_nodes[index]->m_data == data){
			Node<T>* foundNode = m_nodes[index];
			RemoveAtFast(m_nodes, index);

			//remove any edges attached to the node from the graph
			for (DirectedEdge<T>* inEdge : foundNode->m_inboundEdges){
				RemoveEdge(inEdge);
			}
			for (DirectedEdge<T>* outEdge : foundNode->m_outboundEdges){
				RemoveEdge(outEdge);
			}
			return foundNode;
		}
	}
	return nullptr;
}

template<typename T>
Node<T>*  DirectedGraph<T>::RemoveNode(Node<T> * node)
{
	for (int index = (int) m_nodes.size() - 1; index >= 0; index--){
		if (m_nodes[index] == node){
			Node<T>* foundNode = m_nodes[index];
			RemoveAtFast(m_nodes, index);

			//remove any edges attached to the node from the graph
			for (DirectedEdge<T>* inEdge : foundNode->m_inboundEdges){
				RemoveEdge(inEdge);
			}
			for (DirectedEdge<T>* outEdge : foundNode->m_outboundEdges){
				RemoveEdge(outEdge);
			}
			return foundNode;
		}
	}
	return nullptr;
}

template<typename T>
DirectedEdge<T>* DirectedGraph<T>::RemoveEdge(Node<T>* start, Node<T>* end)
{
	for(int index = (int) m_edges.size() - 1; index >= 0; index--){
		if (m_edges[index]->GetStart() == start){
			if (m_edges[index]->GetEnd() == end){
				DirectedEdge<T>* foundEdge = m_edges[index];
				RemoveAtFast(m_edges, index); 
				return foundEdge;
			}
		}
	}
	return nullptr;
}

template<typename T>
DirectedEdge<T>* DirectedGraph<T>::RemoveEdge(DirectedEdge<T>* edge)
{
	for(int index = (int) m_edges.size() - 1; index >= 0; index--){
		if (m_edges[index] == edge){
			DirectedEdge<T>* foundEdge = m_edges[index];
			RemoveAtFast(m_edges, index); 
			return foundEdge;
		}
	}
	return nullptr;
}

template<typename T>
inline void DirectedGraph<T>::Clear()
{
	for (int index = (int) m_nodes.size() - 1; index >= 0; index--){
		delete m_nodes[index];
		RemoveAtFast(m_nodes, index);
	}
	for (int index = (int) m_edges.size() - 1; index >= 0; index--){
		delete m_edges[index];
		RemoveAtFast(m_edges, index);
	}
}

template<typename T>
DirectedEdge<T>* DirectedGraph<T>::GetEdge(Node<T>* start, Node<T>* end) const
{
	for(DirectedEdge<T>* edge : m_edges){
		if (edge->GetStart() == start){
			if (edge->GetEnd() == end){
				return edge;
			}
		}
	}
	return nullptr;
}

template<typename T>
Node<T>* DirectedGraph<T>::GetNode(std::string name) const
{
	for (Node<T>* node : m_nodes){
		if (node->GetName() == name){
			return node;
		}
	}
	return nullptr;
}

template<typename T>
inline unsigned int DirectedGraph<T>::GetNumNodes() const
{
	return (unsigned int) m_nodes.size();
}

template<typename T>
inline unsigned int DirectedGraph<T>::GetNumEdges() const
{
	return (unsigned int) m_edges.size();
}

template<typename T>
inline bool DirectedGraph<T>::ContainsNode(Node<T> * node) const
{
	for (Node<T>* nodeInGraph : m_nodes){
		if (nodeInGraph == node){
			return true;
		}
	}
	return false;
}

template<typename T>
inline bool DirectedGraph<T>::ContainsNode(T data) const
{
	for (Node<T>* nodeInGraph : m_nodes){
		if (nodeInGraph->m_data == data){
			return true;
		}
	}
	return false;
}

template<typename T>
bool DirectedGraph<T>::ContainsEdge(DirectedEdge<T>* edge) const
{
	for(DirectedEdge<T>* edgeInGraph : m_edges){
		if (edgeInGraph == edge){
			return true;
		}
	}
	return false;
}

template<typename T>
bool DirectedGraph<T>::ContainsEdge(Node<T>* start, Node<T>* end) const
{
	for(DirectedEdge<T>* edge : m_edges){
		if (edge->GetStart() == start){
			if (edge->GetEnd() == end){
				return true;
			}
		}
	}
	return false;
}

template<typename T>
std::string DirectedGraph<T>::ToString() const
{
	std::string nodes = "NODES:\n";
	for(Node<T>* node : m_nodes){
		nodes += node->GetData();
		nodes += "\n";
	}

	std::string edges = "\nEDGES:\n";
	for (DirectedEdge<T>* edge : m_edges){
		edges+= edge->ToString();
		edges += "\n";
	}


	return nodes + edges;
}

