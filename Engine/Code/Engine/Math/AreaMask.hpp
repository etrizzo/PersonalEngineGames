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
	AreaMask(){};
	AreaMask(float centerDensity, float edgeDensity, float feather);

	float m_centerDensity = 1.f;
	float m_edgeDensity = 1.f;
	float m_featherRate = 1.f;		//chance decrease per tile
	unsigned int m_seed = 0;

	virtual IntVector2 GetRandomPointInArea() const = 0;
	virtual bool IsPointInside(IntVector2 xyCoords) const;
	virtual bool IsPointInside(int x, int y) const = 0;
	virtual float GetDistanceFromEdge(int x, int y) const = 0;
	virtual float GetDistanceFromCenter(int x, int y) const = 0;
	virtual float GetMaxDistanceFromCenter() const = 0;
	virtual bool IsOnEdge(int x, int y) const;
	virtual AreaMask* GetSubArea(FloatRange centerRange, FloatRange sizeRange) const;	//not applicable for perlin??
	virtual int GetWidth() const;
	virtual int GetHeight() const;
	virtual IntVector2 GetMins() const;
	virtual IntVector2 GetMaxs() const;

	void SetDensity(float centerDensity = 1.f, float edgeDensity = 1.f, float featherRate = 1.f);


	virtual bool CanDrawOnPoint(int x, int y) const;
	virtual bool CanDrawOnPoint(IntVector2 xyCoords) const;

	virtual float GetChanceAtPoint(int x, int y, float existingChance = 1.f) const;

	FloatRange m_acceptableRange = FloatRange(0.f);
};

class AreaMask_Rectangle : public AreaMask{
public:
	AreaMask_Rectangle() {};
	AreaMask_Rectangle(IntVector2 center, IntVector2 halfDimensions, float centerDensity = 1.f, float edgeDensity = 1.f, float featherRate = 1.f);

	IntVector2 m_center = IntVector2(0,0);
	IntVector2 m_halfDims = IntVector2(0,0);

	IntRange m_xRange = IntRange(0);
	IntRange m_yRange = IntRange(0);	//generated on construction

	IntVector2 GetRandomPointInArea() const			override;
	//bool IsPointInside(IntVector2 xyCoords) const	override;
	bool IsPointInside(int x, int y) const			override;
	float GetDistanceFromEdge(int x, int y) const	override;
	float GetDistanceFromCenter(int x, int y) const override;
	float GetMaxDistanceFromCenter() const override;

	IntVector2 GetClosestPointOnEdge(int x, int y) const;

	IntVector2 GetMins() const override	;
	IntVector2 GetMaxs() const override	;

	AreaMask* GetSubArea(FloatRange centerRange, FloatRange sizeRange) const override;
};

class AreaMask_Circle : public AreaMask{
public:
	AreaMask_Circle(IntVector2 center, int radius, float centerDensity = 1.f, float edgeDensity = 1.f, float featherRate = 1.f);

	IntVector2 m_center = IntVector2(0,0);
	int m_radius;

	IntVector2 GetRandomPointInArea() const			override;
	//bool IsPointInside(IntVector2 xyCoords) const	override;
	bool IsPointInside(int x, int y) const			override;
	float GetDistanceFromEdge(int x, int y) const	override;
	float GetDistanceFromCenter(int x, int y) const override;
	float GetMaxDistanceFromCenter() const override;

	IntVector2 GetMins() const override	;
	IntVector2 GetMaxs() const override	;

	AreaMask* GetSubArea(FloatRange centerRange, FloatRange sizeRange) const override;

};

class AreaMask_Perlin : public AreaMask{
public:
	AreaMask_Perlin(FloatRange acceptableRange, IntVector2 mapMins, IntVector2 mapMaxs, unsigned int seed = 1, float scale = 20.f, unsigned int numOctaves = 1, float octavePersistence = .5f, float octaveScale = 2.f);

	
	IntRange m_xRange;
	IntRange m_yRange;
	float m_noiseScale;
	unsigned int m_numOctaves;
	float m_octavePersistence;
	float m_octaveScale;


	IntVector2 GetRandomPointInArea() const			override;
	//bool IsPointInside(IntVector2 xyCoords) const	override;
	bool IsPointInside(int x, int y) const			override;
	float GetDistanceFromEdge(int x, int y) const	override;
	float GetDistanceFromCenter(int x, int y) const override;
	float GetMaxDistanceFromCenter() const			override;
	bool IsOnEdge(int x, int y) const				override;


	IntVector2 GetMins() const override	;
	IntVector2 GetMaxs() const override	;

	float GetNoiseAtPoint(int x, int y) const;
};