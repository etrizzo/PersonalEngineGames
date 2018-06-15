#pragma once
#include "Engine/Math/Vector2.hpp"


class IntVector2{
public:
	


	~IntVector2() {}											// destructor: do nothing (for speed)
	IntVector2() {}											// default constructor: do nothing (for speed)
	IntVector2( const IntVector2& copyFrom );						// copy constructor (from another vec2)
	explicit IntVector2( int initialX, int initialY );		// explicit constructor (from x, y)

															// Operators
	const IntVector2 operator+( const IntVector2& vecToAdd ) const;		// vec2 + vec2
	const IntVector2 operator-( const IntVector2& vecToSubtract ) const;	// vec2 - vec2
	const IntVector2 operator*( int uniformScale ) const;			// vec2 * int
	const IntVector2 operator/( int inverseScale ) const;			// vec2 / int
	void operator+=( const IntVector2& vecToAdd );						// vec2 += vec2
	void operator-=( const IntVector2& vecToSubtract );				// vec2 -= vec2
	void operator*=( const int uniformScale );					// vec2 *= int
	void operator/=( const int uniformDivisor );					// vec2 /= int
	void operator=( const IntVector2& copyFrom );						// vec2 = vec2
	bool operator==( const IntVector2& compare ) const;				// vec2 == vec2
	bool operator!=( const IntVector2& compare ) const;				// vec2 != vec2

	friend const IntVector2 operator*( int uniformScale, const IntVector2& vecToScale );	// float * vec2


	Vector2 GetVector2() const;
	void SetFromText(const char* text);

	int x;
	int y;

	const static IntVector2 NORTH;
	const static IntVector2 SOUTH;
	const static IntVector2 EAST;
	const static IntVector2 WEST;
	
	const static IntVector2 INVALID_INDEX;

	const static IntVector2 AWAY_LEFT;
	const static IntVector2 AWAY_RIGHT;
	const static IntVector2 TOWARD_LEFT;
	const static IntVector2 TOWARD_RIGHT;

};

const IntVector2 Interpolate( const IntVector2& start, const IntVector2& end, float fractionTowardEnd );