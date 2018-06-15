#pragma once
#include "Game/Actor.hpp"

class Player: public Actor{
public:
	Player(ActorDefinition* actorDef, Vector2 initialPos, Map* map);
	~Player(){};

	void Update(float deltaSeconds) override;
	void Render() override;
	void RenderStatsInBox(AABB2 statsBox, RGBA tint = RGBA::WHITE);
	void RenderDistanceMap() const;

	void HandleInput();

	void SetPosition(Vector2 newPos, Map* newMap = nullptr) override;
	void EnterTile(Tile* tile) override;

	void TakeDamage(int dmg);
	void Respawn();
	std::string GetAnimName();

	
	bool m_godMode = false;
	bool m_dead;
	Heatmap m_distanceMap;

private:
	void UpdateDistanceMap();
	virtual void FireArrow();
	Vector2 GetClosestAngle();
	float checkDot(IntVector2 direction);

};