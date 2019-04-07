#include "Game/GameCamera.hpp"
#include "Game/Entity.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Game.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/World.hpp"

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
	case CAMERA_MODE_FIXEDANGLE:
		UpdateFixedAngle();
		break;
	}
}

std::string GameCamera::GetModeString()
{
	switch (m_mode)
	{
	case CAMERA_MODE_FIRSTPERSON:
		return "First Person";
	case CAMERA_MODE_THIRDPERSON:
		return "Third Person";
	case CAMERA_MODE_FIXEDANGLE:
		return "Fixed Angle";
	case CAMERA_MODE_SPECTATOR:
		return "Spectator";
	}
	return "NO_CAMERA_MODE";
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

	InitForNewMode();
	
}

void GameCamera::SetMode(eCameraMode newMode)
{
	FinishMode();
	m_mode = newMode;

	InitForNewMode();
}

void GameCamera::UpdateFirstPerson()
{

}

void GameCamera::UpdateThirdPerson()
{
	RaycastResult hit = g_theGame->GetWorld()->Raycast(m_myDaddy->GetPosition(), -1.f * m_myDaddy->GetForward(), THIRD_PERSON_ARM_LENGTH);
	m_camera->m_transform.SetLocalPosition(Vector3(-hit.m_impactDistance, 0.f, 0.f));
}

void GameCamera::UpdateFixedAngle()
{
	Vector3 offset = m_camera->GetForward() * 10.f;
	m_camera->SetPosition(m_myDaddy->GetPosition() - offset);
}

void GameCamera::FinishMode()
{
	switch (m_mode)
	{
	case (CAMERA_MODE_FIRSTPERSON):

		break;
	case (CAMERA_MODE_THIRDPERSON):
	
		break;
	case (CAMERA_MODE_FIXEDANGLE):
		
		break;
	case (CAMERA_MODE_SPECTATOR):
		g_theGame->m_debugRenderSystem->ReattachCamera();
		break;
	}
}

void GameCamera::InitForNewMode()
{
	switch (m_mode)
	{
	case (CAMERA_MODE_FIRSTPERSON):
		m_camera->m_transform.SetLocalMatrix(Matrix44::IDENTITY);
		m_camera->m_transform.SetParent(&m_myDaddy->m_transform);
		m_camera->m_transform.SetLocalPosition(Vector3(0.0f, 0.f, 0.f));
		break;
	case (CAMERA_MODE_THIRDPERSON):
		m_camera->m_transform.SetLocalMatrix(Matrix44::IDENTITY);
		m_camera->m_transform.SetParent(&m_myDaddy->m_transform);
		m_camera->m_transform.SetLocalPosition(Vector3(-THIRD_PERSON_ARM_LENGTH, 0.f, 0.f));
		break;
	case(CAMERA_MODE_FIXEDANGLE):
		m_camera->m_transform.SetLocalMatrix(Matrix44::IDENTITY);
		m_camera->m_transform.SetParent(nullptr);
		m_camera->Rotate(40.f, 30.f, 0.0f);
		break;
	case (CAMERA_MODE_SPECTATOR):
		g_theGame->m_debugRenderSystem->DetachCamera();
		break;
	}
}

