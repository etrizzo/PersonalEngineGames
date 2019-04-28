#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

class Asteroid;

#define FLOWER_POT_HEALTH (10)
constexpr float FLOWERPOT_DEPTH = 15.f;
constexpr float FLOWERPOT_WIDTH = 3.f;
constexpr float RESUPPLY_WIDH = .9f;

class FlowerPot : public Entity
{
public:
	//sets up the pot, flower sprite, collider, and resupply sprite at the right positions
	FlowerPot(float xPosition);
	~FlowerPot();

	void Update() override;
	void Render() override;

	void TakeDamage() override;

	void Reload(int numToReload = 1);

	bool IsDead() const override;

private:
	//update loop
	void FindNewTarget();
	void TurnTowardsTarget();
	void BeginAttack() override;
	void ExecuteAttack() override;






public:
	int m_numBullets = FLOWER_POT_HEALTH;
	//int m_health = FLOWER_POT_HEALTH;

private:
	void UpdateFlowerAnimation();

	StopWatch m_rateOfFire;
	Asteroid* m_target = nullptr;

	//the sprite for the resupply point that appears on the walkable plane
	Sprite* m_flowerSprite = nullptr;
	
	//renderable for the pot
	Renderable* m_flowerPotRenderable = nullptr;

	SpriteAnimSet* m_flowerSpriteAnimSet = nullptr;
};