#include "Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Tile.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/ObjLoader.hpp"

Entity::~Entity()
{
	g_theGame->GetScene()->RemoveRenderable(m_renderable);
}

Entity::Entity()
{
	m_renderable = new Renderable2D();
}

Entity::Entity(Vector2 position, Vector2 size, std::string texturePath, RGBA color)
{
	m_renderable = new Renderable2D(RENDERABLE_PLANE, size, color);
	Texture* tex = g_theRenderer->CreateOrGetTexture(texturePath);
	m_renderable->SetDiffuseTexture(tex, 0);
	m_renderable->SetPosition(position);


	m_spinDegreesPerSecond = 45.f;
	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;

}

void Entity::Update()
{
	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;

}

//void Entity::Render()
//{
//	//if (m_texture != nullptr){
//	//	g_theRenderer->BindTexture(*m_texture);
//	//	g_theRenderer->BindTexture("flat", 1);
//	//}
//
//	g_theRenderer->BindMaterial(m_renderable->m_materials[0]);
//
//	//g_theRenderer->PushAndTransform(m_transform.GetLocalMatrix());
//	g_theRenderer->DrawMesh(m_renderable->m_mesh);
//	//g_theRenderer->Pop();
//	g_theRenderer->ReleaseTexture();
//	//RenderHealthBar();
//	if (g_theGame->m_devMode){
//		RenderDevMode();
//	}
//}

void Entity::RenderDevMode()
{
	//g_theRenderer->SetFillMode(FILL_MODE_WIRE);
	//Render();
	//g_theRenderer->SetFillMode(FILL_MODE_FILL);

}

void Entity::SetDiffuseTexture(std::string texPath)
{
	m_renderable->SetDiffuseTexture( Texture::CreateOrGetTexture(texPath));
}

void Entity::SetMaterial(int index, std::string diffusePath, std::string normalPath)
{
	m_renderable->SetMaterial(index, diffusePath, normalPath);
}

void Entity::SetMaterial(Material * mat, int index)
{
	m_renderable->SetMaterial(mat, index);
}



bool Entity::IsAboutToBeDeleted()
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
	// 	float percentFull = (float) m_health / (float) m_maxHealth;
	// 	//if (percentFull < 1.f){
	// 	g_theRenderer->PushAndTransform2(m_position + Vector2(0.f, .5f), 0.f, .4f);
	// 	Vector2 mins = Vector2(-1.f, 0.f);
	// 	Vector2 maxs = Vector2(1.f, .3f);
	// 	AABB2 boundsBorder = AABB2(mins, maxs);
	// 	g_theRenderer->DrawAABB2(boundsBorder, RGBA(0,0,0));	//draw black outline
	// 
	// 	g_theRenderer->PushAndTransform2(Vector2(0.f, .1f), 0.f, .9f);
	// 	AABB2 boundsBackground = AABB2(mins, Vector2(maxs.x, maxs.y-.15f));
	// 	g_theRenderer->DrawAABB2(boundsBackground, RGBA(255,0,0));// draw red background;
	// 	Vector2 maxsPercentage = Vector2(RangeMapFloat(percentFull,0.f,1.f,-1.f,1.f), maxs.y-.15f);
	// 	AABB2 boundsHealth = AABB2(mins, maxsPercentage);
	// 	g_theRenderer->DrawAABB2(boundsHealth, RGBA(0,255,0));
	// 	g_theRenderer->Pop2();
	// 	g_theRenderer->Pop2();
	//}






}

void Entity::SetTransform(Transform newT)
{
	//m_renderable->m_transform = newT;
}

void Entity::Translate(Vector3 translation)
{
	m_renderable->m_transform.TranslateLocal(translation.XZ());
}

void Entity::Rotate(Vector3 rotation)
{
	m_renderable->m_transform.RotateByEuler(rotation.z);
}

void Entity::SetPosition(Vector3 newPos)
{
	m_renderable->m_transform.SetLocalPosition(newPos.XZ());
}

void Entity::SetScale(Vector3 scale)
{
	m_renderable->m_transform.SetScale(scale.XZ());
}

bool Entity::IsPointInForwardView(Vector3 point)
{
	return false;
	//return IsPointInConicSector2D(point, m_position, m_facing, m_forwardViewAngle, m_range);
}


