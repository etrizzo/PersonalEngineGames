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

	void SetLights(Light* lights[MAX_LIGHTS]);

	// these kinda serve same purpose 
	int m_layer; 
	int m_queue;  // in Unity, these two are combined
	//will need to set camera, but won't store here
	Matrix44 m_model;
	SubMesh* m_mesh;
	Material* m_material;

	float m_distanceToCamera = 9999.f;

	unsigned int m_lightCount; 
	 
	Light* m_lights[MAX_LIGHTS]; 

	
};


class DrawCall2D
{
public:
	DrawCall2D(){};

	int GetSpriteLayer();
	float GetYCoord();

	//void SetLights(Light* lights[MAX_LIGHTS]);

	// these kinda serve same purpose 
	int		m_spriteLayer; 
	float	m_yCoord;  // in Unity, these two are combined
				  //will need to set camera, but won't store here
	Matrix44 m_model;
	SubMesh* m_mesh;
	Material* m_material;

	//unsigned int m_lightCount; 

	//Light* m_lights[MAX_LIGHTS]; 


};
