#include "Asteroid.hpp"
#include "Game/Game.hpp"
#include "Game/DebugRenderSystem.hpp"

Asteroid::Asteroid(Vector3 position, Vector3 target)
{
	static float asteroidRadius = .8f;
	m_sprite = new Sprite(g_theRenderer->CreateOrGetTexture("asteroid.png"), AABB2::ZERO_TO_ONE, Vector2(.5f, 0.f), Vector2::ONE * asteroidRadius);
	m_targetPosition = target;
	m_collider = Sphere(position, asteroidRadius);
	SetPosition(position);
}

void Asteroid::Update()
{
	Vector3 displacement = m_targetPosition - GetPosition();
	Vector3 direction = displacement.GetNormalized();

	Translate(direction * m_speed * GetMasterClock()->GetDeltaSeconds());

	if (m_collider.IsPointInside(m_targetPosition))
	{
		Explode();
	}

}

void Asteroid::Render()
{
	Entity::Render();
	//g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.0f, m_collider.m_center, m_collider.m_radius);
	g_theGame->m_debugRenderSystem->MakeDebugRender3DText("Target", 0.0f, m_targetPosition, .1f, Vector3::Y_AXIS, g_theGame->m_mainCamera->GetRight(), RGBA::YELLOW, RGBA::YELLOW, DEBUG_RENDER_XRAY);
}

void Asteroid::Explode()
{
	if (!m_aboutToBeDeleted)
	{
		m_aboutToBeDeleted = true;
	}
	
}
