#include "Game/Entity.hpp"

class Asteroid;

class Missile : public Entity
{
public:
	Missile(Vector3 position, Asteroid* target);
	~Missile();

	void Update() override;
	void Render() override;

protected:
	void KillTarget();

	Renderable* m_renderable = nullptr;

	Asteroid* m_target = nullptr;
	Light* m_light = nullptr;
	float m_speed = 15.f;
};