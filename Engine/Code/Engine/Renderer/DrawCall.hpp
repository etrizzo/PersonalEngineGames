#pragma once
#include "Engine/Core/EngineCommon.hpp"

class SubMesh;
class Material;
class Light;

class DrawCall
{
public:
	DrawCall(){};

	int GetSortLayer();
	float GetDistance(Vector3 pos);


	//will need to set camera, but won't store here
	Matrix44 m_model;
	SubMesh* m_mesh;
	Material* m_material;

	unsigned int m_lightCount; 
	//unsigned int m_lightIndices[MAX_LIGHTS]; 
	Light* m_lights[MAX_LIGHTS]; 

	// these kinda serve same purpose 
	int m_layer; 
	int m_queue;  // in Unity, these two are combined
};
