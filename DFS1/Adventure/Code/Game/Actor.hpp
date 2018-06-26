#pragma once
#include "Game/Entity.hpp"
#include "Game/Stats.hpp"
#include "Game/ClothingSet.hpp"

class ActorDefinition;
class Item;

class Actor: public Entity{
public:
	Actor(){};
	Actor(ActorDefinition* definition, Map* entityMap, Vector2 initialPos, float initialRotation = 0.f, int difficulty = 0);
	~Actor();

	void Update(float deltaSeconds);
	void Render();
	std::string GetAnimName();

	void UpdateRenderable() override;
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
	//std::vector<Texture*> m_layerTextures;
	ClothingSet* m_currentLook;
	Item* m_equippedItems[NUM_EQUIP_SLOTS];
	Stats m_stats;
	Stats m_baseStats;
	bool m_changedClothes;

	void StartFiringArrow();
	
	static std::string GetEquipSlotByID(EQUIPMENT_SLOT equipID);

	//std::vector<SpriteAnimSet*> m_animSets;		//collection of all anim sets - m_animSets[0] = m_animSet;


protected:
	void ParseLayersElement();

	float m_lastAttacked;
	void UpdateWithController(float deltaSeconds);
	void RunSimpleAI(float deltaSeconds);
	void Wander(float deltaSeconds);

	virtual void FireArrow();

	void CheckActorForCollision(Actor* actor);
	void GetRandomStatsFromDefinition();

	void UpdateStats();
	bool m_isFiring;
};