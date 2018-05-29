#include "Engine/Core/EngineCommon.hpp"

class Trajectory{
public:
	static Vector2 Evaluate(float gravity, Vector2 launchVelocity, float time);
	static Vector2 Evaluate(float gravity, float launchSpeed, float launchAngle, float time);

	//returns minimum launch speed - assumes optimal angle of 45 degrees
	static float GetMinimumLaunchSpeed(float gravity, float distance);

	//given fixed launch speed, determines angle(s) needed to hit target distance away at height. 
	static bool GetLaunchAngles(Vector2* out, float gravity, float launchSpeed, float distance, float height = 0.f);

	//given a launch speed and distance, return max possible height
	static float GetMaxHeight(float gravity, float launchSpeed, float distance);

	//given a target apex height (highest point on arc) and target dist/height, determine launch velocity. apexHeight > height.
	static Vector2 GetLaunchVelocity(float gravity, float apexHeight, float distance, float height);
};