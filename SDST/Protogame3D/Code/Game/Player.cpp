#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"

Player::Player(GameState_Playing* playState, Vector3 position)
{
	float size = 1.f;
	m_collider = Sphere(position, size);
	m_renderable = new Renderable();//new Renderable(RENDERABLE_CUBE, 1.f);
	m_turretRenderable = new Renderable();
	//make tank
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(position, Vector3(.8f, .3f, 1.f), RGBA::GREEN);
	mb.End();
	m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_LIT));

	//make turret
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	Vector3 sphere = position - (Vector3::UP * .08f);
	mb.AppendSphere(sphere, .2f, 10, 10, RGBA::RED);
	mb.AppendCube(position + Vector3::FORWARD * .4f, Vector3(.1f, .1f, .8f), RGBA::RED);
	mb.End();
	m_turretRenderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_LIT));
	
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

	//set up auxilary transforms: camera target, turret renderable, barrel fire position, laser sight...
	m_cameraTarget = new Transform();
	m_cameraTarget->SetLocalPosition(GetPosition());
	m_turretRenderable->SetPosition(GetPosition() + GetUp() * .25f);
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
	m_laserSightRenderable->SetMaterial(Material::GetMaterial("default_unlit"));
	m_laserSightRenderable->m_transform.SetParent(m_barrelPosition);


	m_targetRenderable = new Renderable(RENDERABLE_PLANE, .15f);
	m_targetRenderable->GetEditableMaterial()->SetProperty("TINT", RGBA::RED);

	//m_turretRenderable->m_transform.SetParent(m_cameraTarget);

	m_playState = playState;
}

void Player::Update()
{

	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;
	//m_thrusterSystem->m_emitters[0]->m_transform(m_leftThruster->GetWorldPosition());
	//m_thrusterSystem->m_emitters[1]->m_renderable->SetPosition(m_rightThruster->GetWorldPosition());
	//Translate(GetForward() * ds * m_speed * .5f);
	SetWorldPosition();
	//if (m_rateOfFire.CheckAndReset()){
	//	g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(1.f, GetPosition());
	//}
	UpdateTarget();
	
	m_cameraTarget->SetLocalPosition(GetPosition() + GetUp() * .25f);
	MoveTurretTowardTarget();
	//g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.f, m_collider.m_center, m_collider.m_radius, m_collider.m_slices, m_collider.m_wedges, RGBA::RED, RGBA::RED, DEBUG_RENDER_IGNORE_DEPTH);
	//g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, m_barrelPosition->GetWorldPosition(), .5f, m_barrelPosition->GetWorldMatrix());
	//g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, GetPosition(), 1.5f, m_renderable->m_transform.GetWorldMatrix());
	//g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, m_turretRenderable->GetPosition(), 1.f, m_turretRenderable->m_transform.GetWorldMatrix());
}

void Player::HandleInput()
{
	float ds = g_theGame->GetDeltaSeconds();

	if (g_theInput->IsKeyDown(VK_SPACE) || g_theInput->IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
		if (m_rateOfFire.CheckAndReset()){
			m_playState->AddNewBullet(*m_barrelPosition);
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

	Vector2 mouseMovement = g_theInput->GetMouseDirection()  * .3f;
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

	m_positionXZ = ClampVector2(m_positionXZ, g_theGame->m_currentMap->m_extents.mins, g_theGame->m_currentMap->m_extents.maxs);
	


}



static Vector2 gPos = Vector2(0.0f, 0.0f); 

void Player::SetWorldPosition()
{

	Vector3 pos = Vector3(m_positionXZ.x, GetHeightAtCurrentPos() + .3f, m_positionXZ.y);

	Vector3 normal = g_theGame->m_currentMap->GetNormalAtPosition(m_positionXZ);
	

	//maintaining Forward:
	Vector3 forward = GetForward(); 
	Vector3 newUp = normal.GetNormalized();
	Vector3 newRight = Cross(newUp, GetForward()).GetNormalized();
	Vector3 newForward = Cross(newRight, newUp).GetNormalized();

	Matrix44 mat = Matrix44(newRight, newUp, newForward, pos);

	m_renderable->m_transform.SetLocalMatrix(mat);
	m_collider.SetPosition(pos);
	//SetPosition(pos);
}

void Player::MoveTurretTowardTarget()
{
	//m_turretRenderable->m_transform.SetRotationEuler(Vector3::ZERO);
	Transform* base = m_turretRenderable->m_transform.GetParent();
	Vector3 localPos = base->WorldToLocal(m_target);
	//m_turretRenderable->m_transform.LocalLookAt(localPos);

	Matrix44 targetTransform = Matrix44::LookAt(m_turretRenderable->m_transform.GetLocalPosition(), localPos);
	Matrix44 currentTransform = m_turretRenderable->m_transform.GetLocalMatrix();
	//currentTransform.RotateDegrees3D(Vector3(0.f, 60.f, 0.f));

	float turnThisFrame = m_degPerSecond * g_theGame->GetDeltaSeconds() * .8f;
	Matrix44 localMat = TurnToward(currentTransform, targetTransform, turnThisFrame);
	m_turretRenderable->m_transform.SetLocalMatrix(localMat);
	
	//g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, m_turretRenderable->m_transform.GetWorldPosition(), 1.f, m_turretRenderable->m_transform.GetWorldMatrix(), RGBA::YELLOW);

}

void Player::UpdateTarget()
{
	//ray cast against the world from camera forward
	Ray3D ray = Ray3D(g_theGame->m_mainCamera->GetPosition(), g_theGame->m_mainCamera->GetForward() );
	
	Contact3D contact;
	if (g_theGame->m_currentMap->Raycast(contact, 1, ray, 1000.f)) {
		//if we hit something, update target
		m_target = contact.m_position;
		//g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(m_barrelPosition->GetWorldPosition(), m_target, RGBA::RED.GetColorWithAlpha(128), RGBA::RED.GetColorWithAlpha(128));
		g_theGame->m_debugRenderSystem->MakeDebugRenderQuad(0.f, m_target, Vector2::HALF * .2f, g_theGame->GetCurrentCameraRight(), g_theGame->GetCurrentCameraUp(), RGBA::RED, RGBA::RED, DEBUG_RENDER_IGNORE_DEPTH);
		//g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.f, m_target, .1f);
		//g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(m_target, m_target + contact.m_normal, RGBA::RED, RGBA::YELLOW);
	} else {
		m_target = ray.Evaluate(1000.f);
		//g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(m_cameraTarget->GetWorldPosition(), m_target, RGBA::RED, RGBA::RED);
	}
}

float Player::GetHeightAtCurrentPos()
{
	return g_theGame->m_currentMap->GetHeightAtCoord(m_positionXZ);
}

