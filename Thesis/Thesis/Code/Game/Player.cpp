#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"

Player::Player(GameState_Playing* playState, Vector3 position)
{
	float size = 1.f;
	m_collider = Sphere(Vector3::ZERO, size);
	m_renderable = new Renderable();//new Renderable(RENDERABLE_CUBE, 1.f);
	m_turretRenderable = new Renderable();
	//make tank
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(Vector3::ZERO, Vector3(.8f, .3f, 1.f), RGBA::GREEN);
	mb.End();
	m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_LIT));

	//make turret
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	Vector3 sphere = Vector3::ZERO - (Vector3::UP * .08f);
	mb.AppendSphere(sphere, .2f, 10, 10, RGBA::RED);
	//mb.AppendCube(sphere, Vector3::ONE * .5f, RGBA::RED);
	mb.AppendCube(Vector3::FORWARD * .4f, Vector3(.1f, .1f, .8f), RGBA::RED);
	mb.End();
	m_turretRenderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_LIT));
	
	
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

	//set up auxilary transforms: camera target, turret renderable, barrel fire position, laser sight...
	m_cameraTarget = new Transform();
	m_cameraTarget->SetLocalPosition(Vector3::ZERO);
	m_turretRenderable->SetPosition(Vector3::ZERO + GetUp() * .25f);
	m_turretRenderable->m_transform.SetParent(&m_renderable->m_transform);
	m_barrelPosition = new Transform();
	m_barrelPosition->SetParent(&m_turretRenderable->m_transform);
	m_barrelPosition->SetLocalPosition(Vector3(0.f, 0.f, .8f));

	mb.Clear();
	mb.Begin(PRIMITIVE_LINES, false);
	mb.AppendLine(Vector3::ZERO, Vector3::FORWARD * 15.f, RGBA::RED, RGBA(100,0,0,128));
	mb.End();
	m_laserSightRenderable = new Renderable();
	m_laserSightRenderable->SetMesh(mb.CreateMesh());
	m_laserSightRenderable->SetMaterial(Material::GetMaterial("default_alpha"));
	m_laserSightRenderable->m_transform.SetParent(m_barrelPosition);


	m_targetRenderable = new Renderable(RENDERABLE_PLANE, .15f);
	m_targetRenderable->GetEditableMaterial()->SetProperty("TINT", RGBA::RED);

	//m_turretRenderable->m_transform.SetParent(m_cameraTarget);

	m_playState = playState;

	m_health = 50;
	m_maxHealth = m_health;

	SetPosition(position);
	SetScale(Vector3(size,size,size));
}

Player::~Player()
{
	g_theGame->GetScene()->RemoveRenderable(m_laserSightRenderable);
	g_theGame->GetScene()->RemoveRenderable(m_targetRenderable);
	g_theGame->GetScene()->RemoveRenderable(m_turretRenderable);

	delete m_cameraTarget;
	delete m_turretRenderable;
	delete m_laserSightRenderable;
	delete m_targetRenderable;
	delete m_barrelPosition;
}

void Player::Update()
{

	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;
}

void Player::HandleInput()
{
	float ds = g_theGame->GetDeltaSeconds();

	
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
	g_theGame->GetScene()->RemoveRenderable(m_laserSightRenderable);
	g_theGame->GetScene()->RemoveRenderable(m_turretRenderable);
}

void Player::AddRenderable()
{
	g_theGame->GetScene()->AddRenderable(m_renderable);
	g_theGame->GetScene()->AddRenderable(m_laserSightRenderable);
	g_theGame->GetScene()->AddRenderable(m_targetRenderable);
	g_theGame->GetScene()->AddRenderable(m_turretRenderable);
}

float Player::GetPercentageOfHealth() const
{
	float t = RangeMapFloat((float) m_health, 0.f, (float) m_maxHealth, 0.f, 1.f);
	return t;
}



static Vector2 gPos = Vector2(0.0f, 0.0f); 

