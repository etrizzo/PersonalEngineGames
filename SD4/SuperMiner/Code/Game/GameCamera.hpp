#include "Game/GameCommon.hpp"

class Entity;

constexpr float THIRD_PERSON_ARM_LENGTH = 4.0f;

class GameCamera
{
public:
	GameCamera(Camera* camera, Entity* owner);

	void Update();

	Camera* GetCamera()		{ return m_camera; };
	Entity* GetOwner()		{ return m_myDaddy; };
	eCameraMode GetMode()	{ return m_mode; };

	void SetOwner(Entity* owner);
	void SetMode(eCameraMode newMode);
private:
	Camera* m_camera;
	eCameraMode m_mode;
	Entity* m_myDaddy;


	void UpdateFirstPerson();
	void UpdateThirdPerson();

};