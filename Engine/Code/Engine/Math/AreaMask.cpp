#include "Engine/Math/AreaMask.hpp"
#include "Engine/Math/IntRange.hpp"

AreaMask::AreaMask(IntVector2 center, IntVector2 dimensions)
{
	m_center = center;
	m_halfDims = dimensions;
	m_xRange = IntRange(m_center.x - m_halfDims.x, m_center.x + m_halfDims.x);
	m_yRange = IntRange(m_center.y - m_halfDims.y, m_center.y + m_halfDims.y);
}

IntVector2 AreaMask::GetRandomPointInArea() const
{
	return IntVector2(m_xRange.GetRandomInRange(), m_yRange.GetRandomInRange());
}

bool AreaMask::IsPointInside(IntVector2 xyCoords) const
{
	return IsPointInside(xyCoords.x, xyCoords.y);
}

bool AreaMask::IsPointInside(int x, int y) const
{
	switch(m_type){
	case (AREA_TYPE_CIRCLE):
		return IsPointInsideCircle(x,y);
		break;
	case (AREA_TYPE_RECTANGLE):
		return IsPointInsideRectangle(x,y);
	}
	
	return false;
}

IntVector2 AreaMask::GetMins() const
{
	return m_center - m_halfDims;
}

IntVector2 AreaMask::GetMaxs() const
{
	return m_center + m_halfDims;
}

int AreaMask::GetDistanceFromEdge(int x, int y) const
{
	switch(m_type){
	case (AREA_TYPE_CIRCLE):
		return GetDistanceFromEdgeCircle(x,y);
		break;
	case (AREA_TYPE_RECTANGLE):
		return GetDistanceFromEdgeRectangle(x,y);
	}
	return -1;
}

AreaMask AreaMask::GetSubArea(FloatRange centerRange, FloatRange sizeRange)
{
	int width = m_halfDims.x * 2;
	int height = m_halfDims.y * 2;

	float centerX = centerRange.GetRandomInRange();
	float centerY = centerRange.GetRandomInRange();
	IntVector2 center = GetMins() + IntVector2((int) (centerX * width), (int) (centerY * height));
	float dimX = sizeRange.GetRandomInRange();
	float dimY = sizeRange.GetRandomInRange();
	IntVector2 halfDims = IntVector2((int) (dimX * width), (int) (dimY * height));

	//clamp the sub-area to the actual box
	
	IntVector2 mins = center - halfDims;
	if (!IsPointInside(mins)){
		int distance = GetDistanceFromEdge(mins.x, mins.y);
		halfDims-= IntVector2(distance, distance);
	}
	IntVector2 maxs = center + halfDims;
	if (!IsPointInside(maxs)){
		int distance = GetDistanceFromEdge(mins.x, mins.y);
		halfDims-= IntVector2(distance, distance);
	}
	return AreaMask(center, halfDims);
}

bool AreaMask::IsPointInsideCircle(int x, int y) const
{
	IntVector2 point = IntVector2(x,y);
	IntVector2 fromCenter = point - m_center;
	float radius = m_halfDims.GetVector2().GetLength();
	float dist = fromCenter.GetVector2().GetLength();
	return dist <= radius;
}

bool AreaMask::IsPointInsideRectangle(int x, int y) const
{
	return (m_xRange.IsIntInRange(x) && m_yRange.IsIntInRange(y));
}

int AreaMask::GetDistanceFromEdgeCircle(int x, int y) const
{
	IntVector2 point = IntVector2(x,y);
	IntVector2 fromCenter = point - m_center;
	float radius = m_halfDims.GetVector2().GetLength();
	float dist = fromCenter.GetVector2().GetLength();
	return abs((int)(dist - radius));
}

int AreaMask::GetDistanceFromEdgeRectangle(int x, int y) const
{
	IntVector2 point = IntVector2(x,y);
	IntVector2 distFromMins = point - GetMins();
	IntVector2 distFromMaxs = GetMaxs() - point;
	int minX = Min(abs(distFromMins.x), abs(distFromMaxs.x));
	int minY = Min(abs(distFromMins.y), abs(distFromMaxs.y));
	return Min(minX, minY);
}
