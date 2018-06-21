#pragma once
#include "Game/Actor.hpp"

class DecorationDefinition;

class Decoration: public Entity{
public:
	Decoration(){};
	Decoration(DecorationDefinition* definition, Map* entityMap, Vector2 initialPos, float initialRotation = 0.f, std::string faction = "none");
	~Decoration(){};

	void Update(float deltaSeconds);
	void Render();

	void RunCorrectivePhysics();

	DecorationDefinition* m_definition;
	bool m_currentlyEquipped = false;
private:
	void CheckActorForCollision(Actor* actor);
};