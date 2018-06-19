#include "Engine/Math/AreaMask.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/SmoothNoise.hpp"

AreaMask_Rectangle::AreaMask_Rectangle(IntVector2 center, IntVector2 dimensions)
{
	m_center = center;
	m_halfDims = dimensions;
	m_xRange = IntRange(m_center.x - m_halfDims.x, m_center.x + m_halfDims.x);
	m_yRange = IntRange(m_center.y - m_halfDims.y, m_center.y + m_halfDims.y);
}

IntVector2 AreaMask_Rectangle::GetRandomPointInArea() const
{
	return IntVector2(m_xRange.GetRandomInRange(), m_yRange.GetRandomInRange());
}

bool AreaMask::IsPointInside(IntVector2 xyCoords) const
{
	return IsPointInside(xyCoords.x, xyCoords.y);
}

bool AreaMask_Rectangle::IsPointInside(int x, int y) const
{
	return (m_xRange.IsIntInRange(x) && m_yRange.IsIntInRange(y));
}

IntVector2 AreaMask_Rectangle::GetMins() const
{
	return m_center - m_halfDims;
}

IntVector2 AreaMask_Rectangle::GetMaxs() const
{
	return m_center + m_halfDims;
}

float AreaMask_Rectangle::GetDistanceFromEdge(int x, int y) const
{
	IntVector2 point = IntVector2(x,y);
	IntVector2 distFromMins = point - GetMins();
	IntVector2 distFromMaxs = GetMaxs() - point;
	int minX = Min(abs(distFromMins.x), abs(distFromMaxs.x));
	int minY = Min(abs(distFromMins.y), abs(distFromMaxs.y));
	return (float) Min(minX, minY);
}

AreaMask* AreaMask_Rectangle::GetSubArea(FloatRange centerRange, FloatRange sizeRange) const
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
	if (!IsPointInside(mins.x, mins.y)){
		int distance = GetDistanceFromEdge(mins.x, mins.y);
		halfDims-= IntVector2(distance, distance);
	}
	IntVector2 maxs = center + halfDims;
	if (!IsPointInside(maxs.x, maxs.y)){
		int distance = GetDistanceFromEdge(mins.x, mins.y);
		halfDims-= IntVector2(distance, distance);
	}
	return (AreaMask*) (new AreaMask_Rectangle(center, halfDims));
}



AreaMask_Circle::AreaMask_Circle(IntVector2 center, int radius)
{
	m_center = center;
	m_radius = radius;
}

IntVector2 AreaMask_Circle::GetRandomPointInArea() const
{
	int x = GetRandomIntLessThan(m_radius + 1);
	int y = GetRandomIntLessThan(m_radius + 1);
	return IntVector2();
}

bool AreaMask_Circle::IsPointInside(int x, int y) const
{
	IntVector2 point = IntVector2(x,y);
	IntVector2 fromCenter = point - m_center;
	float distSquared = fromCenter.GetVector2().GetLengthSquared();
	return distSquared <= (m_radius * m_radius);
}

float AreaMask_Circle::GetDistanceFromEdge(int x, int y) const
{
	IntVector2 point = IntVector2(x,y);
	IntVector2 fromCenter = point - m_center;
	//float radius = m_halfDims.GetVector2().GetLength();
	float dist = fromCenter.GetVector2().GetLength();
	return abs((int)(dist - m_radius));
}

AreaMask * AreaMask_Circle::GetSubArea(FloatRange centerRange, FloatRange sizeRange) const
{
	int diameter =  m_radius * 2;

	float centerX = centerRange.GetRandomInRange();
	float centerY = centerRange.GetRandomInRange();
	IntVector2 center = m_center + IntVector2((int) (centerX * m_radius), (int) (centerY * m_radius));
	float radPerc = sizeRange.GetRandomInRange();
	int newRadius = (int) (radPerc * diameter);		//or should be multiplying by existing radius?

	//clamp the sub-area to the current area

	IntVector2 mins = center - IntVector2(newRadius,newRadius);
	if (!IsPointInside(mins.x, mins.y)){
		int distance = GetDistanceFromEdge(mins.x, mins.y);
		newRadius-=distance;
	}
	IntVector2 maxs = center + IntVector2(newRadius,newRadius);
	if (!IsPointInside(maxs.x, maxs.y)){
		int distance = GetDistanceFromEdge(mins.x, mins.y);
		newRadius-=distance;
	}
	return (AreaMask*) (new AreaMask_Circle(center, newRadius));
}

AreaMask_Perlin::AreaMask_Perlin(FloatRange acceptableRange, IntVector2 mapMins, IntVector2 mapMaxs, unsigned int seed, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale)
	: m_acceptableRange(0.f), m_xRange(0), m_yRange(0)
{
	m_acceptableRange = acceptableRange;
	m_xRange = IntRange(mapMins.x, mapMaxs.x);
	m_yRange = IntRange(mapMins.y, mapMaxs.y);
	m_seed = seed;
	m_noiseScale = scale;
	m_numOctaves = numOctaves;
	m_octavePersistence = octavePersistence;
	m_octaveScale = octaveScale;
}

IntVector2 AreaMask_Perlin::GetRandomPointInArea() const
{
	int x = m_xRange.GetRandomInRange();
	int y = m_yRange.GetRandomInRange();
	int tries = 0;
	while (!IsPointInside(x,y) && tries < 3000){
		int x = m_xRange.GetRandomInRange();
		int y = m_yRange.GetRandomInRange();
	}

	return IntVector2(x,y);
}

bool AreaMask_Perlin::IsPointInside(int x, int y) const
{
	float noiseVal = GetNoiseAtPoint(x,y);
	if (m_acceptableRange.IsValueInRangeInclusive(noiseVal)){
		return true;
	} else {
		return false;
	}
}

//for perlin, this is an estimation
float AreaMask_Perlin::GetDistanceFromEdge(int x, int y) const
{
	int mapWidth = m_xRange.max - m_xRange.min;
	int mapHeight = m_yRange.max - m_yRange.min;
	int maxSide = Max(mapWidth, mapHeight);
	float tileVariation = (float) maxSide / m_noiseScale;	//should be the max difference in value between tiles

	//get how far outside of the edge of the "acceptable range" you are
	float noiseVal = GetNoiseAtPoint(x,y);
	float distanceFromAccepted = m_acceptableRange.GetDistanceFromEdge(noiseVal);

	return distanceFromAccepted * tileVariation * 2.f;	//return distance based on average tile variation
}

bool AreaMask_Perlin::IsOnEdge(int x, int y) const
{
	int mapWidth = m_xRange.max - m_xRange.min;
	int mapHeight = m_yRange.max - m_yRange.min;
	int maxSide = Max(mapWidth, mapHeight);
	float tileVariation = (float) maxSide / m_noiseScale;	//should be the max distance
	return GetDistanceFromEdge(x,y) < tileVariation;
}

float AreaMask_Perlin::GetNoiseAtPoint(int x, int y) const
{
	float noiseVal = Compute2dPerlinNoise((float) x, (float) y, m_noiseScale, m_numOctaves, m_octavePersistence, m_octaveScale, true, m_seed);
	return noiseVal;
}
