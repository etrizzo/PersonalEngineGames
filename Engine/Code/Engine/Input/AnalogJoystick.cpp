#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"

float AnalogJoystick::GetMagnitude()
{
	return m_polar.x;
}

float AnalogJoystick::GetAngle()
{
	return m_polar.y;
}

Vector2 AnalogJoystick::GetPositionCartesian()
{
	return m_cartesian;
}

float AnalogJoystick::GetX()
{
	return m_cartesian.x;
}

float AnalogJoystick::GetY()
{
	return m_cartesian.y;
}

void AnalogJoystick::SetJoystick(float inX, float inY)
{

	//convert x,y to (-1.f, 1.f)
	float scaledX = RangeMapFloat(inX, -32768.f, 32767.f, -1.f, 1.f);
	float scaledY = RangeMapFloat(inY, -32768.f, 32767.f, -1.f, 1.f);

	//convert to polar R, theta
	float r = sqrtf((scaledX*scaledX) + (scaledY*scaledY));
	float theta = Atan2Degreesf(scaledY, scaledX);

	//remap live zone to R in [0,1] (save corrected r)
	float correctedR = RangeMapFloat(r, m_innerDeadZone, m_outerDeadZone, 0.f, 1.f);
	ClampZeroToOne(correctedR);
	m_polar = Vector2(correctedR, theta);

	//convert back to corrected cartesian and save
	float correctedX = correctedR * CosDegreesf(theta);
	float correctedY = correctedR * SinDegreesf(theta);
	m_cartesian = Vector2(correctedX, correctedY);


}
