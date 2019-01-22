#pragma once
#include "Game/Actor.hpp"




class Archer: public Actor{
public:
	Archer(){};
	Archer(IntVector2 initialPosition, Map* entityMap, Faction faction);

	void Render() override;
	void RenderProcessTurn() override;
	void RenderHoverData(Actor* target) override;

	void Attack(IntVector2 targetPos) override;

	void TakeDamage(int damage, float critChance = 0.f, float blockChance = 0.f) override;

	Menu* MakeMenu() override;
	void ProcessTurn() override;
	bool HandleTurnInputAndCheckCursorMovement() override;

	float GetBlockChance(Actor* target) override;

	void SetBowArea();

	void BowAttack(IntVector2 targetPos);

	Sprite* GetDisplaySprite() const override;

	float m_minRange = 2.f;
	float m_maxRange = 5.f;

	

private:
	void RenderTrajectoryPath();


};