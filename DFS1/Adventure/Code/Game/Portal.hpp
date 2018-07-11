#pragma once
#include "Game/Entity.hpp"
class PortalDefinition;
class Actor;
class Party;

class Portal: public Entity{
public:
	Portal(){};
	Portal(PortalDefinition* definition, Map* entityMap, Map* destinationMap, Vector2 initialPos, Vector2 toPos, float initialRotation, bool spawnReciprocal = true, std::string faction="good");
	~Portal(){};

	void Update(float deltaSeconds);
	void Render();
	void CheckIfTeleport();

	void RunCorrectivePhysics();

	void Teleport(Actor* actor);
	void Teleport(Party* party);

	PortalDefinition* m_definition;
	bool m_isReadyToTeleport = false;

private:
	Map* m_destinationMap;
	Vector2 m_toPosition;
	bool m_isLocked;
	Portal* reciprocal = nullptr;
	
};