#include "Disc2.hpp"
#include <stdio.h>

Disc2::Disc2(const Disc2 & copyFrom)
{
	center = Vector2(copyFrom.center);
	radius = copyFrom.radius;

}

Disc2::Disc2(float initialX, float initialY, float initialRadius)
{
	center = Vector2(initialX, initialY);
	radius = initialRadius;
}

Disc2::Disc2(const Vector2 & initialCenter, float initialRadius)
{
	center = initialCenter;
	radius = initialRadius;
}

void Disc2::StretchToIncludePoint(float x, float y)
{
	float dist = GetDistance(center, Vector2(x,y));
	if (radius < dist){
		radius = dist;
	}
	//TODO
}

void Disc2::StretchToIncludePoint(const Vector2 & point)
{
	float dist = GetDistance(center, point);
	if (radius < dist){
		radius = dist;
	}
	//TODO
}

void Disc2::AddPaddingToRadius(float paddingRadius)
{
	//TODO
	radius+=paddingRadius;
}

void Disc2::Translate(const Vector2 & translation)
{
	this->center+=translation;
}

void Disc2::Translate(float translationX, float translationY)
{
	this->center.x+=translationX;
	this->center.y+=translationY;
}

//should these return true if on the radius?
bool Disc2::IsPointInside(float x, float y) const
{
	float dist = GetDistance(center, Vector2(x,y));
	return dist <= radius;
}

bool Disc2::IsPointInside(const Vector2 & point) const
{
	float dist = GetDistance(center, point);
	return dist <= radius;
}

bool Disc2::DoesAABB2Overlap(const AABB2 & box) const
{
	bool leftOverlap = false;
	bool rightOverlap = false;
	bool upOverlap = false;
	bool downOverlap = false;
	//if the center is inside the box return true
	if (box.IsPointInside(center)){
		return true;
	} else {
		//disc is to the left, check for left overlap
		if (center.x < box.mins.x){
			//if center.x is less than radius away from mins x, and center.y is between mins.y and maxs.y, there's an overlap on the left side
			if((box.mins.x - center.x) < radius){
				if (box.mins.y <= center.y && box.maxs.y >= center.y){
					leftOverlap = true;
				}
			}
		}

		//disc is to the right, check for right overlap
		if (center.x > box.maxs.x){
			//if center.x is less than radius away from maxs.x, and center.y is between mins.y and maxs.y, there's an overlap on the right side
			if((center.x - box.maxs.x) < radius){
				if (box.mins.y <= center.y && box.maxs.y >= center.y){
					rightOverlap = true;
				}
			}
		}

		//disc is to the north, check for up overlap
		if (center.y > box.maxs.y){
			//if center.y is less than radius away from maxs.y, and center.x is between mins.x and maxs.x, there's an overlap on the right side
			if((center.y - box.maxs.y) < radius){
				if (box.mins.x <= center.x && box.maxs.x >= center.x){
					upOverlap = true;
				}
			}
		}

		//disc is to the north, check for up overlap
		if (center.y < box.mins.y){
			//if center.y is less than radius away from maxs.y, and center.x is between mins.x and maxs.x, there's an overlap on the right side
			if((box.mins.y - center.y) < radius){
				if (box.mins.x <= center.x && box.maxs.x >= center.x){
					downOverlap = true;
				}
			}
		}
	}
	return (leftOverlap || rightOverlap || upOverlap || downOverlap);
}

Vector2 Disc2::PushOutOfPoint(const Vector2 & point)
{
	Vector2 distance = center - point;
	Vector2 distanceNormalized = distance.GetNormalized();
	float overlap = radius - distance.GetLength();

	return center + (distanceNormalized * overlap);
}

void Disc2::operator+=(const Vector2 & translation)
{
	center+=translation;
}

void Disc2::operator-=(const Vector2 & antiTranslation)
{
	center-=antiTranslation;
}

Disc2 Disc2::operator+(const Vector2 & translation) const
{
	return Disc2(center + translation, radius);
}

Disc2 Disc2::operator-(const Vector2 & antiTranslation) const
{
	return Disc2(center - antiTranslation, radius);
}




bool DoDiscsOverlap(const Disc2 & a, const Disc2 & b)
{
	float dist = GetDistanceSquared(a.center, b.center);
	float sumRadius = static_cast<float>(fabs(a.radius + b.radius));
	//return dist <= sumRadius;
	return (dist <= (sumRadius * sumRadius));
}

bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius)
{
	float dist = GetDistanceSquared(aCenter, bCenter);
	float sumRadius = static_cast<float>(fabs(aRadius + bRadius));
	//return dist <= sumRadius;
	return (dist <= (sumRadius*sumRadius));
}

const Disc2 Interpolate(const Disc2 & start, const Disc2 & end, float fractionTowardEnd)
{
	Vector2 center = Interpolate(start.center, end.center, fractionTowardEnd);
	float radius = Interpolate(start.radius, end.radius, fractionTowardEnd);
	return Disc2(center, radius);
}
