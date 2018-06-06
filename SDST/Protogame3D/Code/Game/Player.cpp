#include "Game/Player.hpp"
#include "Game/Game.hpp"

Player::Player(Vector3 position)
{
	float size = 5.f;
	m_renderable = new Renderable(RENDERABLE_SPHERE, 1.f);
	SetPosition(position);
	SetScale(Vector3(size,size,size));
	Material* mat = Material::GetMaterial("couch");
	if (mat != nullptr){
		SetMaterial(mat);
	}


	m_spinDegreesPerSecond = 45.f;
	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;


	m_rateOfFire = StopWatch();
	m_rateOfFire.SetTimer(.3f);
	m_positionXZ = position.XZ();
}

void Player::Update()
{

	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;
	//m_thrusterSystem->m_emitters[0]->m_transform(m_leftThruster->GetWorldPosition());
	//m_thrusterSystem->m_emitters[1]->m_renderable->SetPosition(m_rightThruster->GetWorldPosition());
	//Translate(GetForward() * ds * m_speed * .5f);

}

void Player::HandleInput()
{
	float ds = g_theGame->GetDeltaSeconds();

	//float degPerSecond = 30.f;
	Vector3 rotation = Vector3::ZERO;

	if (g_theInput->IsKeyDown(VK_UP)){
		rotation.x += 1.f;
	}
	if (g_theInput->IsKeyDown(VK_DOWN)){
		rotation.x -=1.f;
	}
	if (g_theInput->IsKeyDown(VK_RIGHT)){
		rotation.y += 1.f;
	}
	if (g_theInput->IsKeyDown(VK_LEFT)){
		rotation.y -=1.f;
	}

	Vector2 controllerRotation = g_theInput->GetController(0)->GetRightThumbstickCoordinates();
	rotation+=Vector3(controllerRotation.y, controllerRotation.x, 0.f);

	Rotate(rotation * m_degPerSecond * ds);

	Vector2 movement = Vector2::ZERO;


	//why are there 6 keys if there are only 4 directions
	//float speed = 10.f;
	if (g_theInput->IsKeyDown('D')){
		//Translate(GetRight() * ds * m_speed);
		movement+= GetRight().XZ() * ds * m_speed;
	}
	if (g_theInput->IsKeyDown('A')){
		//Translate(GetRight() * ds * m_speed * -1.f);
		movement+= GetRight().XZ() * ds * m_speed * -1.f;
	}
	if (g_theInput->IsKeyDown('W')){
		//Translate(GetForward() * ds* m_speed );
		movement+= GetForward().XZ() * ds * m_speed;
	}
	if (g_theInput->IsKeyDown('S')){
		movement+= GetForward().XZ() * ds * m_speed * -1.f;
	}


	Vector2 controllerTranslation = g_theInput->GetController(0)->GetLeftThumbstickCoordinates();

	movement+= (GetForward().XZ() * controllerTranslation.y * ds * m_speed);
	movement+=(GetRight().XZ() * controllerTranslation.x * ds * m_speed);

	m_positionXZ+=movement;

	SetWorldPosition();


}

void Player::SetWorldPosition()
{
	Vector3 pos = Vector3(m_positionXZ.x, GetHeightAtCurrentPos(), m_positionXZ.y);
}

float Player::GetHeightAtCurrentPos()
{
	return 1.0f;
}

