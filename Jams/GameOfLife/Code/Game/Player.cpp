#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/DebugRenderSystem.hpp"

Player::Player(Vector2 position)
{
	float size = 1.f;
	m_renderable = new Renderable2D(RENDERABLE_PLANE, Vector2::ONE);
	SetPosition(Vector3(position, 0.f));
	SetScale(Vector3(size,size,size));
	Material* mat = Material::GetMaterial("test");
	if (mat != nullptr){
		SetMaterial(mat);
	}


	m_spinDegreesPerSecond = 45.f;
	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;


	m_rateOfFire = StopWatch();
	m_rateOfFire.SetTimer(.3f);
	m_position = position;

}

void Player::Update()
{

	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;
}

void Player::HandleInput()
{
	float ds = g_theGame->GetDeltaSeconds();

	Vector2 movement = Vector2::ZERO;
	float rotation = 0.f;

	if (IsUpDown()){
		movement.y += 1.f;
	}
	if (IsDownDown()){
		movement.y -=1.f;
	}
	if (IsRightDown()){
		movement.x += 1.f;
	}
	if (IsLeftDown()){
		movement.x -=1.f;
	}


	if (g_theInput->IsKeyDown('E')){
		rotation-=1.f;
	}
	if (g_theInput->IsKeyDown('Q')){
		rotation+=1.f;
	}

	Vector2 controllerRotation = g_theInput->GetController(0)->GetRightThumbstickCoordinates();
	rotation+= controllerRotation.x;

	rotation*= ds * m_degPerSecond;

	Rotate(Vector3(0.f, 0.f, rotation * m_degPerSecond * ds));


	Vector2 controllerTranslation = g_theInput->GetController(0)->GetLeftThumbstickCoordinates();

	//movement+= (GetForward().XZ() * controllerTranslation.y * ds * m_speed);
	movement+=controllerTranslation;
	movement = movement * ds * m_speed;

	m_position+=movement;

	m_renderable->m_transform.TranslateLocal(movement);
	m_renderable->m_transform.RotateByEuler(rotation);


}
