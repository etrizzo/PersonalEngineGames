#include "Game/GameCamera.hpp"
#include "Game/Entity.hpp"
#include "Engine/Renderer/Camera.hpp"

GameCamera::GameCamera(Camera* camera, Entity* owner)
{
	m_camera = camera;
	SetOwner(owner);
}

void GameCamera::Update()
{
	switch (m_mode)
	{
	case CAMERA_MODE_FIRSTPERSON:
		UpdateFirstPerson();
		break;
	case CAMERA_MODE_THIRDPERSON:
		UpdateThirdPerson();
		break;
	}
}

void GameCamera::SetOwner(Entity* owner)
{
	//clear the current owner's camera ownership status
	if (owner != nullptr)
	{
		owner->RemoveGameCamera();
	}
	m_myDaddy = owner;
	m_myDaddy->GiveGameCamera(this);

	switch (m_mode)
	{
	case (CAMERA_MODE_FIRSTPERSON):
		m_camera->m_transform.SetParent(&m_myDaddy->m_transform);
		m_camera->m_transform.SetLocalPosition(Vector3(0.0f, 0.f,0.f));
		break;
	case (CAMERA_MODE_THIRDPERSON):
		m_camera->m_transform.SetParent(&m_myDaddy->m_transform);
		m_camera->m_transform.SetLocalPosition(Vector3(-THIRD_PERSON_ARM_LENGTH, 0.f,0.f));
		break;
	}
	
}

void GameCamera::SetMode(eCameraMode newMode)
{
	m_mode = newMode;

	switch (m_mode)
	{
	case (CAMERA_MODE_FIRSTPERSON):
		m_camera->m_transform.SetParent(&m_myDaddy->m_transform);
		m_camera->m_transform.SetLocalPosition(Vector3(0.0f, 0.f,0.f));
		break;
	case (CAMERA_MODE_THIRDPERSON):
		m_camera->m_transform.SetParent(&m_myDaddy->m_transform);
		m_camera->m_transform.SetLocalPosition(Vector3(-THIRD_PERSON_ARM_LENGTH, 0.f,0.f));
		break;
	}
}

void GameCamera::UpdateFirstPerson()
{

}

void GameCamera::UpdateThirdPerson()
{

}

