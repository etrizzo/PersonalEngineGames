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
	mb.AppendCube(Vector3(0.f, 0.f, 0.f), Vector3::ONE * .5f, RGBA::GREEN, RIGHT, UP, FORWARD);
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


	//m_turretRenderable->m_transform.SetParent(m_cameraTarget);

	m_playState = playState;

	m_health = 50;
	m_maxHealth = m_health;

	m_cameraTarget = new Transform();
	m_cameraTarget->SetParent(&m_renderable->m_transform);

	/*m_shadowCameraTransform = new Transform();
	Vector3 lightEuler = g_theGame->m_playState->m_sun->m_transform.GetEulerAnglesYawPitchRoll();
	Vector3 sunDirection = g_theGame->m_playState->m_sun->m_transform.GetForward();
	m_shadowCameraOffset = sunDirection * -25.f;
	m_shadowCameraTransform->SetRotationEuler(lightEuler);
	m_shadowCameraTransform->SetLocalPosition(position + m_shadowCameraOffset);*/



	SetPosition(position);
	SetScale(Vector3(size,size,size));
}

Player::~Player()
{

}

void Player::Update()
{

	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;

}

void Player::HandleInput()
{
	float ds = g_theGame->GetDeltaSeconds();

	float speed = m_speed;
	if (g_theInput->IsKeyDown(VK_SHIFT)){
		speed *= m_shiftMultiplier;
	}

	float deltaYaw = 0.f;
	float deltaPitch = 0.f;

	Vector2 mouseDirection = g_theInput->GetMouseDirection();
	mouseDirection *= .5f;


	if (g_theInput->IsKeyDown(VK_UP)){
		deltaPitch -= 1.f;			// positive deltaPitch is from z towards x
	}
	if (g_theInput->IsKeyDown(VK_DOWN)){
		deltaPitch +=1.f;
	}
	if (g_theInput->IsKeyDown(VK_RIGHT)){
		deltaYaw -= 1.f;		//positive yaw is from right to left
	}
	if (g_theInput->IsKeyDown(VK_LEFT)){
		deltaYaw +=1.f;		
	}

	deltaPitch += 1.f * mouseDirection.y;		//up and down is  deltaPitch
	deltaYaw += -1.f * mouseDirection.x;				//left and right is yaw

	Vector2 controllerRotation = g_theInput->GetController(0)->GetRightThumbstickCoordinates();
	deltaYaw += -1.f * controllerRotation.x;
	deltaPitch += -1.f * controllerRotation.y;

	/*deltaYaw *= degPerSecond * ds;
	deltaPitch *= degPerSecond * ds;
	*/
	Rotate(deltaYaw, deltaPitch, 0.f);

	Vector3 forward = GetForward();
	Vector3 forwardLockedVertical = Vector3(forward.x, forward.y, 0.f);
	forwardLockedVertical.NormalizeAndGetLength();

	//why are there 6 keys if there are only 4 directions
	if (g_theInput->IsKeyDown('D')){
		Translate(GetRight() * ds * speed);
	}
	if (g_theInput->IsKeyDown('A')){
		Translate(GetRight() * ds * speed * -1.f);
	}
	if (g_theInput->IsKeyDown('W')){
		Translate(forwardLockedVertical * ds* speed );
	}
	if (g_theInput->IsKeyDown('S')){
		Translate(forwardLockedVertical * ds * speed * -1.f);
	}
	if (g_theInput->IsKeyDown('E') || g_theInput->GetController(0)->IsButtonDown(XBOX_BUMPER_RIGHT)){
		Translate(UP * ds* speed );		//going off of camera's up feels very weird when it's not perfectly upright
	}
	if (g_theInput->IsKeyDown('Q') || g_theInput->GetController(0)->IsButtonDown(XBOX_BUMPER_LEFT)){
		Translate(UP * ds * speed * -1.f);
	}

	Vector2 controllerTranslation = g_theInput->GetController(0)->GetLeftThumbstickCoordinates();

	Translate(forwardLockedVertical * controllerTranslation.y * ds * speed);
	Translate(GetRight() * controllerTranslation.x * ds * speed);



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


