#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"


// Camera that pivots around a target.
// Good for UI, or tactical RPGs
class OrbitCamera : public Camera
{
public:
	OrbitCamera(float radius, float rotation, float azimuth, Vector3 target);

	// whatever helpers you would prefer
	void SetTarget( Vector3 new_target ); 
	//sets position based on target's position and current radius
	void SetTargetAndPosition(Vector3 newTarget);
	void SetSphericalCoordinate( float radius, float rotation, float azimuth ); 
	void SnapToNextCorner(int direction = 1);

public:
	Vector3 m_target; 
	Vector3 m_position;

	float m_radius;      // distance from target
	float m_rotation;    // rotation around Y
	float m_azimuth;     // rotation toward up after previous rotation

	float m_maxAzimuth = 70.f;
	float m_minAximuth = 20.f;
	float m_maxRadius = 50.f;		//set these later
	float m_minRadius = 1.f;
};
