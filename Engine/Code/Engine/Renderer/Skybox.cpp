#include "Engine/Renderer/Skybox.hpp"
#include "Engine/Renderer/Camera.hpp"

Skybox::Skybox(Camera * cam, const char* fileName)
	:m_texCube(fileName)
{
	m_mesh = CreateCubeMesh(Vector3::ZERO, Vector3::ONE, RGBA::WHITE);
	m_transform = Transform();
	m_transform.SetLocalPosition(cam->GetPosition());
	m_transform.RotateByEuler(Vector3(180.f,0.f,0.f));
}

void Skybox::Update(Camera* cam)
{
	//m_transform = cam->m_t
	m_transform.SetLocalPosition(cam->GetPosition());
}

