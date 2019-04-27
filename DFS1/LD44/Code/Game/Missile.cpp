#include "Missile.hpp"
#include "Game/Game.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/Asteroid.hpp"

Missile::Missile(Vector3 position, Asteroid * target)
{
	static float MissileRadius = .8f;
	m_sprite = new Sprite(g_theRenderer->CreateOrGetTexture("missile.png"), AABB2::ZERO_TO_ONE, Vector2(.5f, 0.f), Vector2::ONE * MissileRadius);
	m_target = target;
	m_target->m_isTargeted = true;
	m_collider = Sphere(position, MissileRadius);
	SetPosition(position);

	RGBA tint = RGBA(255, 200, 100, 240);

	//set up flowerpot renderable
	MeshBuilder mb;
	Vector3 upright = (UP + RIGHT).GetNormalized();
	Vector3 leftRight = (UP + (-1.f * RIGHT)).GetNormalized();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendPlane(Vector3::ZERO, upright, FORWARD, Vector2(.55f, .1f), tint, Vector2::ZERO, Vector2::ONE);
	mb.AppendPlane(Vector3::ZERO, leftRight, FORWARD, Vector2(.55f, .1f), tint, Vector2::ZERO, Vector2::ONE);
	mb.End();

	m_renderable = new Renderable();
	m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_3DPCU));
	m_renderable->m_transform.SetParent(&m_transform);
	g_theGame->m_playState->m_scene->AddRenderable(m_renderable);
	//m_renderable->m_mesh->SetSubMesh(mb.CreateMesh())

	//m_renderable->m_transform.SetLocalMatrix(m_transform.GetWorldMatrix());
	//Rotate(Vector3(0.f,0.f,45.f));
	//SetPosition(t.GetWorldPosition());
	Material* mat = Material::GetMaterial("additive");
	if (mat != nullptr) {
		m_renderable->SetMaterial(mat);
	}
	//m_renderable->m_transform = t;
	//m_playState = playState;
	m_light = g_theGame->m_playState->AddNewPointLight(Vector3::ZERO, tint);
	m_light->m_transform.SetParent(&m_renderable->m_transform);

}

Missile::~Missile()
{
	g_theGame->m_playState->m_scene->RemoveRenderable(m_renderable);
	g_theGame->m_playState->m_scene->RemoveLight(m_light);
	delete m_renderable;
	delete m_light;
}

void Missile::Update()
{
	if (m_target == nullptr || m_target->m_aboutToBeDeleted)
	{
		m_aboutToBeDeleted = true;
	}
	else {
		Vector3 displacement = m_target->GetPosition() - GetPosition();
		Vector3 direction = displacement.GetNormalized();

		Translate(direction * m_speed * GetMasterClock()->GetDeltaSeconds());
		m_transform.LookAt(GetPosition(), m_target->GetPosition());

		if (m_collider.IsPointInside(m_target->GetPosition()))
		{
			KillTarget();
		}
	}

}

void Missile::Render()
{
	g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.0f, m_transform.GetWorldPosition());



	//g_theRenderer->DrawSprite(m_transform.GetWorldPosition(), m_sprite, g_theGame->m_mainCamera->GetRight(), GetForward());
	//Entity::Render();
	//g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.0f, m_collider.m_center, m_collider.m_radius);
	//g_theGame->m_debugRenderSystem->MakeDebugRender3DText("Target", 0.0f, m_targetPosition, .1f, Vector3::Y_AXIS, g_theGame->m_mainCamera->GetRight(), RGBA::YELLOW, RGBA::YELLOW, DEBUG_RENDER_XRAY);
}

void Missile::KillTarget()
{
	if (!m_aboutToBeDeleted)
	{
		m_target->m_aboutToBeDeleted = true;
		m_aboutToBeDeleted = true;
	}

}
