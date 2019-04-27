#include "Game/Entity.hpp"

class Asteroid : public Entity
{
public:
	Asteroid(Vector3 position, Vector3 target);

	void Update() override;
	void Render() override;

	bool m_isTargeted = false;
protected:
	void Explode();

	Vector3 m_targetPosition;
	float m_speed = 4.f;
	
};