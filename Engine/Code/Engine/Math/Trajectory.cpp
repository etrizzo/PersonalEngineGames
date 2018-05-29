#include "Engine/Math/Trajectory.hpp"

Vector2 Trajectory::Evaluate(float gravity, Vector2 launchVelocity, float time)
{
	return Evaluate(gravity, launchVelocity.x, launchVelocity.y, time);
}

Vector2 Trajectory::Evaluate(float g, float v, float theta, float t)
{
	return Vector2(v * CosDegreesf(theta) * t, (-.5f * g * (t * t) ) + (v * SinDegreesf(theta)  * t));
}

float Trajectory::GetMinimumLaunchSpeed(float gravity, float distance)
{
	return sqrtf(gravity * distance);
}

bool Trajectory::GetLaunchAngles(Vector2 * out, float gravity, float launchSpeed, float distance, float height)
{
	//src: https://en.wikipedia.org/wiki/Projectile_motion#Angle_required_to_hit_coordinate_.28x.2Cy.29

	if (launchSpeed == 0.f || gravity == 0.f || distance == 0.f){
		return false;
	}
	float vSqrd = (launchSpeed * launchSpeed);
	float oneOverGX = 1.f / (gravity * distance);
	float innersqrt = sqrtf((vSqrd*vSqrd) - (gravity * (((gravity * (distance *distance)) + (2.f * height * (vSqrd))))));
	float theta1 = AtanDegreesf((vSqrd + innersqrt) * oneOverGX);
	float theta2 = AtanDegreesf((vSqrd - innersqrt) * oneOverGX);


	out->x = theta1;
	out->y = theta2;
	if (isnan(theta1) || isnan(theta2)){
		return false;
	}
	return true;
}

float Trajectory::GetMaxHeight(float gravity, float launchSpeed, float distance)
{
	Vector2 thetas = Vector2::ZERO;
	Trajectory::GetLaunchAngles(&thetas, gravity, launchSpeed, distance);
	float maxTheta = max(thetas.x, thetas.y);
	float sinTheta = SinDegreesf(maxTheta);
	float div = 1.f / (2.f * gravity);

	return (((launchSpeed * launchSpeed) * ( sinTheta * sinTheta)) * div);
}

