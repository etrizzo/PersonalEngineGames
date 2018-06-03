#pragma once
#include "Game/Entity.hpp"
#include "Game/Stats.hpp"

class ActorDefinition;
class Item;

class Actor: public Entity{
public:
	Actor(){};
	Actor(ActorDefinition* definition, Map* entityMap, Vector2 initialPos, float initialRotation = 0.f);
	~Actor();

	void Update(float deltaSeconds);
	void Render();
	std::string GetAnimName();

	void RunCorrectivePhysics() override;
	void RunWorldPhysics() override;
	void RunEntityPhysics() override;
	void SetPosition(Vector2 newPos, Map* newMap = nullptr) override;
	void EnterTile(Tile* tile) override;

	virtual void TakeDamage(int dmg);

	void EquipOrUnequipItem(Item* itemToEquip);

	ActorDefinition* m_definition;
	float m_timeLastUpdatedDirection;
	bool m_moving;
	Item* m_equippedItems[NUM_EQUIP_SLOTS];
	Stats m_stats;
	Stats m_baseStats;
	
	static std::string GetEquipSlotByID(EQUIPMENT_SLOT equipID);

protected:
	float m_lastAttacked;
	void UpdateWithController(float deltaSeconds);
	void RunSimpleAI(float deltaSeconds);
	void Wander(float deltaSeconds);

	virtual void FireArrow();

	void CheckActorForCollision(Actor* actor);
	void GetRandomStatsFromDefinition();

	void UpdateStats();
};