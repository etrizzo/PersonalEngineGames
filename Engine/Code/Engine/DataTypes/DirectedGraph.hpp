#pragma once
#include "Engine/Core/EngineCommon.hpp"




template<typename T, typename C>
class DirectedEdge;

template<typename T, typename C>
class Node
{
public:
	Node(){};
	Node(T data);
	~Node();
	T m_data;
	float m_shortestDistance;
	int m_orderAdded;

	std::string GetName();	//all data types T must have GetName function
	std::string GetDataAsString();	//all data types T must have ToString function

	void AddOutboundEdge(DirectedEdge<T, C>* edge);
	void AddInboundEdge(DirectedEdge<T, C>* edge);

	void UpdateData(T data);

	bool RemoveOutboundEdge(DirectedEdge<T, C>* edge);
	bool RemoveInboundEdge(DirectedEdge<T, C>* edge);

	float GetShortestDistance() const;
	void SetShortestDistance(float distance);

	std::vector<Node<T,C>*> GetReachableNodes(int targetDepth = -1, int depth = 0);

	std::vector<DirectedEdge<T, C>*> m_outboundEdges;
	std::vector<DirectedEdge<T, C>*> m_inboundEdges;
	
};

template<typename T, typename C>
class DirectedEdge
{
public:
	DirectedEdge(Node<T, C>* start, Node<T, C>* end, C cost);
	DirectedEdge(Node<T, C>* start, Node<T, C>* end);
	DirectedEdge(){};
	~DirectedEdge();

	void SetCost(C cost);

	C GetCost() const;
	Node<T, C>* GetStart() const;
	Node<T, C>* GetEnd() const;

	std::string ToString() const;
	//std::string ToStringDebug() const;

private:
	Node<T, C>* m_start;
	Node<T, C>* m_end;
	C m_cost;
};

template<typename T, typename C>
class DirectedGraph
{
public:
	DirectedGraph(){};
	~DirectedGraph();

	Node<T, C>* AddNode(T data);
	Node<T, C>* AddNode(Node<T, C>* newNode);
	DirectedEdge<T, C>* AddEdge(Node<T, C>* start, Node<T, C>* end);
	DirectedEdge<T, C>* AddEdge(Node<T, C>* start, Node<T, C>* end, C cost);

	Node<T, C>* RemoveNode(T data);
	Node<T, C>* RemoveNode(Node<T, C>* node);
	DirectedEdge<T, C>* RemoveEdge(Node<T, C>* start, Node<T, C>* end);
	DirectedEdge<T, C>* RemoveEdge(DirectedEdge<T, C>* edge);

	void Clear();

	DirectedEdge<T, C>* GetEdge(Node<T, C>* start, Node<T, C>* end) const;
	Node<T, C>* GetNode(std::string name) const;

	unsigned int GetNumNodes() const;
	unsigned int GetNumEdges() const;

	bool ContainsNode(Node<T, C>* node) const;
	bool ContainsNode(T data) const;
	bool ContainsEdge(DirectedEdge<T, C>* edge) const;
	bool ContainsEdge(Node<T, C>* start, Node<T, C>* end) const;

	std::string ToString() const;		//string representation of the graph


	std::vector<Node<T, C>*> m_nodes;
	std::vector<DirectedEdge<T, C>*> m_edges;

};




template<typename T, typename C>
inline Node<T, C>::Node(T data)
{
	m_data = data;
}

template<typename T, typename C>
inline Node<T, C>::~Node()
{

}

template<typename T, typename C>
inline std::string Node<T, C>::GetName()
{
	return  ptr(m_data)->GetName();
}

template<typename T, typename C>
inline std::string Node<T, C>::GetDataAsString()
{
	std::string data = Stringf("cost:%.2f\nOrder Added: %i\n", m_shortestDistance, m_orderAdded);
	return  data + ptr(m_data)->ToString();
}

template<typename T, typename C>
inline void Node<T, C>::AddOutboundEdge(DirectedEdge<T, C>* edge)
{
	m_outboundEdges.push_back(edge);
}

template<typename T, typename C>
inline void Node<T, C>::AddInboundEdge(DirectedEdge<T, C>* edge)
{
	m_inboundEdges.push_back(edge);
}

template<typename T, typename C>
inline void Node<T, C>::UpdateData(T data)
{
	ptr(m_data)->AddData(data);
}

template<typename T, typename C>
inline bool Node<T, C>::RemoveOutboundEdge(DirectedEdge<T, C>* edge)
{
	for (unsigned int i = 0; i < (unsigned int) m_outboundEdges.size(); i++){
		if (m_outboundEdges[i] == edge){
			RemoveAtFast(m_outboundEdges, i);
			return true;
		}
	}
	return false;
}

template<typename T, typename C>
inline bool Node<T, C>::RemoveInboundEdge(DirectedEdge<T, C>* edge)
{
	for (unsigned int i = 0; i < (unsigned int) m_inboundEdges.size(); i++){
		if (m_inboundEdges[i] == edge){
			RemoveAtFast(m_inboundEdges, i);
			return true;
		}
	}
	return false;
}

template<typename T, typename C>
inline float Node<T, C>::GetShortestDistance() const
{
	return m_shortestDistance;
}

template<typename T, typename C>
inline void Node<T, C>::SetShortestDistance(float distance)
{
	m_shortestDistance = distance;
}

template<typename T, typename C>
inline std::vector<Node<T,C>*> Node<T, C>::GetReachableNodes(int targetDepth, int depth)
{
	if (targetDepth > 0){
		if (depth > targetDepth){
			return std::vector<Node<T,C>*>();
		}
	}
	std::vector<Node<T,C>*> nodes = std::vector<Node<T,C>*>();
	for (DirectedEdge<T,C>* edge : m_outboundEdges){
		nodes.push_back(edge->GetEnd());
		//recursively get all reachable nodes and add to found list
		std::vector<Node<T,C>*> outboundNodes = edge->GetEnd()->GetReachableNodes(targetDepth, depth + 1);
		for (Node<T,C>* node : outboundNodes){
			bool add = true;
			//check for duplicates
			for (Node<T,C>* alreadyAdded : nodes){
				if (node == alreadyAdded){
					add = false;
					break;
				}
			}
			if (add){
				nodes.push_back(node);
			}
		}
	}
	return nodes;
}


template<typename T, typename C>
inline DirectedEdge<T, C>::DirectedEdge(Node<T, C>* start, Node<T, C>* end, C cost)
{
	m_start = start;
	m_end = end;
	m_cost = cost;
}

template<typename T, typename C>
inline DirectedEdge<T, C>::DirectedEdge(Node<T, C>* start, Node<T, C>* end)
{
	m_start = start;
	m_end = end;
	m_cost = C();
}

template<typename T, typename C>
inline DirectedEdge<T, C>::~DirectedEdge()
{
}

template<typename T, typename C>
inline void DirectedEdge<T, C>::SetCost(C cost)
{
	m_cost = cost;
}



template<typename T, typename C>
C DirectedEdge<T, C>::GetCost() const
{
	return m_cost;
}

template<typename T, typename C>
Node<T, C> * DirectedEdge<T, C>::GetStart() const
{
	return m_start;
}

template<typename T, typename C>
Node<T, C> * DirectedEdge<T, C>::GetEnd() const
{
	return m_end;
}

template<typename T, typename C>
std::string DirectedEdge<T, C>::ToString() const
{
	std::string s = Stringf("%s --%s--> %s", m_start->GetName().c_str(), m_cost->ToString().c_str(), m_end->GetName().c_str());
	return s;
}

template<typename T, typename C>
DirectedGraph<T, C>::~DirectedGraph()
{
	Clear();
}

template<typename T, typename C>
inline Node<T, C>* DirectedGraph<T, C>::AddNode(T data)
{
	if (!ContainsNode(data)){
		Node<T, C>* n = new Node<T, C>(data);
		n->m_orderAdded = (int) m_nodes.size();
		m_nodes.push_back(n);
		return n;
	}
	return nullptr;
}

template<typename T, typename C>
inline Node<T, C>* DirectedGraph<T, C>::AddNode(Node<T, C>* newNode)
{
	if (!ContainsNode(newNode)){
		newNode->m_orderAdded = (int) m_nodes.size();
		m_nodes.push_back(newNode);
		return newNode;
	}
	return nullptr;
}

template<typename T, typename C>
inline DirectedEdge<T, C>* DirectedGraph<T, C>::AddEdge(Node<T, C>* start, Node<T, C>* end)
{
	if (!ContainsEdge(start, end)){
		if (!ContainsNode(start)){
			AddNode(start);
		}
		if (!ContainsNode(end)){
			AddNode(end);
		}

		DirectedEdge<T, C>* newEdge = new DirectedEdge<T, C>(start, end);
		start->AddOutboundEdge(newEdge);
		end->AddInboundEdge(newEdge);
		m_edges.push_back(newEdge);
		return newEdge;
	}
	return nullptr;
}

template<typename T, typename C>
inline DirectedEdge<T, C>* DirectedGraph<T, C>::AddEdge(Node<T, C> * start, Node<T, C> * end, C cost)
{
	if (!ContainsEdge(start, end)){
		if (!ContainsNode(start)){
			AddNode(start);
		}
		if (!ContainsNode(end)){
			AddNode(end);
		}

		DirectedEdge<T, C>* newEdge = new DirectedEdge<T, C>(start, end, cost);
		start->AddOutboundEdge(newEdge);
		end->AddInboundEdge(newEdge);
		m_edges.push_back(newEdge);
		return newEdge;
	}
	return nullptr;
}

template<typename T, typename C>
Node<T, C>* DirectedGraph<T, C>::RemoveNode(T data)
{
	for (int index = (int) m_nodes.size() - 1; index >= 0; index--){
		if (m_nodes[index]->m_data == data){
			Node<T, C>* foundNode = m_nodes[index];
			RemoveAtFast(m_nodes, index);

			//remove any edges attached to the node from the graph
			for (DirectedEdge<T, C>* inEdge : foundNode->m_inboundEdges){
				RemoveEdge(inEdge);
			}
			for (DirectedEdge<T, C>* outEdge : foundNode->m_outboundEdges){
				RemoveEdge(outEdge);
			}
			return foundNode;
		}
	}
	return nullptr;
}

template<typename T, typename C>
Node<T, C>*  DirectedGraph<T, C>::RemoveNode(Node<T, C> * node)
{
	for (int index = (int) m_nodes.size() - 1; index >= 0; index--){
		if (m_nodes[index] == node){
			Node<T, C>* foundNode = m_nodes[index];
			RemoveAtFast(m_nodes, index);

			//remove any edges attached to the node from the graph
			for (DirectedEdge<T, C>* inEdge : foundNode->m_inboundEdges){
				RemoveEdge(inEdge);
			}
			for (DirectedEdge<T, C>* outEdge : foundNode->m_outboundEdges){
				RemoveEdge(outEdge);
			}
			return foundNode;
		}
	}
	return nullptr;
}

template<typename T, typename C>
DirectedEdge<T, C>* DirectedGraph<T, C>::RemoveEdge(Node<T, C>* start, Node<T, C>* end)
{
	for(int index = (int) m_edges.size() - 1; index >= 0; index--){
		if (m_edges[index]->GetStart() == start){
			if (m_edges[index]->GetEnd() == end){
				DirectedEdge<T, C>* foundEdge = m_edges[index];
				foundEdge->GetStart()->RemoveOutboundEdge(foundEdge);
				foundEdge->GetEnd()->RemoveInboundEdge(foundEdge);
				RemoveAtFast(m_edges, index); 
				return foundEdge;
			}
		}
	}
	return nullptr;
}

template<typename T, typename C>
DirectedEdge<T, C>* DirectedGraph<T, C>::RemoveEdge(DirectedEdge<T, C>* edge)
{
	for(int index = (int) m_edges.size() - 1; index >= 0; index--){
		if (m_edges[index] == edge){
			DirectedEdge<T, C>* foundEdge = m_edges[index];
			foundEdge->GetStart()->RemoveOutboundEdge(foundEdge);
			foundEdge->GetEnd()->RemoveInboundEdge(foundEdge);
			RemoveAtFast(m_edges, index); 
			return foundEdge;
		}
	}
	return nullptr;
}

template<typename T, typename C>
inline void DirectedGraph<T, C>::Clear()
{
	for (int index = (int) m_nodes.size() - 1; index >= 0; index--){
		//delete m_nodes[index];
		RemoveAtFast(m_nodes, index);
	}
	for (int index = (int) m_edges.size() - 1; index >= 0; index--){
		//delete m_edges[index];
		RemoveAtFast(m_edges, index);
	}
}

template<typename T, typename C>
DirectedEdge<T, C>* DirectedGraph<T, C>::GetEdge(Node<T, C>* start, Node<T, C>* end) const
{
	for(DirectedEdge<T, C>* edge : m_edges){
		if (edge->GetStart() == start){
			if (edge->GetEnd() == end){
				return edge;
			}
		}
	}
	return nullptr;
}

template<typename T, typename C>
Node<T, C>* DirectedGraph<T, C>::GetNode(std::string name) const
{
	for (Node<T, C>* node : m_nodes){
		if (node->GetName() == name){
			return node;
		}
	}
	return nullptr;
}

template<typename T, typename C>
inline unsigned int DirectedGraph<T, C>::GetNumNodes() const
{
	return (unsigned int) m_nodes.size();
}

template<typename T, typename C>
inline unsigned int DirectedGraph<T, C>::GetNumEdges() const
{
	return (unsigned int) m_edges.size();
}

template<typename T, typename C>
inline bool DirectedGraph<T, C>::ContainsNode(Node<T, C> * node) const
{
	for (Node<T, C>* nodeInGraph : m_nodes){
		if (nodeInGraph == node){
			return true;
		}
	}
	return false;
}

template<typename T, typename C>
inline bool DirectedGraph<T, C>::ContainsNode(T data) const
{
	for (Node<T, C>* nodeInGraph : m_nodes){
		if (nodeInGraph->m_data == data){
			return true;
		}
	}
	return false;
}

template<typename T, typename C>
bool DirectedGraph<T, C>::ContainsEdge(DirectedEdge<T, C>* edge) const
{
	for(DirectedEdge<T, C>* edgeInGraph : m_edges){
		if (edgeInGraph == edge){
			return true;
		}
	}
	return false;
}

template<typename T, typename C>
bool DirectedGraph<T, C>::ContainsEdge(Node<T, C>* start, Node<T, C>* end) const
{
	for(DirectedEdge<T, C>* edge : m_edges){
		if (edge->GetStart() == start){
			if (edge->GetEnd() == end){
				return true;
			}
		}
	}
	return false;
}

template<typename T, typename C>
std::string DirectedGraph<T, C>::ToString() const
{
	std::string nodes = "NODES:\n";
	for(Node<T, C>* node : m_nodes){
		nodes += node->GetDataAsString();
		nodes += "\n";
	}

	std::string edges = "\nEDGES:\n";
	for (DirectedEdge<T, C>* edge : m_edges){
		edges+= edge->ToString();
		edges += "\n";
	}


	return nodes + edges;
}

