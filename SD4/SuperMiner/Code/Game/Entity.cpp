#include "Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Tile.hpp"
#include "Game/World.hpp"
#include "Engine/Renderer/ObjLoader.hpp"
#include "Game/GameCamera.hpp"
#include "Game/DebugRenderSystem.hpp"

Entity::~Entity()
{
	if (m_camera != nullptr)
	{
		delete m_camera;
	}
}

Entity::Entity()
{
	m_renderable = new Renderable();
	m_renderable->m_transform.SetParent(&m_transform);
}

Entity::Entity(Vector3 position, std::string objFile, std::string materialFile)
{
	m_renderable = new Renderable(objFile, materialFile);
	m_renderable->m_transform.SetParent(&m_transform);
	//wm_renderable->m_transform.SetLocalMatrix(matrix);
	//m_renderable->m_transform.SetLocalMatrix(g_worldToEngine);
	m_transform.TranslateLocal(position);

	m_spinDegreesPerSecond = 45.f;
	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;

	TODO("Set collision sphere on mesh entities to be the bounds of the mesh");
	m_collider = Sphere(position, .4f);

}

void Entity::Update()
{
	PROFILE_PUSH_FUNCTION();
	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;
	PROFILE_POP();

}

void Entity::Render()
{
	g_theRenderer->BindModel(m_renderable->m_transform.GetWorldMatrix());
	g_theRenderer->BindMaterial(m_renderable->GetEditableMaterial(0));
	g_theRenderer->DrawMesh(m_renderable->m_mesh->m_subMeshes[0]);
}

void Entity::RenderBounds()
{
	//g_theGame->m_debugRenderSystem->MakeDebugRenderWireAABB3(0.f, m_collider.m_center, m_collider.m_radius, RGBA::CYAN, RGBA::CYAN, DEBUG_RENDER_HIDDEN);
	//g_theGame->m_debugRenderSystem->MakeDebugRenderWireAABB3(0.f, m_collider.m_center, m_collider.m_radius, RGBA::RED, RGBA::RED, DEBUG_RENDER_USE_DEPTH);
	g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.0f, m_collider.m_center, m_collider.m_radius, 10, 10, RGBA::RED.GetColorWithAlpha(32), RGBA::RED.GetColorWithAlpha(32), DEBUG_RENDER_HIDDEN);
	g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.0f, m_collider.m_center, m_collider.m_radius, 10, 10, RGBA::RED, RGBA::RED, DEBUG_RENDER_USE_DEPTH);
	
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

void Entity::RunPhysics()
{
	float ds = GetMasterClock()->GetDeltaSeconds();
	switch (m_physicsMode)
	{
	case (PHYSICS_MODE_NOCLIP):
		//do no physics
		return;
	case (PHYSICS_MODE_WALKING):
		//set walking variables
		break;
	case (PHYSICS_MODE_FLYING):
		//set flying variables
		break;
	}

	//compute friction force
	Vector3 frictionForce = -m_velocity * 1.f;

	//compute willpower force
	Vector3 willpowerForce = m_moveIntention * m_accelerationXY;

	//compute total force (gravity + willpower + friction in XY and Z)
	Vector3 totalForceDelta = (m_gravity + willpowerForce + frictionForce) * ds;

	//save current speed, apply change to velocity, and then get speed after change in velocity
	float unchangedSpeed = m_velocity.GetLength();
	m_velocity += totalForceDelta;
	float changedSpeed = m_velocity.GetLength();

	//if your speed after change > speed before change, clamp it to Max(speedbeforechange, maxspeed)
	if (changedSpeed > unchangedSpeed)
	{
		float maxSpeed = Max(unchangedSpeed, m_walkingSpeed);
		m_velocity.ClampLength(maxSpeed);
	}

	//do the physics
	Translate(m_velocity * GetMasterClock()->GetDeltaSeconds());

	RunCorrectivePhysics();
}

void Entity::RunCorrectivePhysics()
{
	//get your blocc
}



//void Entity::RunCorrectivePhysics()
//{
//	if (!m_noClipMode){
//		RunWorldPhysics();
//		RunEntityPhysics();
//	}
//}
//
//void Entity::RunWorldPhysics()
//{
//	
//}
//
//void Entity::RunEntityPhysics()
//{
//	
//}

void Entity::GiveGameCamera(GameCamera* camera)
{
	if (m_camera != nullptr)
	{
		//uhhhh panic?
		ConsolePrintf(RGBA::RED, "Entity already had camera - deleting it i guess");
		delete m_camera;
	}
	m_camera = camera;
}

void Entity::RemoveGameCamera()
{
	m_camera = nullptr;
}

IntVector2 Entity::GetCurrentChunkCoordinates() const
{
	return g_theGame->GetWorld()->GetChunkCoordinatesFromWorldCoordinates(GetPosition());
}


void Entity::SetTransform(Transform newT)
{
	m_transform = newT;
	m_renderable->m_transform.SetParent(&m_transform);
	m_collider.SetPosition(m_transform.GetWorldPosition());
}

void Entity::Translate(Vector3 translation)
{
	m_transform.TranslateLocal(translation);
	m_collider.SetPosition(m_transform.GetWorldPosition());
}

void Entity::Rotate(float yaw, float pitch, float roll)
{
	m_transform.RotateByEuler(yaw, pitch, roll);
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

std::string Entity::GetPhysicsModeString() const
{
	switch (m_physicsMode)
	{
	case PHYSICS_MODE_NOCLIP:
		return "NO_CLIP";
	case PHYSICS_MODE_WALKING:
		return "WALKING";
	case PHYSICS_MODE_FLYING:
		return "FLYING";
	default:
		return "WHO ARE U";
	}
}

