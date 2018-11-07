#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"


class OBB2
{
public:
	// Constructors/Destructors
	~OBB2(){};
	OBB2(){};
	explicit OBB2(const Vector2& centerPoint, const Vector2& right, const Vector2& halfDims);
	explicit OBB2(const Vector2& centerPoint, const Vector2& right, float halfSizeX, float halfSizeY);


	bool operator==( const OBB2& compare ) const;				
	bool operator!=( const OBB2& compare ) const;			

	Vector2 GetUp() const;
	Vector2 GetLocalMins() const;
	Vector2 GetLocalMaxs() const;
	Vector2 GetWorldMins() const;
	Vector2 GetWorldMaxs() const;
	Vector2 GetWorldTopLeft() const;
	Vector2 GetWorldBottomRight() const;

	Vector2 GetScaledRight() const;
	Vector2 GetScaledUp() const;

	//// Mutators
	//void StretchToIncludePoint(float x, float y);
	//void StretchToIncludePoint(const Vector2& point);
	//void AddPaddingToSides(float xPaddingRadius, float yPaddingRadius);
	//void Translate(const Vector2& translation);
	//void Translate(float translationX, float translationY);
	//void UniformScaleFromCenter(float scale);
	//void TrimToAspectRatio(float aspect);	//shrinks to desired aspect ratio
	//void ExpandToAspectRatio(float aspect);	//expands to desired aspect ratio
	//void TrimToSquare();	//trims to shortest side length to a perfect square(preserves center);
	//void ExpandToSquare();	//expands to longest side length to a perfect square (preserves center)

	//void SetFromText(const char* text);

	////splits into 2 AABBs hamburger style
	//void SplitAABB2Vertical(float xDivide, AABB2 &outLeft, AABB2 &outRight);
	////splits into 2 AABBs hotdog style
	//void SplitAABB2Horizontal(float yDivide, AABB2 &outTop, AABB2 &outBottom);

	////gets a portion of the AABB2 by percentages - useful for UI boxes in screen space
	//OBB2 GetPercentageBox(float xPercentMin, float yPercentMin, float xPercentMax,  float yPercentMax) const;
	//OBB2 GetPercentageBox(Vector2 minPercentages, Vector2 maxPercentages) const;

	////gets percentage into the box the point is
	//Vector2 GetPercentageOfPoint(Vector2 point) const;
	//Vector2 GetPointAtNormalizedCoord(Vector2 normalizedPoint) const;
	//Vector2 GetPointAtNormalizedCoord(float x, float y) const;


	//Accessors/queries
	bool IsPointInside(float x, float y) const;
	bool IsPointInside(const Vector2& point) const;
	Vector2 GetDimensions() const;
	//Vector2 GetCenter() const;
	float GetWidth() const;
	float GetHeight() const;
	//Vector2 GetRandomPointInBox() const;
	//Vector2 GetClosestPointOnEdge(float x, float y) const;
	//Vector2 GetClosestPointOnEdge(Vector2 point) const;

	float GetAspect() const;

	////Operators
	//void operator+=(const Vector2& translation);
	//void operator-=(const Vector2& antiTranslation);
	//OBB2 operator+(const Vector2& translation) const;
	//OBB2 operator-(const Vector2& antiTranslation) const;

public:
	Vector2 center;
	Vector2 normalizedRight;
	Vector2 halfDimensions; 

};

//bool DoAABBsOverlap(const AABB2& a, const AABB2& b);
OBB2 Interpolate( const OBB2& start, const OBB2& end, float fractionTowardEnd);

