#include "Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Tile.hpp"
#include "Game/World.hpp"
#include "Engine/Renderer/ObjLoader.hpp"

Entity::~Entity()
{
	g_theGame->GetScene()->RemoveRenderable(m_renderable);
}

Entity::Entity()
{
	m_renderable = new Renderable();
}

Entity::Entity(Vector3 position, std::string objFile, std::string materialFile)
{
	m_renderable = new Renderable(objFile, materialFile);
	//wm_renderable->m_transform.SetLocalMatrix(matrix);
	//m_renderable->m_transform.SetLocalMatrix(g_worldToEngine);
	m_renderable->m_transform.TranslateLocal(position);

	m_spinDegreesPerSecond = 45.f;
	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;

	TODO("Set collision sphere on mesh entities to be the bounds of the mesh");
	m_collider = Sphere(position, 1.f);

}

void Entity::Update()
{
	PROFILE_PUSH_FUNCTION();
	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;
	PROFILE_POP();

}

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
	
}

void Entity::RunEntityPhysics()
{
	
}

IntVector2 Entity::GetCurrentChunkCoordinates() const
{
	return g_theGame->GetWorld()->GetChunkCoordinatesFromWorldCoordinates(GetPosition());
}


void Entity::SetTransform(Transform newT)
{
	m_renderable->m_transform = newT;
}

void Entity::Translate(Vector3 translation)
{
	m_renderable->m_transform.TranslateLocal(translation);
}

void Entity::Rotate(float yaw, float pitch, float roll)
{
	m_renderable->m_transform.RotateByEuler(yaw, pitch, roll);
}

void Entity::SetPosition(Vector3 newPos)
{
	m_renderable->m_transform.SetLocalPosition(newPos);
	m_collider.SetPosition(newPos);
}

void Entity::SetScale(Vector3 scale)
{
	m_renderable->m_transform.SetScale(scale);
}

bool Entity::IsPointInForwardView(Vector3 point)
{
	return false;
	//return IsPointInConicSector2D(point, m_position, m_facing, m_forwardViewAngle, m_range);
}


