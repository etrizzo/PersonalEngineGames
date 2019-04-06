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
	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;

	TODO("Set collision sphere on mesh entities to be the bounds of the mesh");
	m_collider = Sphere(Vector3::ZERO, m_radius);
	//m_eyePosition = new Transform();
	//m_eyePosition->SetParent(&m_transform);
	//m_eyePosition->SetLocalPosition(GetPosition() + (UP * m_eyeHeight));
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
	m_collider = Sphere(position, m_radius);

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
	//draw collider
	g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.0f, m_collider.m_center, m_collider.m_radius, 10, 10, RGBA::RED.GetColorWithAlpha(32), RGBA::RED.GetColorWithAlpha(32), DEBUG_RENDER_HIDDEN);
	g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.0f, m_collider.m_center, m_collider.m_radius, 10, 10, RGBA::RED, RGBA::RED, DEBUG_RENDER_USE_DEPTH);
	
	g_theGame->m_debugRenderSystem->MakeDebugRenderWireAABB3(0.0f, GetBounds(), RGBA::BLANCHEDALMOND.GetColorWithAlpha(32), RGBA::BLANCHEDALMOND.GetColorWithAlpha(32), DEBUG_RENDER_HIDDEN);
	g_theGame->m_debugRenderSystem->MakeDebugRenderWireAABB3(0.0f, GetBounds(), RGBA::BLANCHEDALMOND, RGBA::BLANCHEDALMOND, DEBUG_RENDER_USE_DEPTH);

	//g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.0f, m_eyePosition->GetWorldPosition(), RGBA::CYAN, RGBA::CYAN, DEBUG_RENDER_HIDDEN);
	//g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.0f, m_eyePosition->GetWorldPosition(), RGBA::CYAN, RGBA::CYAN, DEBUG_RENDER_USE_DEPTH);
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
	//clamp ds
	ds = ClampFloat(ds, 0.0f, .016f);


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

	

	//compute willpower force
	Vector3 willpowerForce = m_moveIntention * m_accelerationXY;

	//compute total force (gravity + willpower + friction in XY and Z)
	Vector3 totalForceDelta = (m_gravity + willpowerForce) * ds;

	//save current speed, apply change to velocity, and then get speed after change in velocity
	float unchangedSpeed = m_velocity.GetLength();
	m_velocity += totalForceDelta;
	float changedSpeed = m_velocity.GetLength();

	//if your speed after change > speed before change, clamp it to Max(speedbeforechange, maxspeed)
	if (changedSpeed > unchangedSpeed)
	{
		float maxSpeed = Max(unchangedSpeed, m_walkingSpeed);
		//m_velocity.ClampLength(20.0f);
		m_velocity.ClampLengthXY(maxSpeed);
	}

	//compute friction force
	Vector3 frictionForce = Vector3(m_velocity.XY() * -5.f, 0.0f);
	//apply friction
	m_velocity += (frictionForce * ds);

	//do the physics
	//float ds2 = ds;
	//ds2 = ClampFloat(ds2, 0.0f, .016f);
	Translate(m_velocity * ds);

	RunCorrectivePhysics();
}

void Entity::RunCorrectivePhysics()
{
	//get your blocc
	BlockLocator blocc = g_theGame->GetWorld()->GetBlockLocatorAtWorldPosition(m_collider.m_center);

	if (blocc.IsValid())
	{
		//check your first priority neighbors
		std::vector<BlockLocator> blocksToCheck = GetFirstCheckNeighbors(blocc);

		//push out of your neighbors
		for (int i = 0; i < blocksToCheck.size(); i++)
		{
			if (blocksToCheck[i].IsBlockSolid())
			{
				PushOutOfBlock(blocksToCheck[i]);
			}
		}

		blocksToCheck.clear();
		//check your second priority neighbors
		blocksToCheck = GetSecondCheckNeighbors(blocc);

		//push out of your neighbors
		for (int i = 0; i < blocksToCheck.size(); i++)
		{
			if (blocksToCheck[i].IsBlockSolid())
			{
				PushOutOfBlock(blocksToCheck[i]);
			}
		}
	}
}

void Entity::PushOutOfBlock(const BlockLocator & block)
{
	AABB3 blockBounds = block.GetBlockBounds();

	Vector3 closestPoint = blockBounds.GetClosestPoint(m_collider.m_center);
	if (m_collider.IsPointInside(closestPoint))
	{
		Vector3 dir = closestPoint - m_collider.m_center;
		float nonOverlap = dir.GetLength();
		float overlap = m_collider.m_radius - nonOverlap;
		dir = dir.GetNormalized();
		Translate(-dir * overlap);

		//remove this part of your velocity
		float lengthOnDir = DotProduct(m_velocity, dir);
		if (lengthOnDir >= 0.0f)
		{
			m_velocity -= (dir * lengthOnDir);
		}
	}

}

std::vector<BlockLocator> Entity::GetFirstCheckNeighbors(const BlockLocator & myBlock)
{
	std::vector<BlockLocator> neighbors = std::vector<BlockLocator>();
	//get your immediate neighbors
	neighbors.push_back(myBlock.GetNorth());
	neighbors.push_back(myBlock.GetSouth());
	neighbors.push_back(myBlock.GetWest());
	neighbors.push_back(myBlock.GetEast());
	neighbors.push_back(myBlock.GetUp());
	neighbors.push_back(myBlock.GetDown());

	return neighbors;
}

std::vector<BlockLocator> Entity::GetSecondCheckNeighbors(const BlockLocator & myBlock)
{
	std::vector<BlockLocator> neighbors = std::vector<BlockLocator>();
	//get your immediate neighbors
	neighbors.push_back(myBlock.GetNorth().GetEast());
	neighbors.push_back(myBlock.GetNorth().GetWest());
	neighbors.push_back(myBlock.GetSouth().GetEast());
	neighbors.push_back(myBlock.GetSouth().GetWest());
	
	neighbors.push_back(myBlock.GetNorth().GetDown());
	neighbors.push_back(myBlock.GetNorth().GetUp());
	neighbors.push_back(myBlock.GetSouth().GetDown());
	neighbors.push_back(myBlock.GetSouth().GetUp());

	neighbors.push_back(myBlock.GetEast().GetDown());
	neighbors.push_back(myBlock.GetEast().GetUp());
	neighbors.push_back(myBlock.GetWest().GetDown());
	neighbors.push_back(myBlock.GetWest().GetUp());

	return neighbors;
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
	SyncColliderPosition();
}

void Entity::Translate(Vector3 translation)
{
	m_transform.TranslateLocal(translation);
	SyncColliderPosition();
}

void Entity::Rotate(float yaw, float pitch, float roll)
{
	m_transform.RotateByEuler(yaw, pitch, roll);
}

void Entity::SetPosition(Vector3 newPos)
{
	m_transform.SetLocalPosition(newPos);
	SyncColliderPosition();
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

AABB3 Entity::GetBounds()
{
	Vector3 bottomCenter = m_transform.GetWorldPosition() - (UP * m_eyeHeight);
	Vector3 center = bottomCenter + (UP * m_height * .5f);
	return AABB3(center, m_radius, m_radius, m_height * .5f);
}

void Entity::SyncColliderPosition()
{
	Vector3 bottomCenter = m_transform.GetWorldPosition() - (UP * m_eyeHeight);
	m_collider.SetPosition(bottomCenter + (UP * m_collider.m_radius));

	//m_eyePosition->SetLocalPosition(GetPosition() + (UP * m_eyeHeight));
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

