#include "FlowerPot.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Asteroid.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer//PerspectiveCamera.hpp"

FlowerPot::FlowerPot(float xPosition)
{
	m_rateOfFire = StopWatch();
	m_rateOfFire.SetTimer(3.f);

	float zOfMap = g_theGame->m_currentMap->m_collider.m_center.z;

	float yPos = g_theGame->m_currentMap->GetHeightFromXPosition(xPosition);
	m_collider = Sphere(Vector3(xPosition, yPos, zOfMap), .9f);
	float renderableY = yPos * .95f;		//lower the actual renderable a little bit
	m_transform.SetLocalPosition(Vector3(xPosition, yPos, zOfMap + .1f));
	Vector3 position = Vector3(xPosition, renderableY, FLOWERPOT_DEPTH);

	//set up flowerpot renderable
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(Vector3::ZERO, Vector3::ONE * FLOWERPOT_WIDTH, RGBA::WHITE, Vector3::X_AXIS, Vector3::Y_AXIS, Vector3::Z_AXIS);
	mb.End();

	m_flowerPotRenderable = new Renderable();
	m_flowerPotRenderable->SetMesh(mb.CreateMesh());
	m_flowerPotRenderable->SetMaterial(Material::GetMaterial("couch_cel"));
	m_flowerPotRenderable->SetPosition(position);
	g_theGame->m_playState->m_scene->AddRenderable(m_flowerPotRenderable);

	m_flowerSprite = new Sprite(g_theRenderer->CreateOrGetTexture("turret.png"), AABB2::ZERO_TO_ONE, Vector2(.5f, 0.0f), Vector2::ONE * FLOWERPOT_WIDTH * .8f, Vector2::ONE);
	//this is the resupply point sprite
	m_sprite = new Sprite(g_theRenderer->CreateOrGetTexture("bucket.png"), AABB2::ZERO_TO_ONE, Vector2(.5f, 0.0f), Vector2::ONE * .9f);
}

FlowerPot::~FlowerPot()
{
	g_theGame->m_playState->m_scene->RemoveRenderable(m_flowerPotRenderable);
	delete m_flowerPotRenderable;
	delete m_flowerSprite;
}

void FlowerPot::Update()
{
	if (m_target == nullptr)
	{
		//get u a target
		FindNewTarget();
	} else {
		TODO("Turn towards target first");
		FireAtTarget();
	}
}

void FlowerPot::Render()
{
	Entity::Render();
	Vector3 spritePos = m_flowerPotRenderable->GetPosition() + (Vector3::Y_AXIS * FLOWERPOT_WIDTH);
	g_theRenderer->DrawSprite(spritePos, m_flowerSprite, g_theGame->m_mainCamera->GetRight(), Vector3::Y_AXIS);

	g_theGame->m_debugRenderSystem->MakeDebugRender3DText(std::to_string(m_numBullets), 0.0f, m_flowerPotRenderable->GetPosition(), 2.f, UP, RIGHT, RGBA::CYAN, RGBA::CYAN, DEBUG_RENDER_XRAY);
}

void FlowerPot::FindNewTarget()
{
	//target the lowest asteroid that is not already targeted
	Asteroid* target = nullptr;
	float lowestHeight = 10000.f;
	for (Asteroid* asteroid : g_theGame->m_playState->m_asteroids)
	{
		if (!asteroid->m_isTargeted && !asteroid->m_aboutToBeDeleted)
		{
			if (asteroid->GetPosition().y < lowestHeight)
			{
				lowestHeight = asteroid->GetPosition().y;
				target = asteroid;
			}
		}
	}
	
	m_target = target;
	if (m_target != nullptr)
	{ 
		m_target->m_isTargeted = true;
	}
}

void FlowerPot::FireAtTarget()
{
	if (m_target != nullptr && m_numBullets > 0)
	{
		if (m_rateOfFire.CheckAndReset())
		{
			g_theGame->m_playState->SpawnMissile(m_flowerPotRenderable->GetPosition(), m_target);
			m_target = nullptr;
			m_numBullets--;
		}
	}
}

void FlowerPot::Reload(int numToReload)
{
	m_numBullets += numToReload;
}
