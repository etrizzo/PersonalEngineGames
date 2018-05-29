#include "AABB3.hpp"
#include "Engine/Core/StringUtils.hpp"

const AABB3 AABB3::NEGONE_TO_ONE = AABB3(-1.f,-1.f,-1.f, 1.f,1.f,1.f);

AABB3::AABB3(float nearminX, float nearminY, float nearminz, float farmaxX, float farmaxY, float farmaxZ)
{
	mins = Vector3(nearminX, nearminY, nearminz);
	maxs = Vector3(farmaxX, farmaxY, farmaxZ);
}

AABB3::AABB3(const Vector3 & nearMins, const Vector3 & farMaxs)
{
	mins = Vector3(nearMins);
	maxs = Vector3(farMaxs);
}

AABB3::AABB3(const Vector3 & center, float radiusX, float radiusY, float radiusZ)
{
	mins = Vector3(center.x - radiusX, center.y - radiusY, center.z - radiusZ);
	maxs = Vector3(center.x + radiusX, center.y + radiusY, center.z + radiusZ);
}

void AABB3::StretchToIncludePoint(float x, float y, float z)
{
	if (x < mins.x){
		mins.x = x;
	}
	if (y < mins.y){
		mins.y = y;
	}
	if (z < mins.z){
		mins.z = z;
	}
	if (x > maxs.x){
		maxs.x = x;
	}
	if (y > maxs.y){
		maxs.y = y;
	}
	if (z > maxs.z){
		maxs.z = z;
	}

}

void AABB3::StretchToIncludePoint(const Vector3& point)
{
	StretchToIncludePoint(point.x, point.y, point.z);
}

void AABB3::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius)
{
	mins.x-=xPaddingRadius;
	maxs.x+=xPaddingRadius;
	mins.y-=yPaddingRadius;
	maxs.y+=yPaddingRadius;
	mins.z-=zPaddingRadius;
	maxs.z+=zPaddingRadius;
}

void AABB3::Translate(const Vector3 & translation)
{
	mins+=translation;
	maxs+=translation;
}

void AABB3::Translate(float translationX, float translationY, float translationZ)
{
	Translate(Vector3(translationX, translationY, translationZ));
}

void AABB3::SetFromText(const char * text)
{
	Strings floats = Strings();
	Split((std::string) text, ',', floats);
	std::string minsText = floats[0] + "," + floats[1] + "," + floats[2];
	std::string maxsText = floats[3] + "," + floats[4] + "," + floats[5];
	mins.SetFromText(minsText.c_str());
	maxs.SetFromText(maxsText.c_str());
}

//void AABB3::SplitAABB3Vertical(float xDivide, AABB3 &outLeft, AABB3 &outRight)
//{
//	/*float boxWidth = GetWidth();
//	Vector2 splitLineBottom = mins + Vector2(boxWidth * xDivide, 0.f);
//	Vector2 splitLineTop = mins + Vector2(boxWidth * xDivide, GetHeight());
//	outLeft = AABB3(mins, splitLineTop);
//	outRight = AABB3(splitLineBottom, maxs);*/
//}
//
//void AABB3::SplitAABB3Horizontal(float yDivide, AABB3 &outTop, AABB3 &outBottom)
//{
//	float boxHeight = GetHeight();
//	Vector2 splitLineLeft = mins + Vector2(0.f, boxHeight * yDivide);
//	Vector2 splitLineRight = mins + Vector2(GetWidth(), boxHeight * yDivide);
//
//	outBottom = AABB3(mins, splitLineRight);
//	outTop = AABB3(splitLineLeft, maxs);
//}

bool AABB3::IsPointInside(float x, float y) const
{
	if (x <= maxs.x && x >= mins.x){
		if (y <= maxs.y && y>= mins.y){
			return true;
		}
	}
	return false;
}

bool AABB3::IsPointInside(const Vector2 & point) const
{
	if (point.x <= maxs.x && point.x >= mins.x){
		if (point.y <= maxs.y && point.y>= mins.y){
			return true;
		}
	}
	return false;
}

Vector3 AABB3::GetDimensions() const
{
	return Vector3(maxs.x - mins.x, maxs.y-mins.y, maxs.z - mins.z);
}

Vector3 AABB3::GetCenter() const
{
	return Vector3((maxs.x+mins.x) *.5f, (maxs.y+mins.y) * .5f, (maxs.z+mins.z) * .5f);
}

float AABB3::GetWidth() const
{
	return maxs.x - mins.x;
}

float AABB3::GetHeight() const
{
	return maxs.y - mins.y;
}

Vector2 AABB3::GetRandomPointInBox() const
{
	float x = GetRandomFloatInRange(mins.x, maxs.x);
	float y = GetRandomFloatInRange(mins.y, maxs.y);
	return Vector2(x,y);
}

void AABB3::operator+=(const Vector3 & translation)
{
	maxs+=translation;
	mins+=translation;
}

void AABB3::operator-=(const Vector3 & antiTranslation)
{
	maxs-=antiTranslation;
	mins-=antiTranslation;
}

AABB3 AABB3::operator+(const Vector3 & translation) const
{
	return AABB3(mins+translation, maxs+translation );
}

AABB3 AABB3::operator-(const Vector3 & antiTranslation) const
{
	return AABB3(mins - antiTranslation, maxs-antiTranslation);
}

bool DoAABBsOverlap(const AABB3 & a, const AABB3 & b)
{
	if (GetDistance(a.mins, b.mins) < GetDistance (a.mins, a.maxs)){
		return true;
	}
	if (GetDistance(b.mins, a.mins) < GetDistance (b.mins, b.maxs)){
		return true;
	}
	return false;
}

AABB3 Interpolate(const AABB3 & start, const AABB3 & end, float fractionTowardEnd)
{
	Vector3 mins = Interpolate(start.mins, end.mins, fractionTowardEnd);
	Vector3 maxs = Interpolate(start.maxs, end.maxs, fractionTowardEnd);
	return AABB3(mins, maxs);
}
