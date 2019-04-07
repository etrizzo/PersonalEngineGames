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
	std::string GetModeString();

	void SetOwner(Entity* owner);
	void SetMode(eCameraMode newMode);
private:
	Camera* m_camera;
	eCameraMode m_mode;
	Entity* m_myDaddy;


	void UpdateFirstPerson();
	void UpdateThirdPerson();
	void UpdateFixedAngle();

	void FinishMode();
	void InitForNewMode();

	Vector3 m_fixedAngleOffset = Vector3(6.f, 4.f, 5.f);
};