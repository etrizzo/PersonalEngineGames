#include "PerspectiveCamera.hpp"

PerspectiveCamera::PerspectiveCamera(PerspectiveCamera * camera)
	:Camera()
{
	m_transform = camera->m_transform;  
	m_projMatrix = camera->m_projMatrix;    
	//m_modelMatrix = camera->m_modelMatrix;
	GetViewMatrix();		//refreshes the view matrix

	m_fovDegrees = camera->m_fovDegrees;
	SetPerspectiveOrtho(m_fovDegrees,  g_gameConfigBlackboard.GetValue("windowAspect", 1.f), .1f, 100.f);		//should maybe just hardcode in farz but this seems like a nice correlation
	
}

void PerspectiveCamera::SetPerspectiveOrtho(float fovDegrees, float aspect, float nearZ, float farZ)
{
	m_fovDegrees = fovDegrees;
	SetProjection(Matrix44::MakePerspectiveProjection(fovDegrees, aspect, nearZ, farZ));
}
