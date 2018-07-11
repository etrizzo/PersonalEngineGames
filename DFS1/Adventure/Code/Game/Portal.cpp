#include "Game/Portal.hpp"
#include "Game/PortalDefinition.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Party.hpp"

Portal::Portal(PortalDefinition * definition, Map * entityMap, Map* destinationMap, Vector2 initialPos, Vector2 toPos, float initialRotation, bool spawnReciprocal, std::string faction)
	:Entity((EntityDefinition*)definition, entityMap, initialPos, initialRotation)
{
	m_definition = definition;
	m_destinationMap = destinationMap;
	m_toPosition = toPos;
	m_faction = faction;
	m_rotationDegrees = initialRotation;
	m_localDrawingBox = AABB2(-definition->m_drawingRadius, -definition->m_drawingRadius, definition->m_drawingRadius, definition->m_drawingRadius);

	//if there's a reciprocal portal, spawn it 
	if (spawnReciprocal && (m_definition->m_hasReciprocalPortal)){
		reciprocal = m_destinationMap->SpawnNewPortal(m_definition->m_reciprocalName, toPos, m_map, initialPos, initialRotation, false);
		reciprocal->reciprocal = this;
	}
}

void Portal::Update(float deltaSeconds)
{
	Entity::Update(deltaSeconds);
	

}

void Portal::Render()
{
	Entity::Render();
}

void Portal::CheckIfTeleport()
{
	m_isReadyToTeleport = false;
	bool blocked = false;
	for (unsigned int actorIndex = 0; actorIndex < m_map->m_allActors.size(); actorIndex++){
		Actor* actor = m_map->m_allActors[actorIndex];
		if (DoDiscsOverlap(m_physicsDisc, actor->m_physicsDisc)){
			blocked = true;
			break;
		}
	}
	m_isReadyToTeleport = !blocked;
}

void Portal::RunCorrectivePhysics()
{
}

void Portal::Teleport(Actor * actor)
{
	if (reciprocal != nullptr){
		reciprocal->m_isReadyToTeleport = false;
	}
	actor->SetPosition(m_toPosition, m_destinationMap);
}

void Portal::Teleport(Party * party)
{
	if (reciprocal != nullptr){
		reciprocal->m_isReadyToTeleport = false;
	}
	party->MovePartyToMap(m_destinationMap, m_toPosition);
}
