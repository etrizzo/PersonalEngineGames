#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"

Player::Player(GameState_Playing* playState, Vector3 position)
{
	float size = 1.f;
	m_collider = Sphere(Vector3::ZERO, size);
	m_renderable = new Renderable();//new Renderable(RENDERABLE_CUBE, 1.f);
	//make tank
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(Vector3::ZERO, Vector3(.8f, .3f, 1.f), RGBA::GREEN, RIGHT, UP, FORWARD);
	mb.End();
	m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_LIT));
	
	Material* mat = Material::GetMaterial("couch_cel");
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

	//set up auxilary transforms: camera target, turret renderable, barrel fire position, laser sight...
	m_cameraTarget = new Transform();
	m_cameraTarget->SetLocalPosition(Vector3::ZERO);


	//m_turretRenderable->m_transform.SetParent(m_cameraTarget);

	m_playState = playState;

	m_health = 50;
	m_maxHealth = m_health;

	m_shadowCameraTransform = new Transform();
	Vector3 lightEuler = g_theGame->m_playState->m_sun->m_transform.GetEulerAnglesYawPitchRoll();
	Vector3 sunDirection = g_theGame->m_playState->m_sun->m_transform.GetForward();
	m_shadowCameraOffset = sunDirection * -25.f;
	m_shadowCameraTransform->SetRotationEuler(lightEuler);
	m_shadowCameraTransform->SetLocalPosition(position + m_shadowCameraOffset);



	SetPosition(position);
	SetScale(Vector3(size,size,size));
}

Player::~Player()
{
	delete m_cameraTarget;

}

void Player::Update()
{

	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;

}

void Player::HandleInput()
{
	float ds = g_theGame->GetDeltaSeconds();

	if (g_theInput->IsKeyDown(VK_SPACE) || g_theInput->IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
		if (m_rateOfFire.CheckAndReset()){
			g_theAudio->PlayOneOffSoundFromGroup("laser1");
		}
	}

	Vector3 camRotation = Vector3::ZERO;

	if (g_theInput->IsKeyDown(VK_UP)){
		camRotation.x += 1.f;
	}
	if (g_theInput->IsKeyDown(VK_DOWN)){
		camRotation.x -=1.f;
	}
	if (g_theInput->IsKeyDown(VK_RIGHT)){
		camRotation.y += 1.f;
	}
	if (g_theInput->IsKeyDown(VK_LEFT)){
		camRotation.y -=1.f;
	}

	Vector2 controllerRotation = g_theInput->GetController(0)->GetRightThumbstickCoordinates();
	camRotation+=Vector3(controllerRotation.y, controllerRotation.x, 0.f);

	Vector2 mouseMovement = g_theInput->GetMouseDirection()  * .2f;
	camRotation+=Vector3(-mouseMovement.y, mouseMovement.x, 0.f);

	m_cameraTarget->RotateByEuler(camRotation * m_degPerSecond * ds);

	Vector3 tankRotation = Vector3::ZERO;

	if (g_theInput->IsKeyDown('E')){
		tankRotation.y += 1.f;
	}
	if (g_theInput->IsKeyDown('Q')){
		tankRotation.y -=1.f;
	}

	m_renderable->m_transform.RotateByEuler(tankRotation * m_degPerSecond * ds);

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



}

void Player::Damage()
{
	if (!g_theGame->m_godMode){
		m_health--;
		if (m_health <= 0){
			m_aboutToBeDeleted = true;
		}
	}
}

void Player::Respawn()
{
	AddRenderable();
	m_aboutToBeDeleted = false;
	m_health = m_maxHealth;
}

void Player::RemoveRenderable()
{
	g_theGame->GetScene()->RemoveRenderable(m_renderable);
}

void Player::AddRenderable()
{
	g_theGame->GetScene()->AddRenderable(m_renderable);
}

float Player::GetPercentageOfHealth() const
{
	float t = RangeMapFloat((float) m_health, 0.f, (float) m_maxHealth, 0.f, 1.f);
	return t;
}


