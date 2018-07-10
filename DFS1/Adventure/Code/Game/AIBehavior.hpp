#pragma once
#include "Game/GameCommon.hpp"
class Actor;
class Tile;

class AIBehavior{
public:
	AIBehavior(const tinyxml2::XMLElement& behaviorElement);
	~AIBehavior(){};

	virtual void Update(float deltaSeconds) = 0;
	virtual AIBehavior* CloneForActor(Actor* owner) = 0;
	virtual float CalcUtility() = 0;

	float m_baseUtility = 0.f;

	static AIBehavior* CreateAIBehavior( const tinyxml2::XMLElement& behaviorElement );

protected:
	std::string m_name = std::string();
	Actor* m_actor = nullptr;
};

////WIP
//class AIBehavior_Patrol : public AIBehavior{
//public:
//	AIBehavior_Patrol(const tinyxml2::XMLElement& behaviorElement);
//	~AIBehavior_Patrol();
//
//	void Update(float deltaSeconds) override;
//	AIBehavior* CloneForActor(Actor* owner) override;
//	float CalcUtility();
//
//	Tile* m_spawnTile = nullptr;
//	Tile* m_patrolToTile = nullptr;
//
//};