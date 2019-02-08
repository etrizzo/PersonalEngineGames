#include "PerspectiveCamera.hpp"

PerspectiveCamera::PerspectiveCamera(PerspectiveCamera * camera)
	:Camera()
{
	//duplicate transform and matrices
	m_transform = Transform();
	m_transform.SetParent(nullptr);
	m_transform.SetLocalMatrix(camera->m_transform.GetWorldMatrix());
	//m_projMatrix = camera->m_projMatrix;    
	GetViewMatrix();		//refreshes the view matrix

	//copy camera options and set perspective
	m_clearColor = camera->m_clearColor;
	m_skybox = camera->m_skybox;
	m_fovDegrees = camera->m_fovDegrees;
	
}

void PerspectiveCamera::SetPerspectiveOrtho(float fovDegrees, float aspect, float nearZ, float farZ)
{
	m_fovDegrees = fovDegrees;
	SetProjection(Matrix44::MakePerspectiveProjection(fovDegrees, aspect, nearZ, farZ));
}
