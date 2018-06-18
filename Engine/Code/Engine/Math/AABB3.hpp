#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"


class AABB3
{
public:
	// Constructors/Destructors
	~AABB3(){};
	AABB3(){};
	explicit AABB3(float nearminX, float nearminY,float nearminz, float farmaxX, float farmaxY, float farmaxZ);
	explicit AABB3(const Vector3 & nearMins, const Vector3 & farMaxs);
	explicit AABB3(const Vector3& center, float radiusX, float radiusY, float radiusZ);


	// Mutators
	void StretchToIncludePoint(float x, float y, float z);
	void StretchToIncludePoint(const Vector3& point);
	void AddPaddingToSides(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius);
	void Translate(const Vector3& translation);
	void Translate(float translationX, float translationY, float translationZ);

	void SetFromText(const char* text);

	//not necessary for now
	////splits into 2 AABBs hamburger style
	//void SplitAABB3Vertical(float xDivide, AABB3 &outLeft, AABB3 &outRight);
	////splits into 2 AABBs hotdog style
	//void SplitAABB3Horizontal(float yDivide, AABB3 &outTop, AABB3 &outBottom);


	//Accessors/queries
	bool IsPointInside(float x, float y, float z) const;
	bool IsPointInside(const Vector3& point) const;
	Vector3 GetDimensions() const;
	Vector3 GetCenter() const;
	float GetWidth() const;
	float GetHeight() const;
	Vector2 GetRandomPointInBox() const;

	//Operators
	void operator+=(const Vector3& translation);
	void operator-=(const Vector3& antiTranslation);
	AABB3 operator+(const Vector3& translation) const;
	AABB3 operator-(const Vector3& antiTranslation) const;

public:
	Vector3 mins;		//near bottom left
	Vector3 maxs;		//far top right

	const static AABB3 NEGONE_TO_ONE;
};

bool DoAABBsOverlap(const AABB3& a, const AABB3& b);
AABB3 Interpolate( const AABB3& start, const AABB3& end, float fractionTowardEnd);