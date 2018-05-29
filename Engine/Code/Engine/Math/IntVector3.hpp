#pragma once
#include "Engine/Math/Vector3.hpp"

//-----------------------------------------------------------------------------------------------
class IntVector3
{
public:
	// Construction/Destruction
	~IntVector3() {}										
	IntVector3() {}											
	IntVector3( const IntVector3& copyFrom );				
	explicit IntVector3( int initialX, int initialY, int initialZ = 0);
	// Operators
	const IntVector3 operator+( const IntVector3& vecToAdd ) const;			
	const IntVector3 operator-( const IntVector3& vecToSubtract ) const;	
	const IntVector3 operator*( int uniformScale ) const;					
	void operator+=( const IntVector3& vecToAdd );							
	void operator-=( const IntVector3& vecToSubtract );						
	void operator*=( const int uniformScale );															
	void operator=( const IntVector3& copyFrom );							
	bool operator==( const IntVector3& compare ) const;						
	bool operator!=( const IntVector3& compare ) const;						

	friend const IntVector3 operator*( int uniformScale, const IntVector3& vecToScale );	


	float GetLength() const;										//returns length of vector
	float GetLengthSquared() const;									//returns length of vector squared
	Vector3 GetNormalized() const;									//returns a new normalized copy of the vector
	float GetOrientationDegrees() const;							//return orientation degrees for the vector (with east being 0)

	void SetFromText(const char* text);
	Vector3 GetVector3() const;


public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x;
	int y;
	int z;

	const static IntVector3 ZERO;
	const static IntVector3 ONE;
	const static IntVector3 UP;
};


//---------------------
// DISTANCE FUNCTIONS
//---------------------

float GetDistance(const IntVector3& a, const IntVector3& b);
float GetDistanceSquared(const IntVector3& a, const IntVector3& b);


const IntVector3 Interpolate( const IntVector3& start, const IntVector3& end, float fractionTowardEnd );
