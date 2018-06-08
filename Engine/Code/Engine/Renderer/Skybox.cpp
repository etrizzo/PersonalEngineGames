#include "Engine/Renderer/Skybox.hpp"
#include "Engine/Renderer/Camera.hpp"

Skybox::Skybox( const char* fileName)
	:m_texCube(fileName)
{
	m_mesh = CreateCubeMesh(Vector3::ZERO, Vector3::ONE, RGBA::WHITE);
	m_transform = Transform();
	//m_transform.RotateByEuler(Vector3(180.f,0.f,0.f));	//flips the skybox but faster than by pixel
}

void Skybox::Update()
{
	//cool effects go here someday	
	/*float ds = GetMasterClock()->GetDeltaSeconds();
	m_transform.RotateByEuler(Vector3(0.f,0.f,20.f) * ds);*/
	
}

