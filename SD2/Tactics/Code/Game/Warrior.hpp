#include "Game/Actor.hpp"

class Warrior: public Actor{
public:
	Warrior(){};
	Warrior(IntVector2 initialPosition, Map* entityMap, Faction faction);

	void Render() override;

	void Attack(IntVector2 targetPos) override;
	void TakeDamage(int damage, float critChance = 0.f, float blockChance = 0.f) override;

	Menu* MakeMenu() override;
	void ProcessTurn() override;
	bool HandleTurnInputAndCheckCursorMovement() override;

	float GetBlockChance(Actor* target) override;
	bool IsDefending() const override {return m_isDefending;}

	Sprite* GetDisplaySprite() const override;

protected:
	bool m_isDefending = false;
	
};