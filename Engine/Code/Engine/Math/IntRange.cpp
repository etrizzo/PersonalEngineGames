#include "IntRange.hpp"
#include "Engine/Core/StringUtils.hpp"

IntRange::IntRange(int initialMin, int initialMax)
{
	min = initialMin;
	max = initialMax;
}

IntRange::IntRange(int initialMinMax)
{
	min = initialMinMax;
	max = initialMinMax;
}

int IntRange::GetRandomInRange() const
{
	return GetRandomIntInRange(min, max);
}

void IntRange::SetFromText(const char * text)
{
	std::string stringText = (std::string) text;
	unsigned int tildeIndex = (unsigned int) stringText.find('~');
	if (tildeIndex != std::string::npos){
		std::string minString = stringText.substr(0, tildeIndex);
		std::string maxString = stringText.substr(tildeIndex+1);
		min = atoi(minString.c_str());
		max = atoi(maxString.c_str());
	} else {
		min = atoi(stringText.c_str());
		max = min;
	}
}

bool IntRange::IsIntInRange(int isInRange) const
{
	if (isInRange >= min && isInRange <= max){
		return true;
	}
	return false;
}

std::string IntRange::ToString() const
{
	return Stringf("%i-%i", min, max);
}

bool DoRangesOverlap(const IntRange & a, const IntRange & b)
{
	if (a.max < b.min && a.min < b.min){
		return true;
	}
	if (b.max < a.min && b.min < a.min){
		return true;
	}
	return false;
}

const IntRange Interpolate(const IntRange & start, const IntRange & end, float fractionTowardEnd)
{
	return IntRange(Interpolate(start.min, end.min, fractionTowardEnd),  Interpolate(start.max, end.max, fractionTowardEnd));
}
