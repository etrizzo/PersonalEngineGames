#pragma once
#include "Engine/Core/VertexLayout.hpp"

enum eVertexType {
	VERTEX_TYPE_3DPCU,
	VERTEX_TYPE_LIT,
	NUM_VERTEX_TYPES
};

struct VertexMaster{
public:
	VertexMaster(){};
	Vector3 m_position;
	RGBA m_color;
	Vector2 m_uvs;

	Vector3 m_normal;
	Vector4 m_tangent;
};

struct Vertex3D_PCU{
public:
	Vertex3D_PCU(){};
	Vertex3D_PCU( VertexMaster const &other ) ;
	Vertex3D_PCU(Vector3 pos, RGBA color, Vector2 uvs);
	Vertex3D_PCU(Vector2 pos, RGBA color, Vector2 uvs);

	Vector3 m_position;
	RGBA m_color;
	Vector2 m_uvs;

	static VertexAttribute const ATTRIBUTES[]; 
	static VertexLayout const LAYOUT; 
};


struct Vertex3D_LIT{
public:
	Vertex3D_LIT(){};
	Vertex3D_LIT( VertexMaster const &other ) ;

	Vector3 m_position;
	Vector3 m_normal;
	Vector4 m_tangent;			//tangent.w is 1.f for right-handed system, -1.f for left-handed
	RGBA m_color;
	Vector2 m_uvs;
	

	static VertexAttribute const ATTRIBUTES[]; 
	static VertexLayout const LAYOUT; 
};


const VertexLayout& GetLayoutForType(eVertexType type);

