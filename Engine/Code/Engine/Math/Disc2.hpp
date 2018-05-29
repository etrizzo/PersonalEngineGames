#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"


class Disc2
{
public:
	//constructors / destructors
	~Disc2(){};
	Disc2(){};
	Disc2(const Disc2& copyFrom);
	explicit Disc2(float initialX, float initialY, float initialRadius);
	explicit Disc2(const Vector2& initialCenter, float initialRadius);


	//Mutators
	void StretchToIncludePoint(float x, float y);
	void StretchToIncludePoint( const Vector2& point);
	void AddPaddingToRadius(float paddingRadius);
	void Translate(const Vector2& translation);
	void Translate(float translationX, float translationY);

	//Accessors/queries
	bool IsPointInside(float x, float y) const;
	bool IsPointInside(const Vector2& point) const;
	bool DoesAABB2Overlap(const AABB2& box) const;

	Vector2 PushOutOfPoint(const Vector2& point);

	//Operators
	void operator+=(const Vector2& translation);
	void operator-=(const Vector2& antiTranslation);
	Disc2 operator+(const Vector2& translation) const;
	Disc2 operator-(const Vector2& antiTranslation) const;


public:
	Vector2 center;
	float radius;
};


//----------------
// DISC FUNCTIONS
//----------------

bool DoDiscsOverlap(const Disc2& a, const Disc2& b);
bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius);

const Disc2 Interpolate( const Disc2& start, const Disc2& end, float fractionTowardEnd );