#include "Decoration.hpp"
#include "DecorationDefinition.hpp"
#include "Map.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Game/Actor.hpp"

Decoration::Decoration(DecorationDefinition * definition, Map * entityMap, Vector2 initialPos, float initialRotation, std::string faction)
	:Entity((EntityDefinition*)definition, entityMap, initialPos, initialRotation)
{
	m_definition = definition;
	m_faction = faction;
	m_facing = Vector2::MakeDirectionAtDegrees(initialRotation);
	m_localDrawingBox = AABB2(-definition->m_drawingRadius, -definition->m_drawingRadius, definition->m_drawingRadius, definition->m_drawingRadius);
}

void Decoration::Update(float deltaSeconds)
{
	Entity::Update(deltaSeconds);
}

void Decoration::Render()
{
	Entity::Render();
}

void Decoration::RunCorrectivePhysics()
{
	for (int actorIndex = 0; actorIndex < (int) m_map->m_allActors.size(); actorIndex++){
		Actor* collidingActor = (Actor*) m_map->m_allActors[actorIndex];
		if (collidingActor->GetPosition() != GetPosition()){
			CheckActorForCollision(collidingActor);
		}
	}
}

void Decoration::CheckActorForCollision(Actor * actor)
{
	Vector2 pos = GetPosition();
	Vector2 actorPos = actor->GetPosition();
	if (DoDiscsOverlap(m_physicsDisc, actor->m_physicsDisc)){
		Vector2 direction = (pos - actorPos).GetNormalized();
		Vector2 radiusPoint = pos - (direction * m_physicsDisc.radius);
		Vector2 entityPushTo = actor->m_physicsDisc.PushOutOfPoint(radiusPoint);
		actor->SetPosition(entityPushTo);
	}
}



