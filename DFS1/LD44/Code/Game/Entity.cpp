#include "Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/ObjLoader.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Engine/Renderer/Sprite.hpp"
#include "Game/DebugRenderSystem.hpp"



Entity::~Entity()
{
	delete m_sprite;
}



//Entity::Entity(Vector3 position, std::string objFile, std::string materialFile)
//{
//	m_renderable = new Renderable(objFile, materialFile);
//	SetPosition(position);
//
//
//	m_spinDegreesPerSecond = 45.f;
//	m_ageInSeconds = 0.f;
//	m_aboutToBeDeleted = false;
//
//	m_noClipMode = false;
//
//	TODO("Expand sphere with object as it gets loaded in");
//	m_collider = Sphere(position, 1.f);
//
//}

void Entity::Update()
{
	PROFILE_PUSH_FUNCTION();
	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;
	PROFILE_POP();

}

void Entity::Render()
{
	g_theRenderer->BindShader(Shader::GetShader("cutout"));
	g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.0f, m_transform.GetWorldPosition());
	g_theRenderer->DrawSprite(m_transform.GetWorldPosition(), m_sprite, g_theGame->m_mainCamera->GetRight(), Vector3::Y_AXIS);
}

void Entity::RenderDevMode()
{
	//g_theRenderer->SetFillMode(FILL_MODE_WIRE);
	//Render();
	//g_theRenderer->SetFillMode(FILL_MODE_FILL);

}

void Entity::TakeDamage()
{
}

void Entity::UpdateAnimation()
{
	
	std::string animName = GetAnimName();
	m_animSet->SetCurrentAnim(animName);

	//get percentage before update
	float tBefore = m_animSet->GetPercentageThroughCurrentAnim();

	m_animSet->Update(GetMasterClock()->GetDeltaSeconds());
	m_sprite->m_uvs = m_animSet->GetCurrentUVs();

	//get percentage after update
	float tAfter = m_animSet->GetPercentageThroughCurrentAnim();

	//if attacking, execute the attack
	if (m_animState == ANIM_STATE_ATTACK)
	{
		if (tBefore < m_percThroughAnimationToExecuteAttack && tAfter >= m_percThroughAnimationToExecuteAttack)
		{
			//execute attack halfway through anim

			//m_isAttacking = false;
			ExecuteAttack();
		}
		if (m_animSet->IsCurrentAnimFinished())
		{
			m_animState = ANIM_STATE_IDLE;
		}
	}

	if (m_animState == ANIM_STATE_RELOAD)
	{
		if (tBefore < m_percThroughAnimationToReload && tAfter >= m_percThroughAnimationToReload)
		{
			//execute attack halfway through anim

			//m_isAttacking = false;
			ExecuteReload();
		}
		if (m_animSet->IsCurrentAnimFinished())
		{
			m_animState = ANIM_STATE_IDLE;
		}
	}

}

std::string Entity::GetAnimName() const
{
	float dir = m_facing.x;
	std::string direction = "Right";
	if (dir >= 0.f) {
		direction = "Right";
	}
	else {
		direction = "Left";
	}

	std::string action = "Idle";
	switch (m_animState)
	{
	case ANIM_STATE_IDLE:
		action = "Idle";
		break;
	case ANIM_STATE_WALK:
		action = "Move";
		break;
	case ANIM_STATE_ATTACK:
		action = "Attack";
		break;
	case ANIM_STATE_DEATH:
		action = "Death";
		break;
	case ANIM_STATE_RELOAD:
		action = "Reload";
		break;
	}
	return action + direction;
}

void Entity::BeginAttack()
{
	m_animState = ANIM_STATE_ATTACK;
}

void Entity::ExecuteAttack()
{
}

void Entity::BeginReload()
{
	m_animState = ANIM_STATE_RELOAD;
}

void Entity::ExecuteReload()
{
}

void Entity::BeginDeath()
{
	m_animState = ANIM_STATE_DEATH;
}

bool Entity::IsDead() const
{
	return m_aboutToBeDeleted;
}

void Entity::RunCorrectivePhysics()
{
	if (!m_noClipMode){
		RunWorldPhysics();
		RunEntityPhysics();
	}
}

void Entity::RunWorldPhysics()
{
	UNIMPLEMENTED();
}

void Entity::RunEntityPhysics()
{
	UNIMPLEMENTED();
}


void Entity::RenderHealthBar()
{
	UNIMPLEMENTED();

}

void Entity::SetTransform(Transform newT)
{
	m_transform = newT;
}

void Entity::Translate(Vector3 translation)
{
	m_transform.TranslateLocal(translation);
	m_collider.SetPosition(m_transform.GetWorldPosition());
}

void Entity::Rotate(Vector3 rotation)
{
	m_transform.RotateByEuler(rotation);
}

void Entity::SetPosition(Vector3 newPos)
{
	m_transform.SetLocalPosition(newPos);
	m_collider.SetPosition(newPos);
}

void Entity::SetScale(Vector3 scale)
{
	m_transform.SetScale(scale);
}

bool Entity::IsPointInForwardView(Vector3 point)
{
	return false;
	//return IsPointInConicSector2D(point, m_position, m_facing, m_forwardViewAngle, m_range);
}


