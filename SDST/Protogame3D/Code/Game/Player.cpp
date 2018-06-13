#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/DebugRenderSystem.hpp"

Player::Player(Vector3 position)
{
	float size = 1.f;
	m_renderable = new Renderable();//new Renderable(RENDERABLE_CUBE, 1.f);
	m_turretRenderable = new Renderable();
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(position, Vector3(1.f, .3f, 1.f), RGBA::GREEN);
	mb.End();
	m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_LIT));

	mb.Begin(PRIMITIVE_TRIANGLES, true);
	Vector3 sphere = position; //+ Vector3::UP * .5f;
	mb.AppendSphere(sphere, .2f, 10, 10, RGBA::RED);
	mb.AppendCube(sphere + Vector3::FORWARD * .5f, Vector3(.1f, .1f, 1.f), RGBA::RED);
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
	m_cameraTarget = new Transform();
	m_cameraTarget->SetLocalPosition(GetPosition());
	m_turretRenderable->SetPosition(GetPosition() + GetUp() * .25f);
	m_turretRenderable->m_transform.SetParent(&m_renderable->m_transform);
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
	m_cameraTarget->SetLocalPosition(GetPosition() + GetUp() * .25f);
	MoveTurretTowardTarget();
	//g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, GetPosition(), 1.5f, m_renderable->m_transform.GetWorldMatrix());
	g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, m_turretRenderable->GetPosition(), 1.f, m_turretRenderable->m_transform.GetWorldMatrix());
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

void Player::SetWorldPosition()
{
	Vector3 pos = Vector3(m_positionXZ.x, GetHeightAtCurrentPos() + .3f, m_positionXZ.y);
	Vector3 normal = g_theGame->m_currentMap->GetNormalAtTile(m_positionXZ);
	Matrix44 mat = Matrix44::IDENTITY;
	mat.SetI(Vector4(Cross(normal, GetForward()).GetNormalized()));
	mat.SetJ(Vector4(normal));
	mat.SetK(Vector4(GetForward()));
	mat.SetT(Vector4(pos));

	m_renderable->m_transform.SetLocalMatrix(mat);
	//SetPosition(pos);
}

void Player::MoveTurretTowardTarget()
{
	//Transform* base = m_turretRenderable->m_transform.GetParent();
	//Vector3 Local_pos = base->WorldToLocal(m_target);
	//m_turretRenderable->m_transform.LocalLookAt(localPos);
}

float Player::GetHeightAtCurrentPos()
{
	return g_theGame->m_currentMap->GetHeightAtCoord(m_positionXZ);
}

