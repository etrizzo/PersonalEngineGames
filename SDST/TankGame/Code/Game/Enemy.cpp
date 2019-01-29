#include "Enemy.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/DebugRenderSystem.hpp"

Enemy::Enemy(Vector2 startPos, GameState_Playing * playState)
{
	m_renderable = new Renderable();
	m_collider = Sphere(Vector3::ZERO, .5f, 10, 10);
	//make the sphere
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendSphere(Vector3::ZERO, .5f, 10, 10, RGBA::YELLOW);
	mb.End();
	m_renderable->SetMesh(mb.CreateSubMesh(VERTEX_TYPE_LIT), 0);
	SetMaterial(Material::GetMaterial("cel_shaded_lit"), 0);

	//make the plane
	mb.Clear();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendPlane(Vector3::FORWARD *.5f, Vector3::UP, -1.f * Vector3::RIGHT, Vector2::HALF, RGBA::WHITE, Vector2::ZERO, Vector2::ONE);
	mb.End();
	m_renderable->SetMesh(mb.CreateSubMesh(VERTEX_TYPE_LIT), 1);
	Material* mat = Material::GetMaterial("minion");
	if (mat != nullptr){
		SetMaterial(mat, 1);
	}

	m_positionXZ = startPos;
	m_playState = playState;
	Vector3 pos = Vector3(startPos.x, GetHeightAtCurrentPos(), startPos.y);

	SetPosition(pos);
}

Enemy::~Enemy()
{
	m_playState->m_scene->RemoveRenderable(m_renderable);
}

void Enemy::Update()
{
	PROFILE_PUSH_FUNCTION();
	if (!m_aboutToBeDeleted){
		float ds = g_theGame->GetDeltaSeconds();
		
		m_ageInSeconds+=ds;
		
		//Vector2 playerPos = g_theGame->GetPlayerPositionXZ();
		//Vector2 direction = playerPos - m_positionXZ;
		//direction.NormalizeAndGetLength();
		UpdateDirection();
		m_positionXZ += m_direction * ds * m_speed;
	
		TurnTowardPlayer();
		SetWorldPosition();
		CheckForPlayerCollision();

		//g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.f, m_collider.m_center, m_collider.m_radius, 10, 10, RGBA::RED, RGBA::RED, DEBUG_RENDER_IGNORE_DEPTH);
	}
	PROFILE_POP();
}

void Enemy::RunEntityPhysics()
{
}

void Enemy::Damage()
{
	m_health--;
	if (m_health <= 0){
		g_theAudio->PlayOneOffSoundFromGroup("minion");
		m_aboutToBeDeleted = true;
	}
}

bool Enemy::IsPointInside(const Vector3 & point) const
{
	return m_collider.IsPointInside(point);
}

void Enemy::CheckForPlayerCollision()
{
	if (m_collider.DoSpheresOverlap(g_theGame->m_playState->m_player->m_collider)){
		Damage();
		//damage the player
		g_theGame->m_playState->m_player->Damage();
	}
}

void Enemy::UpdateDirection()
{
	//get the average of all weighted factors
	m_direction = GetSeekDirection() + GetSeparateDirection() + GetAlignmentDirection() + GetCohesionDirection();
	m_direction *= .25f;	
}

void Enemy::TurnTowardPlayer()
{
	Vector3 playerPos = g_theGame->GetPlayer()->GetPosition();
	Matrix44 targetTransform = Matrix44::LookAt(m_renderable->m_transform.GetLocalPosition(), playerPos);
	Matrix44 currentTransform = m_renderable->m_transform.GetLocalMatrix();

	float turnThisFrame = m_degPerSecond * g_theGame->GetDeltaSeconds();
	Matrix44 localMat = TurnToward(currentTransform, targetTransform, turnThisFrame);
	m_renderable->m_transform.SetLocalMatrix(localMat);

	//g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(GetPosition(), playerPos, RGBA::CYAN);
	//g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, GetPosition(), 1.f, m_renderable->m_transform.GetWorldMatrix());
}

void Enemy::SetWorldPosition()
{
	Vector3 pos = Vector3(m_positionXZ.x, GetHeightAtCurrentPos() + .3f, m_positionXZ.y);

	Vector3 normal = g_theGame->m_currentMap->GetNormalAtPosition(m_positionXZ);

	SetPosition(pos);
}

float Enemy::GetHeightAtCurrentPos()
{
	return g_theGame->m_currentMap->GetHeightAtCoord(m_positionXZ);
}

Vector2 Enemy::GetSeekDirection()
{
	return GetForward().XZ() * g_theGame->m_enemySeekWeight;		//z direction, the forward
}

Vector2 Enemy::GetSeparateDirection()
{
	Vector2 dir = Vector2::ZERO;
	int numToAverage = 0;
	for(Enemy* enemy : m_playState->m_enemies){
		if (enemy != this){
			Vector2 displacement = m_positionXZ - enemy->m_positionXZ;
			float distance = displacement.NormalizeAndGetLength();
			if (distance < g_theGame->m_enemySeparateRadius){
				dir = dir + (displacement.GetNormalized());
				numToAverage++;
			}
		}
	}
	if (numToAverage > 0){
		dir = dir * (1.f / (float) numToAverage);
	}

	return dir * g_theGame->m_enemySeparateWeight;
}

Vector2 Enemy::GetCohesionDirection()
{
	Vector2 pos = Vector2::ZERO;
	int numToAverage = 0;
	for(Enemy* enemy : m_playState->m_enemies){
		if (enemy != this){
			Vector2 displacement = m_positionXZ - enemy->m_positionXZ;
			float distance = displacement.NormalizeAndGetLength();
			if (distance < g_theGame->m_enemyCohesionRadius){
				pos = pos + (enemy->m_positionXZ);
				numToAverage++;
			}
		}
	}
	if (numToAverage > 0){
		pos = pos * (1.f / (float) numToAverage);
	}
	Vector2 dir = pos - m_positionXZ;
	dir.NormalizeAndGetLength();
	return dir * g_theGame->m_enemyCohesionWeight;
}

Vector2 Enemy::GetAlignmentDirection()
{
	Vector2 dir = Vector2::ZERO;
	int numToAverage = 0;
	for(Enemy* enemy : m_playState->m_enemies){
		if (enemy != this){
			Vector2 displacement = m_positionXZ - enemy->m_positionXZ;
			float distance = displacement.NormalizeAndGetLength();
			if (distance < g_theGame->m_enemyAlignmentRadius){
				dir = dir + enemy->m_direction;
				numToAverage++;
			}
		}
	}
	if (numToAverage > 0){
		dir = dir * (1.f / (float) numToAverage);
	}

	return dir * g_theGame->m_enemyAlignmentWeight;
}
