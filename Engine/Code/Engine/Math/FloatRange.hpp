#pragma once
#include "MathUtils.hpp"

class FloatRange{
public:
	explicit FloatRange(float initialMin, float initialMax);
	explicit FloatRange(float initialMinMax);
	~FloatRange(){};

	float min;
	float max;

	float GetRandomInRange() const;
	void SetFromText(const char* text);
	bool IsValueInRangeInclusive(const float& value) const;
	float GetDistanceFromEdge(const float& value) const;
};


bool DoRangesOverlap(const FloatRange& a, const FloatRange& b);
const FloatRange Interpolate(const FloatRange& start, const FloatRange& end, float fractionTowardEnd);