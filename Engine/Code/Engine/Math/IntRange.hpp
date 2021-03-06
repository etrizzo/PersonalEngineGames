#pragma once
#include "MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

class IntRange{
public:
	explicit IntRange(int initialMin, int initialMax);
	explicit IntRange(int initialMinMax);
	~IntRange(){};


	int min;
	int max;

	int GetSize() const;

	int GetRandomInRange() const;
	void SetFromText(const char* text);
	bool IsIntInRange(int isInRange) const;

	std::string ToString() const;
};

bool DoRangesOverlap( const IntRange& a, const IntRange& b);
const IntRange Interpolate( const IntRange& start, const IntRange& end, float fractionTowardEnd );