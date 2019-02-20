#pragma once

class Vector2;


//-----------------------------------------------------------------------------------------------
class Vector3
{
public:
	// Construction/Destruction
	~Vector3() {}											// destructor: do nothing (for speed)
	Vector3() {}											// default constructor: do nothing (for speed)
	Vector3( const Vector3& copyFrom );						// copy constructor (from another vec3)
	Vector3( float initialX, float initialY, float initialZ = 0.f);
	Vector3( const Vector2& vec2, float initialz = 0.f);
	
	// Operators
	const Vector3 operator-() const;								// -vec3
	const Vector3 operator+( const Vector3& vecToAdd ) const;		// vec2 + vec2
	const Vector3 operator-( const Vector3& vecToSubtract ) const;	// vec2 - vec2
	const Vector3 operator*( float uniformScale ) const;			// vec2 * float
	const Vector3 operator/( float inverseScale ) const;			// vec2 / float
	void operator+=( const Vector3& vecToAdd );						// vec2 += vec2
	void operator-=( const Vector3& vecToSubtract );				// vec2 -= vec2
	void operator*=( const float uniformScale );					// vec2 *= float
	void operator/=( const float uniformDivisor );					// vec2 /= float
	void operator=( const Vector3& copyFrom );						// vec2 = vec2
	bool operator==( const Vector3& compare ) const;				// vec2 == vec2
	bool operator!=( const Vector3& compare ) const;				// vec2 != vec2

	friend const Vector3 operator*( float uniformScale, const Vector3& vecToScale );	// float * vec2


	float GetLength() const;										//returns length of vector
	float GetLengthSquared() const;									//returns length of vector squared
	float NormalizeAndGetLength();							//normalizes vector and returns length (1.0)
	Vector3 GetNormalized() const;									//returns a new normalized copy of the vector
	float GetOrientationDegrees() const;							//return orientation degrees for the vector (with east being 0)
	static Vector3 MakeDirectionAtDegrees( float degrees);			//Creates a vector at angle specified

	Vector2 XY() const;
	Vector2 XZ() const;

	void SetFromText(const char* text);


public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
	float z;

	const static Vector3 ZERO;
	const static Vector3 ONE;
	const static Vector3 HALF;
	
	const static Vector3 X_AXIS;
	const static Vector3 Y_AXIS;
	const static Vector3 Z_AXIS;

	//const static Vector3 GRAVITY;
};


//---------------------
// DISTANCE FUNCTIONS
//---------------------

float GetDistance(const Vector3& a, const Vector3& b);
float GetDistanceSquared(const Vector3& a, const Vector3& b);

Vector3 Average(const Vector3& a, const Vector3& b);


//----------------------
// PROJECTION FUNCTIONS
//----------------------

const Vector3 GetProjectedVector( const Vector3& vectorToProject, const Vector3& projectOnto);
const Vector3 GetTransformedIntoBasis( const Vector3& originalVector, const Vector3& newBasisI, const Vector3& oldBasisJ);
const Vector3 GetTransformedOutOfBasis( const Vector3& vectorInBasis, const Vector3& oldBasisI, const Vector3& oldBasisJ);
void DecomposeVectorIntoBasis( const Vector3& originalVector, const Vector3& newBasisI, const Vector3& newBasisJ, Vector3& out_vectorAlongI, Vector3& out_vectorAlongJ);
const Vector3 Reflect(const Vector3& vectorToBounce, const Vector3& normal);

const Vector3 Interpolate( const Vector3& start, const Vector3& end, float fractionTowardEnd );

const Vector3 Cross( const Vector3& A, const Vector3& b);



const Vector3 GetRandomNormalizedDirection();
const Vector3 GetRandomPointOnSphere();