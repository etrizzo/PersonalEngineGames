#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"

enum eAreaType{
	AREA_TYPE_RECTANGLE,
	AREA_TYPE_CIRCLE,
	AREA_TYPE_PERLIN,
	NUM_AREA_TYPES
};

class AreaMask{
public:
	AreaMask() {};
	AreaMask(IntVector2 center, IntVector2 halfDimensions);

	IntVector2 m_center = IntVector2(0,0);
	IntVector2 m_halfDims = IntVector2(0,0);

	IntRange m_xRange = IntRange(0);
	IntRange m_yRange = IntRange(0);	//generated on construction

	eAreaType m_type = AREA_TYPE_RECTANGLE; 

	IntVector2 GetRandomPointInArea() const;
	virtual bool IsPointInside(IntVector2 xyCoords) const;
	virtual bool IsPointInside(int x, int y) const;
	IntVector2 GetMins() const;
	IntVector2 GetMaxs() const;
	virtual int GetDistanceFromEdge(int x, int y) const;

	AreaMask GetSubArea(FloatRange centerRange, FloatRange sizeRange);
private:
	bool IsPointInsideCircle(int x, int y) const ;
	bool IsPointInsideRectangle(int x, int y) const ;

	int GetDistanceFromEdgeCircle(int x, int y) const ;
	int GetDistanceFromEdgeRectangle(int x, int y) const ;
};

//class AreaMask_Rectangle{
//
//};