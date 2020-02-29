#include "FlowerPot.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Asteroid.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer//PerspectiveCamera.hpp"

FlowerPot::FlowerPot(float xPosition)
{
	m_rateOfFire = StopWatch();
	m_rateOfFire.SetTimer(TURRET_RATE_OF_FIRE);

	float zOfMap = g_theGame->m_currentMap->m_collider.m_center.z;

	float yPos = g_theGame->m_currentMap->GetHeightFromXPosition(xPosition);
	m_collider = Sphere(Vector3(xPosition, yPos, zOfMap), 1.5f);
	float renderableY = yPos - (FLOWERPOT_WIDTH * .5f);		//lower the actual renderable a little bit
	m_transform.SetLocalPosition(Vector3(xPosition, yPos, zOfMap + .1f));
	Vector3 position = Vector3(xPosition, renderableY, FLOWERPOT_DEPTH);

	//set up flowerpot renderable
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(Vector3(0.f, -FLOWERPOT_WIDTH, 0.0f), Vector3(FLOWERPOT_WIDTH, FLOWERPOT_WIDTH * 4.0f, FLOWERPOT_WIDTH) , RGBA::WHITE, Vector3::X_AXIS, Vector3::Y_AXIS, Vector3::Z_AXIS);
	mb.End();

	m_flowerPotRenderable = new Renderable();
	m_flowerPotRenderable->SetMesh(mb.CreateMesh());
	m_flowerPotRenderable->SetMaterial(Material::GetMaterial("couch_cel"));
	m_flowerPotRenderable->SetPosition(position);
	g_theGame->m_playState->m_scene->AddRenderable(m_flowerPotRenderable);

	m_facing = Vector3::X_AXIS * -1.f;
	m_flowerSpriteAnimSet = new SpriteAnimSet(g_theGame->m_playState->m_flowerAnimDefinition);
	m_flowerSprite = new Sprite(m_flowerSpriteAnimSet->GetCurrentTexture(), AABB2::ZERO_TO_ONE, Vector2(.5f, 0.0f), Vector2::ONE * FLOWERPOT_WIDTH, Vector2::ONE);
	//this is the resupply point sprite
	m_animSet = new SpriteAnimSet(g_theGame->m_playState->m_resupplyAnimDefinition);
	m_sprite = new Sprite(m_animSet->GetCurrentTexture() , AABB2::ZERO_TO_ONE, Vector2(.5f, 0.0f), Vector2::ONE * .9f);
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
		BeginAttack();
	}

	if (!IsDead() && m_numBullets <= 0)
	{
		BeginDeath();
		g_theAudio->PlayOneOffSoundFromGroup("flowerdeath");
	}

	UpdateAnimation();
	UpdateFlowerAnimation();


}

void FlowerPot::Render()
{
	Entity::Render();
	Vector3 spritePos = m_flowerPotRenderable->GetPosition() + (Vector3::Y_AXIS * FLOWERPOT_WIDTH);
	g_theRenderer->DrawSprite(spritePos, m_flowerSprite, g_theGame->m_mainCamera->GetRight(), Vector3::Y_AXIS);
	//g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.f, m_collider.m_center, m_collider.m_radius, 10, 10, RGBA::RED.GetColorWithAlpha(60));
	g_theGame->m_debugRenderSystem->MakeDebugRender3DText(std::to_string(m_numBullets), 0.0f, m_flowerPotRenderable->GetPosition() + (Vector3::ONE * FLOWERPOT_WIDTH), 1.f, UP, RIGHT, RGBA::CYAN, RGBA::CYAN);
}

void FlowerPot::TakeDamage()
{
	if (m_numBullets > 0 && !g_theGame->m_godMode)
	{
		m_numBullets--;
	}
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

void FlowerPot::BeginAttack()
{
	if (m_target != nullptr && m_numBullets > 0)
	{
		if (m_rateOfFire.CheckAndReset())
		{
			m_animState = ANIM_STATE_ATTACK;

		}
	}
}

void FlowerPot::ExecuteAttack()
{
	if (m_target != nullptr)
	{
		g_theGame->m_playState->SpawnMissile(m_flowerPotRenderable->GetPosition() + (UP * FLOWERPOT_WIDTH * 1.25f), m_target);
		m_target = nullptr;
		if (!g_theGame->m_godMode)
		{
			m_numBullets--;
		}
	}
}

void FlowerPot::UpdateFlowerAnimation()
{
	std::string animName = GetAnimName();
	m_flowerSpriteAnimSet->SetCurrentAnim(animName);

	//get percentage before update
	float tBefore = m_flowerSpriteAnimSet->GetPercentageThroughCurrentAnim();

	m_flowerSpriteAnimSet->Update(GetMasterClock()->GetDeltaSeconds());
	m_flowerSprite->m_uvs = m_flowerSpriteAnimSet->GetCurrentUVs();

	//get percentage after update
	float tAfter = m_flowerSpriteAnimSet->GetPercentageThroughCurrentAnim();

	//if attacking, execute the attack
	if (m_animState == ANIM_STATE_ATTACK)
	{
		if (tBefore < .5f && tAfter >= .5f)
		{
			//execute attack halfway through anim

			//m_isAttacking = false;
			//ExecuteAttack();
		}
		if (m_flowerSpriteAnimSet->IsCurrentAnimFinished())
		{
			m_animState = ANIM_STATE_IDLE;
		}
	}


	if (m_animState == ANIM_STATE_RELOAD)
	{
		if (m_flowerSpriteAnimSet->IsCurrentAnimFinished())
		{
			m_animState = ANIM_STATE_IDLE;
		}
	}


	if (IsDead())
	{
		m_animState = ANIM_STATE_DEATH;
	}

}

void FlowerPot::Reload(int numToReload)
{
	m_numBullets += numToReload;
	m_animState = ANIM_STATE_RELOAD;
	g_theAudio->PlayOneOffSoundFromGroup("reload");
}

bool FlowerPot::IsDead() const
{
	return m_numBullets <= 0;
}
