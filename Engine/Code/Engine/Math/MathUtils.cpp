#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <stdlib.h>

float ConvertRadiansToDegrees(float radians)
{
	return (radians * (180.f/ static_cast<float>(PI)));
}

float ConvertDegreesToRadians(float degrees)
{
	return (degrees * (static_cast<float>(PI)/180.f));
}

float CosDegreesf(float degrees)
{
	return cosf(ConvertDegreesToRadians((degrees)));
}

float SinDegreesf(float degrees)
{
	return sinf(ConvertDegreesToRadians(degrees));
}

float ArcSinDegreesf(float val)
{
	return ConvertRadiansToDegrees(asinf(val));
}

float Atan2Degreesf(float y, float x)
{
	return ConvertRadiansToDegrees(atan2f(y,x));
}

float AtanDegreesf(float val)
{
	return ConvertRadiansToDegrees(atanf(val));
}

Vector2 PolarToCartesian(float radius, float theta)
{
	return Vector2(radius * CosDegreesf(theta), radius * SinDegreesf(theta));
}

Vector2 CartesianToPolar(float x, float y)
{
	Vector2 pos = Vector2(x,y);
	float radius = pos.GetLength();
	float theta = Atan2Degreesf(y,x);
	return Vector2(radius, theta);

}

Vector3 SphericalToCartesian(float radius, float theta, float azimuth)
{
	float x = radius * CosDegreesf(azimuth) * CosDegreesf(theta);
	float y = radius * SinDegreesf(azimuth);
	float z = -radius * CosDegreesf(azimuth) * SinDegreesf(theta);
	return Vector3(x,y,z);
}

Vector3 CartesianToSpherical(float x, float y, float z)
{
	UNIMPLEMENTED();
	UNUSED(x);
	UNUSED(y);
	UNUSED(z);
	return Vector3();
}




// ------- 
// REFERENCE:
// https://stackoverflow.com/questions/686353/c-random-float-number-generation
float GetRandomFloatInRange(float minInclusive, float maxInclusive)
{
	float r = static_cast<float>(rand());
	float range = maxInclusive - minInclusive;
	float percRand = r / RAND_MAX;
	float scaledTORange = percRand * range;
	float shiftToMin = scaledTORange + minInclusive;
	return shiftToMin;
	//return ((r/RAND_MAX)*range) + minInclusive;
	/*float r = static_cast<float>(rand());
	float range = maxInclusive - minInclusive;
	return ((r/RAND_MAX)*range) + minInclusive;*/

}

float GetRandomFloatZeroToOne()
{
	return (static_cast<float>(rand())/static_cast<float>(RAND_MAX));
}

int GetRandomIntInRange(int minInclusive, int maxInclusive)
{
	return (GetRandomIntLessThan((maxInclusive-minInclusive) + 1) + minInclusive);
}

int GetRandomIntLessThan(int maxNotInclusive)
{
	return rand() % maxNotInclusive;
}

bool CheckRandomChance(const float& chanceForSuccess)
{
	float check = GetRandomFloatZeroToOne();
	if (check <= chanceForSuccess){
		return true;
	} else {
		return false;
	}
}

float RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	//if inRange is zero, return average of outRange
	if (inStart == inEnd){
		return (outStart + outEnd)*.5f;
	}

	float inRange = inEnd - inStart;
	float outRange = outEnd - outStart;
	float inRelativeToStart = inValue - inStart;				//translate in to 0
	float fractionIntoRange = inRelativeToStart/inRange;		//scale to be [0,1]
	float outRelativeToStart = fractionIntoRange * outRange;	//scale to output range
	float outValue = outRelativeToStart + outStart;				//translate up to out range
	return outValue;


}

int RoundToNearestInt(float inValue)
{
	float floorOfIn = floorf(inValue);
	float diff = inValue - floorOfIn;
	if (diff >= .5){
		return (int) (ceilf(inValue));
	} else {
		return (int) floorOfIn;
	}
}

int ClampInt(int inValue, int min, int max)
{
	if (inValue > max){
		return max;
	} 
	if (inValue < min){
		return min;
	}
	return inValue;
}

float ClampFloat(float inValue, float min, float max)
{
	if (inValue > max){
		return max;
	} 
	if (inValue < min){
		return min;
	}
	return inValue;
}

float ClampFloatZeroToOne(float inValue)
{
	if (inValue > 1.f){
		return 1.f;
	} 
	if (inValue < 0.f){
		return 0.f;
	}
	return inValue;
}

float ClampFloatNegativeOneToOne(float inValue)
{
	if (inValue > 1.f){
		return 1.f;
	} 
	if (inValue < -1.f){
		return -1.f;
	}
	return inValue;
}

void ClampZeroToOne(float & value)
{
	if (value < 0.f){
		value = 0.f;
	} 
	if (value > 1.f){
		value = 1.f;
	}
}

float GetFractionInRange(float inValue, float rangeStart, float rangeEnd)
{

	if (rangeStart == rangeEnd){
		if (inValue < rangeStart){
			return 0.f;
		} else if (inValue > rangeEnd){
			return 1.f;
		} else {
			return .5f;
		}
	}

	float range = rangeEnd - rangeStart;
	float inRelativeToStart = inValue - rangeStart;				//translate in to 0
	float fractionIntoRange = inRelativeToStart/range;		//scale to be [0,1]
	return fractionIntoRange;
}

bool IsInRange(float value, float minInclusive, float maxExclusive)
{
	return ((value >= minInclusive) && (value < maxExclusive));
}

float Min(const float & x, const float & y)
{
	if (x > y){
		return y;
	} else {
		return x;
	}
}

int Min(const int & x, const int & y)
{
	if (x > y){
		return y;
	} else {
		return x;
	}
}

const float Interpolate(const float&  start, const float&  end, const float&  fractionTowardEnd)
{
	
	float range = end - start;
	float inRelativeToStart = range * fractionTowardEnd;
	return inRelativeToStart + start;
}

int Interpolate(int start, int end, float fractionTowardEnd)
{
	if (fractionTowardEnd == 1.f){
		return end;
	}
	int range = end - start + 1;
	int increase = (int) floor((float) range * fractionTowardEnd);
	return start + increase;
}

unsigned char Interpolate(unsigned char start, unsigned char end, float fractionTowardEnd)
{
	return (unsigned char) Interpolate((int) start, (int) end, fractionTowardEnd);
}

float GetAngularDisplacement(float startDegrees, float endDegrees)
{
	float angularDisp = endDegrees - startDegrees;
	while (angularDisp >= 180)
	{
		angularDisp-=360;
	}
	while (angularDisp <=-180)
	{
		angularDisp+=360;
	}
	return angularDisp;
}

float TurnToward(float currentDegrees, float goalDegrees, float maxTurnDegrees)
{
	float displacement = GetAngularDisplacement(currentDegrees, goalDegrees);
	if (maxTurnDegrees > fabs(displacement)){
		return goalDegrees;
	} else if (displacement > 0){
		return currentDegrees + maxTurnDegrees;
	} else{
		return currentDegrees - maxTurnDegrees;
	}
}

float TurnTowardWithDirection(float currentDegrees, float goalDegrees, float maxTurnDegreesAndDirection)
{
	//idk if this works
	float displacement = GetAngularDisplacement(currentDegrees, goalDegrees);
	if (maxTurnDegreesAndDirection > displacement){
		return goalDegrees;
	} else if (displacement > 0){
		return currentDegrees + maxTurnDegreesAndDirection;
	} else{
		return currentDegrees - maxTurnDegreesAndDirection;
	}
}

float GetRotationNegative180To180(float startDegrees)
{
	while (startDegrees >= 180)
	{
		startDegrees-=360;
	}
	while (startDegrees <=-180)
	{
		startDegrees+=360;
	}
	return startDegrees;
}

float DotProduct(const Vector2 & a, const Vector2 & b)
{
	return ((a.x * b.x) + (a.y * b.y));
}

float DotProduct(const Vector3 & a, const Vector3 & b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

bool IsPointInConicSector2D(const Vector2 & point, const Vector2 & centerPoint, const Vector2 & centerVector, const float sectorTheta, const float sectorRadius)
{
	Vector2 displacement = point - centerPoint;
	if (displacement.GetLength() < sectorRadius){
		float minDot = CosDegreesf(sectorTheta);
		float dot = DotProduct(displacement.GetNormalized(), centerVector.GetNormalized());
		if (dot >= minDot){
			return true;
		}
	}
	return false;
}



bool AreBitsSet(unsigned char bitFlags8, unsigned char flagsToCheck)
{
	return ((bitFlags8 & flagsToCheck) == flagsToCheck);
}

bool AreBitsSet(unsigned int bitFlags32, unsigned int flagsToCheck)
{
	return ((bitFlags32 & flagsToCheck) == flagsToCheck);
}

void SetBits(unsigned char & bitFlags8, unsigned char flagsToSet)
{
	bitFlags8 = (bitFlags8 | flagsToSet);
}

void SetBits(unsigned int & bitFlags32, unsigned int flagsToSet)
{
	bitFlags32 = (bitFlags32 | flagsToSet);
}

void ClearBits(unsigned char & bitFlags8, unsigned char flagToClear)
{
	bitFlags8 = (bitFlags8 & (~flagToClear));
}

void ClearBits(unsigned int & bitFlags32, unsigned int flagToClear)
{
	bitFlags32 = (bitFlags32 & (~flagToClear));
}

//returns index of a 1D array from 2D coordinates
int GetIndexFromCoordinates(int x, int y, int width, int height)
{
	if (y >= 0 && y < height && x >= 0 && x < width){
		return x + (y * width);
	}
	return -1;
}

int GetIndexFromCoordinates(IntVector2 coords, IntVector2 dimensions)
{
	return GetIndexFromCoordinates(coords.x, coords.y, dimensions.x, dimensions.y);
}

int GetIndexFromCoordinates(IntVector3 coords, IntVector3 dimensions)
{
	return GetIndexFromCoordinates(coords.x, coords.y, coords.z, dimensions.x, dimensions.y, dimensions.z);
}

int GetIndexFromCoordinates(int x, int y, int z, int width, int height, int depth)
{
	UNUSED(depth);
	return (x + (width * y) + (width * height * z));
}

//returns 2D coordinates from the index of a 1D list
IntVector2 GetCoordinatesFromIndex(int index, int width)
{
	int x = index % width;
	int y = (index - x) /width;
	return IntVector2(x,y);
}

IntVector3 GetCoordinatesFromIndex(int index, int width, int height)
{

	int z = (int) (index / (width *height));
	int y = (int) ((index - (z * width * height)) / width);
	int x = index - ((y * width ) + (z * width * height));
	return IntVector3(x,y,z);
}




float SmoothStart2(float t)
{
	return t * t;
}

float SmoothStart3(float t)
{
	return t * t * t;
}

float SmoothStart4(float t)
{
	return t * t * t * t;
}

float SmoothStop2(float t)
{
	float flipT = 1.f - t;
	return 1.f - (flipT * flipT);
}

float SmoothStop3(float t)
{
	float flipT = 1.f - t;
	return 1.f - (flipT * flipT * flipT);
}

float SmoothStop4(float t)
{
	float flipT = 1.f - t;
	return 1.f - (flipT * flipT * flipT * flipT);
}

float SmoothStep3(float t)
{
	return Crossfade(SmoothStart3(t), SmoothStop3(t), t);
}

float Crossfade(float a, float b, float t)
{
	return (a + (t * (b - a)));
}

bool Quadratic(Vector2 * out, float a, float b, float c)
{
	float sqroot =  sqrtf((b*b) - ( 4 * a * c));
	if (isnan(sqroot) || a == 0.f){
		return false;
	}
	float ratio = 1.f /  (2 * a);
	float x1 = (-b + sqroot) * ratio;
	float x2 = (-b - sqroot) * ratio;
	out->x = x1;
	out->y = x2;
	return true;
}

float Mix(float a, float b, float weightB)
{
	return (((1- weightB) * a) + (weightB *b));
}
