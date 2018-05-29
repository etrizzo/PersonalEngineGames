#include "FloatRange.hpp"
#include <string>

FloatRange::FloatRange(float initialMin, float initialMax)
{
	min = initialMin;
	max = initialMax;
}

FloatRange::FloatRange(float initialMinMax)
{
	min = initialMinMax;
	max = initialMinMax;
}

float FloatRange::GetRandomInRange() const
{
	return GetRandomFloatInRange(min, max);
}

void FloatRange::SetFromText(const char * text)
{
	std::string stringText = (std::string) text;
	unsigned int tildeIndex = (unsigned int) stringText.find('~');
	if (tildeIndex != std::string::npos){
		std::string minString = stringText.substr(0, tildeIndex);
		std::string maxString = stringText.substr(tildeIndex+1);
		min = (float) atof(minString.c_str());
		max = (float) atof(maxString.c_str());
	} else {
		min = (float) atof(stringText.c_str());
		max = min;
	}
}

bool FloatRange::IsValueInRangeInclusive(const float& value) const
{
	if (value >= min && value <= max){
		return true;
	} else {
		return false;
	}
}

bool DoRangesOverlap(const FloatRange & a, const FloatRange & b)
{
	if (a.max < b.min && a.min < b.min){
		return true;
	}
	if (b.max < a.min && b.min < a.min){
		return true;
	}
	return false;
}

const FloatRange Interpolate(const FloatRange & start, const FloatRange & end, float fractionTowardEnd)
{
	float min = Interpolate(start.min, end.min, fractionTowardEnd);
	float max = Interpolate(start.max, end.max, fractionTowardEnd);
	return FloatRange(min,max);
}
