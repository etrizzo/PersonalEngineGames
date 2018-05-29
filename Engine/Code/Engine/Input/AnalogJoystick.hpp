#include "Engine/Math/Vector2.hpp"

class AnalogJoystick{
	float m_innerDeadZone = .3f;
	float m_outerDeadZone = .9f;
	Vector2 m_cartesian;
	Vector2 m_polar;

public:
	float GetMagnitude();

	float GetAngle();

	Vector2 GetPositionCartesian();

	float GetX();
	float GetY();

	void SetJoystick(float inX, float inY);
};