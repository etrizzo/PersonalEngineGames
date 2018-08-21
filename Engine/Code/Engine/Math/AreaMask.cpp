#include "Engine/Math/AreaMask.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Math/AABB2.hpp"

bool AreaMask::IsOnEdge(int x, int y) const
{
	return GetDistanceFromEdge(x,y) <= 1.f;
}

AreaMask * AreaMask::GetSubArea(FloatRange centerRange, FloatRange sizeRange) const
{
	return nullptr;
}

int AreaMask::GetWidth() const
{
	return GetMaxs().x - GetMins().x;
}

int AreaMask::GetHeight() const
{
	return GetMaxs().y - GetMins().y;
}

IntVector2 AreaMask::GetMins() const
{
	return IntVector2();
}

IntVector2 AreaMask::GetMaxs() const
{
	return IntVector2();
}

bool AreaMask::CanDrawOnPoint(int x, int y) const
{
	float chanceToDraw = GetChanceAtPoint(x,y);
	return CheckRandomChance(chanceToDraw);
}

bool AreaMask::CanDrawOnPoint(IntVector2 xyCoords) const
{
	return CanDrawOnPoint(xyCoords.x, xyCoords.y);
}

void AreaMask::SetDensity(float centerDensity, float edgeDensity, float featherRate)
{
	m_centerDensity = centerDensity;
	m_edgeDensity = edgeDensity;
	m_featherRate = featherRate;
}

float AreaMask::GetChanceAtPoint(int x, int y, float existingChance) const
{
	float distanceFromCenter = GetDistanceFromCenter(x,y);
	float maxFromCenter = GetMaxDistanceFromCenter();
	float densityFactor = RangeMapFloat(distanceFromCenter, 0.f, maxFromCenter, m_centerDensity,  m_edgeDensity);
	float featherFactor = 1.f;
	if (!IsPointInside(x,y)){
		float distanceFromEdge = GetDistanceFromEdge(x,y);
		featherFactor = 1 - (m_featherRate * distanceFromEdge); //will make feather factor 0 if outside and no feathering
		featherFactor = ClampFloatZeroToOne(featherFactor);
	}

	return densityFactor * featherFactor * existingChance;
}


AreaMask_Rectangle::AreaMask_Rectangle(IntVector2 center, IntVector2 dimensions, float centerDensity, float edgeDensity, float featherRate)
	:AreaMask(centerDensity, edgeDensity, featherRate)
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

AreaMask::AreaMask(float centerDensity, float edgeDensity, float feather)
{
	m_centerDensity = centerDensity;
	m_edgeDensity = edgeDensity;
	m_featherRate = feather;
}

bool AreaMask::IsPointInside(IntVector2 xyCoords) const
{
	return IsPointInside(xyCoords.x, xyCoords.y);
}

bool AreaMask_Rectangle::IsPointInside(int x, int y) const
{
	//IntVector2 point = IntVector2(x,y);
	//AABB2 box = AABB2(GetMins().GetVector2(), GetMaxs().GetVector2());
	//return box.IsPointInside(point.GetVector2());
	//bool gtMins = point.GetVector2() > GetMins().GetVector2();
	//bool ltMaxs = point.GetVector2() < GetMaxs().GetVector2();
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
	AABB2 box = AABB2(GetMins().GetVector2(), GetMaxs().GetVector2());
	Vector2 closestPoint = box.GetClosestPointOnEdge((float) x, (float) y);

	Vector2 point = Vector2((float) x, (float) y);
	return GetDistance(point, closestPoint);
	//IntVector2 point = IntVector2(x,y);
	//IntVector2 distFromMins = point - GetMins();
	//IntVector2 distFromMaxs = GetMaxs() - point;

	//IntVector2 closestPointOnEdge = GetClosestPointOnEdge(x,y);
	////int distX = Min(abs(distFromMins.x), abs(distFromMaxs.x));
	////int distY = Min(abs(distFromMins.y), abs(distFromMaxs.y));
	//int distMins = Min(abs(distFromMins.x), abs(distFromMins.y));
	//int distMaxs = Min(abs(distFromMaxs.x), abs(distFromMaxs.y));
	//return (float) Min(distMins, distMaxs);
}

float AreaMask_Rectangle::GetDistanceFromCenter(int x, int y) const
{
	IntVector2 point = IntVector2(x,y);
	IntVector2 offset = point - m_center;
	return offset.GetVector2().GetLength();
}

float AreaMask_Rectangle::GetMaxDistanceFromCenter() const
{
	IntVector2 diagonal = m_center - GetMins();
	return diagonal.GetVector2().GetLength();
}

IntVector2 AreaMask_Rectangle::GetClosestPointOnEdge(int x, int y) const
{
	AABB2 box = AABB2(GetMins().GetVector2(), GetMaxs().GetVector2());
	Vector2 closestPoint = box.GetClosestPointOnEdge((float) x, (float) y);
	
	return IntVector2((int) closestPoint.x, (int) closestPoint.y);
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
		int distance = (int) GetDistanceFromEdge(mins.x, mins.y);
		halfDims-= IntVector2(distance, distance);
	}
	IntVector2 maxs = center + halfDims;
	if (!IsPointInside(maxs.x, maxs.y)){
		int distance = (int) GetDistanceFromEdge(mins.x, mins.y);
		halfDims-= IntVector2(distance, distance);
	}
	return (AreaMask*) (new AreaMask_Rectangle(center, halfDims));
}



AreaMask_Circle::AreaMask_Circle(IntVector2 center, int radius,  float centerDensity, float edgeDensity, float featherRate)
	:AreaMask(centerDensity, edgeDensity, featherRate)
{
	m_center = center;
	m_radius = radius;
}

IntVector2 AreaMask_Circle::GetRandomPointInArea() const
{
	int x = GetRandomIntInRange((int) (-m_radius * .5f), (int) (m_radius * .5f));
	int y = GetRandomIntInRange((int) (-m_radius * .5f), (int) (m_radius * .5f));
	return m_center + IntVector2(x,y);
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
	return (float) abs((int)(dist - m_radius));
}

float AreaMask_Circle::GetDistanceFromCenter(int x, int y) const
{
	IntVector2 point = IntVector2(x,y);
	IntVector2 offset = point - m_center;
	return offset.GetVector2().GetLength();
}

float AreaMask_Circle::GetMaxDistanceFromCenter() const
{
	return (float) m_radius;
}

IntVector2 AreaMask_Circle::GetMins() const
{
	return m_center - IntVector2(m_radius, m_radius);
}

IntVector2 AreaMask_Circle::GetMaxs() const
{
	return  m_center + IntVector2(m_radius, m_radius);
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
		int distance = (int) GetDistanceFromEdge(mins.x, mins.y);
		newRadius-=distance;
	}
	IntVector2 maxs = center + IntVector2(newRadius,newRadius);
	if (!IsPointInside(maxs.x, maxs.y)){
		int distance = (int) GetDistanceFromEdge(mins.x, mins.y);
		newRadius-=distance;
	}
	return (AreaMask*) (new AreaMask_Circle(center, newRadius));
}

AreaMask_Perlin::AreaMask_Perlin(FloatRange acceptableRange, IntVector2 mapMins, IntVector2 mapMaxs, unsigned int seed, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale)
	: m_xRange(0), m_yRange(0)
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
		x = m_xRange.GetRandomInRange();
		y = m_yRange.GetRandomInRange();
		tries++;
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
	//int mapWidth = m_xRange.max - m_xRange.min;
	//int mapHeight = m_yRange.max - m_yRange.min;
	//int maxSide = Max(mapWidth, mapHeight);
	//float tilesPerCycle = m_noiseScale;
	//float tileVariation = (float) 1.f / (m_noiseScale);	//should be the max difference in value between tiles

	//get how far outside of the edge of the "acceptable range" you are
	float noiseVal = GetNoiseAtPoint(x,y);
	float distanceFromAccepted = m_acceptableRange.GetDistanceFromEdge(noiseVal);

	return distanceFromAccepted * m_noiseScale;	//return distance based on average tile variation
}

float AreaMask_Perlin::GetDistanceFromCenter(int x, int y) const
{

	//int mapWidth = m_xRange.max - m_xRange.min;
	//int mapHeight = m_yRange.max - m_yRange.min;
	//int maxSide = Max(mapWidth, mapHeight);
	//float tilesPerCycle = m_noiseScale;
	//float tileVariation = (float) 1.f / (m_noiseScale);	//should be the max difference in value between tiles

														//get how far outside of the edge of the "acceptable range" you are
	float noiseVal = GetNoiseAtPoint(x,y);
	float distanceFromCenter = m_acceptableRange.GetDistanceFromCenter(noiseVal);

	return distanceFromCenter * m_noiseScale;	//return distance based on average tile variation
}

float AreaMask_Perlin::GetMaxDistanceFromCenter() const
{
	return m_acceptableRange.GetSize() * .5f * m_noiseScale;
}

bool AreaMask_Perlin::IsOnEdge(int x, int y) const
{
	int mapWidth = m_xRange.max - m_xRange.min;
	int mapHeight = m_yRange.max - m_yRange.min;
	int maxSide = Max(mapWidth, mapHeight);
	float tileVariation = (float) maxSide / m_noiseScale;	//should be the max distance
	return GetDistanceFromEdge(x,y) < tileVariation;
}

IntVector2 AreaMask_Perlin::GetMins() const
{
	return IntVector2(m_xRange.min, m_yRange.min);
}

IntVector2 AreaMask_Perlin::GetMaxs() const
{
	return IntVector2(m_xRange.max, m_yRange.max);
}

float AreaMask_Perlin::GetNoiseAtPoint(int x, int y) const
{
	float noiseVal = Compute2dPerlinNoise((float) x, (float) y, m_noiseScale, m_numOctaves, m_octavePersistence, m_octaveScale, true, m_seed);
	return noiseVal;
}
