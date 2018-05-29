#include "OrbitCamera.hpp"

OrbitCamera::OrbitCamera(float radius, float rotation, float azimuth, Vector3 target)
	:Camera()
{
	//m_projMatrix.LoadIdentity();
	//m_viewMatrix.SetIdentity();
	SetSphericalCoordinate(radius, rotation, azimuth);
	SetTarget(target);
}

void OrbitCamera::SetTarget(Vector3 new_target)
{
	m_target = new_target;
	LookAt(m_position, m_target);
}


void OrbitCamera::SetTargetAndPosition(Vector3 newTarget)
{
	m_target = newTarget;
	m_position = m_target + SphericalToCartesian(m_radius, m_rotation, m_azimuth);
	LookAt(m_position, m_target);
}

void OrbitCamera::SetSphericalCoordinate(float radius, float rotation, float azimuth)
{
	m_radius	= ClampFloat(radius, m_minRadius, m_maxRadius);
	m_rotation	= rotation;
	m_azimuth	= ClampFloat(azimuth, m_minAximuth, m_maxAzimuth);
	m_position = m_target + SphericalToCartesian(m_radius, m_rotation, m_azimuth);
	LookAt(m_position, m_target);
}

void OrbitCamera::SnapToNextCorner(int direction)
{
	//there is probably a better way to do this
	float corners[4] = {-135.f, -45.f, 45.f, 135.f};
	float normalizedRotation = GetRotationNegative180To180(m_rotation);
	float newRot = 45.f * (float) direction;
	if (normalizedRotation > -135.f && normalizedRotation < -45.f){
		newRot = -90.f + newRot;
	} else if (normalizedRotation > -45.f && normalizedRotation < 45.f){
		newRot = 0.f + newRot;
	} else if (normalizedRotation > 45.f && normalizedRotation < 135.f){
		newRot = 90.f + newRot;
	} else if ((normalizedRotation > 135.f && normalizedRotation < 180.f) || (normalizedRotation > -180.f && normalizedRotation < -135.f)){
		newRot = 180.f + newRot;
	} else {
		//already on a corner angle, add 90 * dir
		newRot = normalizedRotation + (90 * (float) direction);
	}
	SetSphericalCoordinate(m_radius, newRot, m_azimuth);
}
