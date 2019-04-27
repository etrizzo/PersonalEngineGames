#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Engine/Renderer/Sprite.hpp"
#include "Game/FlowerPot.hpp"

Player::Player(GameState_Playing* playState, Vector3 position)
{
	float size = 1.f;
	m_collider = Sphere(Vector3::ZERO, size);


	m_sprite = new Sprite(g_theRenderer->CreateOrGetTexture("TestTexture.png"), AABB2::ZERO_TO_ONE, Vector2(.5f,0.f), Vector2::ONE);


	m_spinDegreesPerSecond = 45.f;
	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;


	m_rateOfAttack = StopWatch();
	m_rateOfAttack.SetTimer(.3f);

	m_positionXY = position.XZ();

	//set up auxilary transforms: camera target, turret renderable, barrel fire position, laser sight...
	m_cameraTarget = new Transform();
	m_cameraTarget->SetLocalPosition(Vector3::ZERO);

	//m_turretRenderable->m_transform.SetParent(m_cameraTarget);

	m_playState = playState;

	m_health = PLAYER_MAX_HEALTH;

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

	RecoverHealth();
	
	SetWorldPosition();

	m_cameraTarget->SetLocalPosition(GetPosition() + GetUp() * .25f);
}

void Player::HandleInput()
{
	HandleMovementInput();
	HandleActionInput();

}

void Player::Damage()
{
	if (!g_theGame->m_godMode){
		m_health-=1.f;
		if (m_health <= 0.f){
			m_aboutToBeDeleted = true;
		}
	}
}

void Player::Respawn()
{
	m_aboutToBeDeleted = false;
	m_health = PLAYER_MAX_HEALTH;
}


float Player::GetPercentageOfHealth() const
{
	float t = RangeMapFloat( m_health, 0.f, PLAYER_MAX_HEALTH, 0.f, 1.f);
	return t;
}



static Vector2 gPos = Vector2(0.0f, 0.0f); 

void Player::HandleMovementInput()
{
	float ds = g_theGame->GetDeltaSeconds();
	Vector3 camRotation = Vector3::ZERO;

	if (g_theInput->IsKeyDown(VK_UP)) {
		camRotation.x += 1.f;
	}
	if (g_theInput->IsKeyDown(VK_DOWN)) {
		camRotation.x -= 1.f;
	}
	if (g_theInput->IsKeyDown(VK_RIGHT)) {
		camRotation.y += 1.f;
	}
	if (g_theInput->IsKeyDown(VK_LEFT)) {
		camRotation.y -= 1.f;
	}

	Vector2 controllerRotation = g_theInput->GetController(0)->GetRightThumbstickCoordinates();
	camRotation += Vector3(controllerRotation.y, controllerRotation.x, 0.f);

	Vector2 mouseMovement = g_theInput->GetMouseDirection()  * .2f;
	camRotation += Vector3(-mouseMovement.y, mouseMovement.x, 0.f);

	m_cameraTarget->RotateByEuler(camRotation * m_cameraDegPerSeconds * ds);


	float movement = 0.0f;


	//Move left and right
	if (g_theInput->IsKeyDown('D')) {
		movement += ds * m_speed;
	}
	if (g_theInput->IsKeyDown('A')) {
		movement += ds * m_speed * -1.f;
	}


	Vector2 controllerTranslation = g_theInput->GetController(0)->GetLeftThumbstickCoordinates();

	movement += (controllerTranslation.x * ds * m_speed);

	float newX = m_positionXY.x + movement;
	float radius = g_theGame->m_currentMap->m_collider.m_radius;
	m_positionXY.x = ClampFloat(newX, -radius * WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS, radius * WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS);

	m_positionXY.y = GetHeightAtCurrentPos();


}

void Player::HandleActionInput()
{
	if (g_theInput->IsKeyDown(VK_SPACE) || g_theInput->IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		if (m_rateOfAttack.CheckAndReset()) {
			g_theAudio->PlayOneOffSoundFromGroup("laser1");
		}
	}

	if ( g_theInput->WasKeyJustPressed('R') || g_theInput->WasMouseButtonJustPressed(MOUSE_BUTTON_RIGHT)) {
		TryToLoadCannon();
	}
}

void Player::TryToLoadCannon()
{
	if (m_health > RELOAD_HEALTH_COST)
	{
		//FlowerPot* reload = nullptr;
		for (FlowerPot* pot : g_theGame->m_playState->m_flowerPots)
		{
			if (pot->m_collider.IsPointInside(GetPosition()))
			{
				pot->Reload();
				m_health -= RELOAD_HEALTH_COST;
				break;
			}
		}
	}
	else
	{
		ConsolePrintf(RGBA::RED, "Not enough health");
	}
}

void Player::RecoverHealth()
{
	if (m_health < PLAYER_MAX_HEALTH)
	{
		m_health += m_regenerationSpeed * GetMasterClock()->GetDeltaSeconds();
	}
}

void Player::SetWorldPosition()
{

	m_worldPos = Vector3(m_positionXY.x, GetHeightAtCurrentPos(), m_worldPos.z);
	m_collider.SetPosition(m_worldPos);


	m_shadowCameraTransform->SetLocalPosition(m_worldPos + m_shadowCameraOffset);

	m_transform.SetLocalPosition(m_worldPos);
	//SetPosition(pos);
}

void Player::MoveTurretTowardTarget()
{
	////m_turretRenderable->m_transform.SetRotationEuler(Vector3::ZERO);
	//Transform* base = m_turretRenderable->m_transform.GetParent();
	//Vector3 localPos = base->WorldToLocal(m_target);
	////m_turretRenderable->m_transform.LocalLookAt(localPos);

	//Matrix44 targetTransform = Matrix44::LookAt(m_turretRenderable->m_transform.GetLocalPosition(), localPos);
	//Matrix44 currentTransform = m_turretRenderable->m_transform.GetLocalMatrix();
	////currentTransform.RotateDegrees3D(Vector3(0.f, 60.f, 0.f));

	//float turnThisFrame = m_degPerSecond * g_theGame->GetDeltaSeconds();
	//Matrix44 localMat = TurnToward(currentTransform, targetTransform, turnThisFrame);
	//m_turretRenderable->m_transform.SetLocalMatrix(localMat);
	//
	////g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, m_turretRenderable->m_transform.GetWorldPosition(), 1.f, m_turretRenderable->m_transform.GetWorldMatrix(), RGBA::YELLOW);

}


float Player::GetHeightAtCurrentPos()
{
	return g_theGame->m_currentMap->GetHeightFromXPosition(m_positionXY.x);
}

