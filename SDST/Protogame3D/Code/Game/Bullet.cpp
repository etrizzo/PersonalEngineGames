#include "Bullet.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/Map.hpp"
#include "Game/GameState_Playing.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/Spawner.hpp"


Bullet::Bullet(const Transform& t, GameState_Playing* playState)
{
	MeshBuilder mb;
	Vector3 upright = (Vector3::UP + Vector3::RIGHT).GetNormalized();
	Vector3 leftRight = (Vector3::UP + (-1.f * Vector3::RIGHT)).GetNormalized();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendPlane(Vector3::ZERO, upright, Vector3::FORWARD, Vector2(.55f,.1f), m_tint, Vector2::ZERO, Vector2::ONE);
	mb.AppendPlane(Vector3::ZERO, leftRight, Vector3::FORWARD, Vector2(.55f,.1f), m_tint, Vector2::ZERO, Vector2::ONE);
	mb.End();

	m_renderable = new Renderable();
	m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_3DPCU));
	//m_renderable->m_mesh->SetSubMesh(mb.CreateMesh())

	m_renderable->m_transform.SetLocalMatrix(t.GetWorldMatrix());
	//Rotate(Vector3(0.f,0.f,45.f));
	//SetPosition(t.GetWorldPosition());
	Material* mat = Material::GetMaterial("additive");
	if (mat != nullptr){
		SetMaterial(mat);
	}
	//m_renderable->m_transform = t;
	m_playState = playState;
	m_light = m_playState->AddNewPointLight(Vector3::ZERO, m_tint);
	m_light->m_transform.SetParent(&m_renderable->m_transform);
}

Bullet::~Bullet()
{
	//Destroy();
}

void Bullet::Update()
{
	if (!m_aboutToBeDeleted){
		Entity::Update();
		if (m_lifeTime <= m_ageInSeconds){
			Destroy();
		} else {
			Vector3 pos = GetPosition();
			Vector3 forward = GetForward();
			float ds = g_theGame->GetDeltaSeconds();
			Vector3 newPos  =pos + (forward * (m_speed * ds));
			SetPosition(newPos);
			CheckPhysics();
			//Translate(GetForward() * m_speed * g_theGame->GetDeltaSeconds());
			//g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.f, pos, .5f);
			//g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.f, m_light->GetPosition(), RGBA::RED, RGBA::RED);
		}
	}
}

void Bullet::CheckPhysics()
{
	CheckAgainstEnemies();
	if (!m_aboutToBeDeleted){
		if (!IsAboveTerrain()){
			Destroy();
		} 
	}
}

bool Bullet::IsAboveTerrain()
{
	return g_theGame->m_currentMap->IsPointAboveTerrain(GetPosition());
}

void Bullet::CheckAgainstEnemies()
{
	Vector3 pos = GetPosition();
	
	for (Enemy* enemy : m_playState->m_enemies){
		if (enemy->IsPointInside(pos)){
			enemy->Damage();
			Destroy();
		}
	}

	for (Spawner* spawner : m_playState->m_spawners){
		if (spawner->IsPointInside(pos)){
			spawner->Damage();
			Destroy();
		}
	}
}

void Bullet::Destroy()
{
	m_aboutToBeDeleted = true;
	m_playState->RemoveLight(m_light);
	delete m_light;
	m_playState->m_scene->RemoveRenderable(m_renderable);
}

//void Bullet::CheckForCollision(Enemy* e)
//{
//	Vector3 pos = GetPosition();
//	if (a != nullptr){
//  		Vector3 asteroidPos = a->GetPosition();
//		if ((asteroidPos - pos).GetLengthSquared() < a->GetRadiusSquared()){
//			a->Damage();
//			m_aboutToBeDeleted = true;
//			g_theGame->SpawnParticleBurst(pos);
//		}
//	}
//}
