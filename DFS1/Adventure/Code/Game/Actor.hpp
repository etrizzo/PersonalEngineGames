#pragma once
#include "Game/Entity.hpp"
#include "Game/Stats.hpp"
#include "Game/ClothingSet.hpp"
#include "Game/DialogueSet.hpp"

class ActorDefinition;
class Item;
class Quest;



class Actor: public Entity{
public:
	Actor(){};
	Actor(ActorDefinition* definition, Map* entityMap, Vector2 initialPos, float initialRotation = 0.f, int difficulty = 0);
	~Actor();

	void Update(float deltaSeconds);
	void Render();
	void HandleInput();

	void EquipItemsInInventory();
	void AssignAsQuestGiver(Quest* questToGive);
	void AdvanceQuest();
	void FinishQuest();		//resets actor to default after finishing their quest

	void RenderStatsInBox(AABB2 statsBox, RGBA tint = RGBA::WHITE);
	void RenderBoyInBox(AABB2 boyBox, RGBA tint=RGBA::WHITE);
	void RenderEquippedWeaponInBox(AABB2 weaponBox, RGBA tint = RGBA::WHITE);
	std::string GetAnimName();

	void UpdateRenderable() override;
	void UpdateHealthBar();
	void RunCorrectivePhysics() override;
	void RunWorldPhysics() override;
	void RunEntityPhysics() override;
	void SetPosition(Vector2 newPos, Map* newMap = nullptr) override;
	void CheckTargetStatus();
	void EnterTile(Tile* tile) override;

	void SetFollowTarget(Actor* actorToFollow);

	void Speak();
	void SpeakToOtherActor();

	virtual void TakeDamage(int dmg);

	void EquipOrUnequipItem(Item* itemToEquip);

	Renderable2D* m_healthRenderable;
	AABB2 m_healthBox;
	AABB2 m_healthBarBG;
	AABB2 m_currentHealthBar;

	bool m_isPlayer = false;
	bool m_godMode = false;
	bool m_dead = false;
	float m_speakRadius = 1.f;

	ActorDefinition* m_definition;
	float m_timeLastUpdatedDirection;
	bool m_moving;
	//std::vector<Texture*> m_layerTextures;
	ClothingSet* m_currentLook;
	Item* m_equippedItems[NUM_EQUIP_SLOTS];
	Stats m_stats;
	Stats m_baseStats;
	bool m_changedClothes;
	eAIBehavior m_defaultBehavior = BEHAVIOR_WANDER;
	eAIBehavior m_currentBehavior = BEHAVIOR_WANDER;

	Actor* m_followTarget = nullptr;
	Actor* m_attackTarget = nullptr;
	float m_lastFoundTarget = 0.f;
	float m_targetUpdateRate = 3.f;


	DialogueSet* m_dialogue;
	Quest* m_questGiven = nullptr;
	//std::queue<Dialogue*> m_dialogue;
	//Dialogue* m_activeDialogue;

	void StartFiringArrow();
	
	static std::string GetEquipSlotByID(EQUIPMENT_SLOT equipID);

	//std::vector<SpriteAnimSet*> m_animSets;		//collection of all anim sets - m_animSets[0] = m_animSet;


protected:
	void ParseLayersElement();

	float m_lastAttacked;
	void UpdateWithController(float deltaSeconds);
	void RunSimpleAI(float deltaSeconds);
	void Wander(float deltaSeconds);
	void UpdateBehavior();
	void FollowPlayer(float deltaSeconds);
	void AttackEnemyActor(float deltaSeconds);	//attacks the closest enemy not of your faction

	//finds closest enemy actor every 3 seconds
	void UpdateAttackTarget();

	void RenderDialogue();

	virtual void FireArrow();

	void CheckActorForCollision(Actor* actor);
	void GetRandomStatsFromDefinition();

	void UpdateStats();
	bool m_isFiring;
};