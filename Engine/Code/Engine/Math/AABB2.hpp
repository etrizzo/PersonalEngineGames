#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"


class AABB2
{
public:
	// Constructors/Destructors
	~AABB2(){};
	AABB2(){};
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2(const Vector2 & minsXY, const Vector2 & maxsXY);
	explicit AABB2(const Vector2& center, float radiusX, float radiusY);


	// Mutators
	void StretchToIncludePoint(float x, float y);
	void StretchToIncludePoint(const Vector2& point);
	void AddPaddingToSides(float xPaddingRadius, float yPaddingRadius);
	void Translate(const Vector2& translation);
	void Translate(float translationX, float translationY);
	void UniformScaleFromCenter(float scale);

	void SetFromText(const char* text);

	//splits into 2 AABBs hamburger style
	void SplitAABB2Vertical(float xDivide, AABB2 &outLeft, AABB2 &outRight);
	//splits into 2 AABBs hotdog style
	void SplitAABB2Horizontal(float yDivide, AABB2 &outTop, AABB2 &outBottom);

	//gets a portion of the AABB2 by percentages - useful for UI boxes in screen space
	AABB2 GetPercentageBox(float xPercentMin, float yPercentMin, float xPercentMax,  float yPercentMax);
	AABB2 GetPercentageBox(Vector2 xPercentages, Vector2 yPercentages);


	//Accessors/queries
	bool IsPointInside(float x, float y) const;
	bool IsPointInside(const Vector2& point) const;
	Vector2 GetDimensions() const;
	Vector2 GetCenter() const;
	float GetWidth() const;
	float GetHeight() const;
	Vector2 GetRandomPointInBox() const;

	float GetAspect() const;

	//Operators
	void operator+=(const Vector2& translation);
	void operator-=(const Vector2& antiTranslation);
	AABB2 operator+(const Vector2& translation) const;
	AABB2 operator-(const Vector2& antiTranslation) const;

public:
	Vector2 mins;
	Vector2 maxs;

	const static AABB2 ZERO_TO_ONE;
};

bool DoAABBsOverlap(const AABB2& a, const AABB2& b);
AABB2 Interpolate( const AABB2& start, const AABB2& end, float fractionTowardEnd);



//stores 4 points instead of 2 
//used for 3D planes because there's infinite planes between mins and maxs
//(could you use just a real flat cube??)
struct Plane{
public:
	Plane(){};
	Plane(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2& size = Vector2::ONE);
	Plane(Vector3 nearBottomLeft, Vector3 nearBottomRight, Vector3 farTopRight, Vector3 farTopLeft);
	~Plane(){};

	Vector3 m_center;
	Vector3 m_up;
	Vector3 m_right;
	Vector2 m_size;


	//for backwards compatibility with tactics
	Vector3 m_nearBottomLeft;
	Vector3 m_nearBottomRight;
	Vector3 m_farTopRight;
	Vector3 m_farTopLeft;

	Vector3 GetCenter() const;
	AABB2 GetAABB2() const;
};