#pragma once

#include<math.h>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/Matrix44.hpp"



#define PI 3.14159265359
#define EPSILON .00000001

//-------------------
// TRIGONOMETRY FUNCTIONS
//-------------------

float ConvertRadiansToDegrees(float radians);

float ConvertDegreesToRadians(float degrees);

float CosDegreesf(float degrees);

float SinDegreesf(float degrees);

float ArcSinDegreesf(float val);

float Atan2Degreesf(float y, float x);

float AtanDegreesf(float val);

Vector2 PolarToCartesian(float radius, float theta);

Vector2 CartesianToPolar(float x, float y);

Vector3 SphericalToCartesian(float radius, float theta, float azimuth);

Vector3 CartesianToSpherical(float x, float y, float z);



//-------------------
// RANDOM FUNCTIONS
//-------------------

float GetRandomFloatInRange(float minInclusive, float maxInclusive);

float GetRandomFloatZeroToOne();

int GetRandomIntInRange(int minInclusive, int maxInclusive);

int GetRandomIntLessThan(int maxNotInclusive);

bool CheckRandomChance(const float& chanceForSuccess);


//-----------------
// RANGE FUNCTIONS
//-----------------

//For a value in [inStart, inEnd], finds the corresponding value in [outStart, outEnd]
float RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd);

int RoundToNearestInt(float inValue);

int ClampInt(int inValue, int min, int max);

float ClampFloat(float inValue, float min, float max);

float ClampFloatZeroToOne(float inValue);

float ClampFloatNegativeOneToOne(float inValue);

void ClampZeroToOne(float& value);

Vector2 ClampVector2(Vector2 inVal, Vector2 mins, Vector2 maxs);

//Finds the % (as a fraction) of inValue in [rangeStart, rangeEnd]
float GetFractionInRange(float inValue, float rangeStart, float rangeEnd);

bool IsInRange(float value, float minInclusive, float maxExclusive);

float Min(const float& x, const float& y);
int Min(const int& x, const int& y);
size_t Min(const size_t& x, const size_t& y);
int Max(const int& x, const int& y);
float Max(const float& x, const float& y);



template <typename T>
bool CycleLess(T a, T b)
{
	T MAX_VAL = (T) (~(T) 0);
	T MAX_SIGNED = MAX_VAL >> 1;		//make everything 1 and then shift it by 1 (this is half max if its unsigned)
	T diff = b - a;
	return ((diff > 0) && (diff <= MAX_SIGNED));		//to get LessThanEqual, just remove diff > 0
};

//---------------
// INTERPOLATION
//---------------


//Finds the value at a certain % (as a fraction) in [rangeStart, rangeEnd]
const float Interpolate(const float& start, const float&  end, const float&  fractionTowardEnd);
int Interpolate(int start, int end, float fractionTowardEnd);
unsigned char Interpolate( unsigned char start, unsigned char end, float fractionTowardEnd );

Vector3 Slerp(const Vector3& a, const Vector3& b, float t);
Vector3 SlerpUnit(const Vector3& a, const Vector3& b, float t);


//------------------------
// ANGLE AND VECTOR UTILS
//------------------------

float GetAngularDisplacement(float startDegrees, float endDegrees);

float TurnToward(float currentDegrees, float goalDegrees, float maxTurnDegrees);


float TurnTowardWithDirection(float currentDegrees, float goalDegrees, float maxTurnDegreesAndDirection);

float GetRotationNegative180To180(float startDegrees);

float DotProduct( const Vector2& a, const Vector2& b );

float DotProduct( const Vector3& a, const Vector3& b );

bool IsPointInConicSector2D(const Vector2& point, const Vector2& centerPoint, const Vector2& centerVector, const float sectorTheta, const float sectorRadius);


//---------------
// BITFLAG UTILS
//---------------

bool AreBitsSet(unsigned char bitFlags8, unsigned char flagsToCheck);

bool AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck );

void SetBits( unsigned char& bitFlags8, unsigned char flagsToSet );

void SetBits( unsigned int& bitFlags32, unsigned int flagsToSet );

void ClearBits( unsigned char& bitFlags8, unsigned char flagToClear );

void ClearBits( unsigned int& bitFlags32, unsigned int flagToClear );




//--------------------
// INDEXING FUNCTIONS
//--------------------

int GetIndexFromCoordinates(int x, int y, int width, int height);
int GetIndexFromCoordinates(IntVector2 coords, IntVector2 dimensions);
int GetIndexFromCoordinates(IntVector3 coords, IntVector3 dimensions);
int GetIndexFromCoordinates(int x, int y ,int z, int width, int height, int depth);

IntVector2 GetCoordinatesFromIndex(int index, int width);
IntVector3 GetCoordinatesFromIndex(int index, int width, int height);




//------------------
// EASING FUNCTIONS
//------------------

float	SmoothStart2( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease in”)
float	SmoothStart3( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease in”)
float	SmoothStart4( float t ); // 4th-degree smooth start (a.k.a. “quartic ease in”)
float	SmoothStop2( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease out”)
float	SmoothStop3( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease out”)
float	SmoothStop4( float t ); // 4th-degree smooth start (a.k.a. “quartic ease out”)
float	SmoothStep3( float t ); // 3rd-degree smooth start/stop (a.k.a. “smoothstep”)

float Mix(float a, float b, float weightB);
float Crossfade(float a, float b, float t);



//-----------
// QUADRATIC
//-----------

bool Quadratic( Vector2 *out, float a, float b, float c );











