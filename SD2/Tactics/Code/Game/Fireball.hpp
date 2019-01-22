#include "Game/Projectile.hpp"

class Fireball : public Projectile{
public:
	Fireball(Map* map, Vector3 startPosition, Vector3 target, int damage, float maxRange, int areaRange, int verticalRange, float gravity = 9.8f, float cosmeticOffset = .5f);

	void Update(float deltaSeconds) override;
	void Render() override;

	int m_areaRange = 0;
	int m_verticalRange = 0;
	IntVector2 m_targetTile;


};
