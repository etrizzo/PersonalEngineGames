#pragma once
#include "Engine/Math/Vector3.hpp"


//-----------------------------------------------------------------------------------------------
class Vector2
{
public:
	// Construction/Destruction
	~Vector2() {}											// destructor: do nothing (for speed)
	Vector2() {}											// default constructor: do nothing (for speed)
	Vector2( const Vector2& copyFrom );						// copy constructor (from another vec2)
	explicit Vector2( float initialX, float initialY );		// explicit constructor (from x, y)

															// Operators
	const Vector2 operator+( const Vector2& vecToAdd ) const;		// vec2 + vec2
	const Vector2 operator-( const Vector2& vecToSubtract ) const;	// vec2 - vec2
	const Vector2 operator*( float uniformScale ) const;			// vec2 * float
	const Vector2 operator/( float inverseScale ) const;			// vec2 / float
	void operator+=( const Vector2& vecToAdd );						// vec2 += vec2
	void operator-=( const Vector2& vecToSubtract );				// vec2 -= vec2
	void operator*=( const float uniformScale );					// vec2 *= float
	void operator/=( const float uniformDivisor );					// vec2 /= float
	void operator=( const Vector2& copyFrom );						// vec2 = vec2
	bool operator==( const Vector2& compare ) const;				// vec2 == vec2
	bool operator!=( const Vector2& compare ) const;				// vec2 != vec2

	friend const Vector2 operator*( float uniformScale, const Vector2& vecToScale );	// float * vec2


	float GetLength() const;										//returns length of vector
	float GetLengthSquared() const;									//returns length of vector squared
	float NormalizeAndGetLength();							//normalizes vector and returns length (1.0)
	Vector2 GetNormalized() const;									//returns a new normalized copy of the vector
	float GetOrientationDegrees() const;							//return orientation degrees for the vector (with east being 0)
	static Vector2 MakeDirectionAtDegrees( float degrees);			//Creates a vector at angle specified
	Vector3 GetVector3() const;

	Vector2 RotateDegrees(float degrees);

	void SetFromText(const char* text);


	const static Vector2 ZERO;
	const static Vector2 ONE;
	const static Vector2 HALF;
	const static Vector2 TOP_LEFT;
	const static Vector2 BOTTOM_RIGHT;
	const static Vector2 ALIGN_CENTER_LEFT;
	const static Vector2 ALIGN_CENTER_RIGHT;
	const static Vector2 ALIGN_CENTER_TOP;
	const static Vector2 ALIGN_CENTER_BOTTOM;

	const static Vector2 TOP_LEFT_PADDED;
	const static Vector2 TOP_RIGHT_PADDED;
	const static Vector2 BOTTOM_LEFT_PADDED;
	const static Vector2 BOTTOM_RIGHT_PADDED;

	const static Vector2 NORTH;
	const static Vector2 SOUTH;
	const static Vector2 EAST;
	const static Vector2 WEST;

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
};


//---------------------
// DISTANCE FUNCTIONS
//---------------------

float GetDistance(const Vector2& a, const Vector2& b);
float GetDistanceSquared(const Vector2& a, const Vector2& b);


//----------------------
// PROJECTION FUNCTIONS
//----------------------

const Vector2 GetProjectedVector( const Vector2& vectorToProject, const Vector2& projectOnto);
const Vector2 GetTransformedIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ);
const Vector2 GetTransformedOutOfBasis( const Vector2& vectorInBasis, const Vector2& oldBasisI, const Vector2& oldBasisJ);
void DecomposeVectorIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ, Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ);
const Vector2 Reflect(const Vector2& vectorToBounce, const Vector2& normal);

const Vector2 Interpolate( const Vector2& start, const Vector2& end, float fractionTowardEnd );