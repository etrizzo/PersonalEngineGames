#pragma once
#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Standalone curve utility functions
//
// Note: Quadratic Bezier functions (only) specify an actual midpoint, not a Bezier guide point
/////////////////////////////////////////////////////////////////////////////////////////////////

template< typename T >
T EvaluateQuadraticBezier( const T& startPos, const T& actualCurveMidPos, const T& endPos, float t );

template< typename T >
T EvaluateQuadraticBezier(const T& startPos, const T& actualCurveMidPos, const T& endPos, float t)
{
	T midPoint = startPos + ((endPos - startPos) * .5f);
	T disp = actualCurveMidPos - midPoint;
	T bezierPoint = actualCurveMidPos + disp;

	T e = Interpolate(startPos, bezierPoint, t);
	T f = Interpolate(bezierPoint, endPos, t);
	T p = Interpolate(e,f,t);

	return p;
}

template< typename T >
T EvaluateCubicBezier( const T& startPos, const T& guidePos1, const T& guidePos2, const T& endPos, float t );

template< typename T >
T EvaluateCubicBezier(const T& startPos, const T& guidePos1, const T& guidePos2, const T& endPos, float t)
{
	T e = Interpolate( startPos, guidePos1, t);
	T f = Interpolate( guidePos1, guidePos2, t);
	T g = Interpolate( guidePos2, endPos, t);
	T q = Interpolate( e, f, t);
	T r = Interpolate( f, g, t);
	T p = Interpolate( q, r, t);

	return p;

}

template< typename T >
T EvaluateCubicHermite( const T& startPos, const T& startVel, const T& endPos, const T& endVel, float t );


template< typename T >
T EvaluateCubicHermite(const T& startPos, const T& startVel, const T& endPos, const T& endVel, float t)
{
	float oneThird = 1.f/3.f;
	T B = startPos + (startVel * oneThird);
	T C = endPos - (endVel * oneThird);

	T e = Interpolate( startPos, B, t);
	T f = Interpolate( B, C, t);
	T g = Interpolate( C, endPos, t);
	T q = Interpolate( e, f, t);
	T r = Interpolate( f, g, t);
	T p = Interpolate( q, r, t);

	return p;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// CubicSpline2D
// 
// Cubic Hermite/Bezier spline of Vector2 positions / velocities
/////////////////////////////////////////////////////////////////////////////////////////////////
class CubicSpline2D
{
public:
	CubicSpline2D() {}
	explicit CubicSpline2D( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray=nullptr );
	~CubicSpline2D() {}

	// Mutators
	void		AppendPoint( const Vector2& position, const Vector2& velocity=Vector2::ZERO );
	void		AppendPoints( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray=nullptr );
	void		InsertPoint( int insertBeforeIndex, const Vector2& position, const Vector2& velocity=Vector2::ZERO );
	void		RemovePoint( int pointIndex );
	void		RemoveAllPoints();
	void		SetPoint( int pointIndex, const Vector2& newPosition, const Vector2& newVelocity );
	void		SetPosition( int pointIndex, const Vector2& newPosition );
	void		SetVelocity( int pointIndex, const Vector2& newVelocity );
	void		SetCardinalVelocities( float tension=0.f, const Vector2& startVelocity=Vector2::ZERO, const Vector2& endVelocity=Vector2::ZERO );

	// Accessors
	int				GetNumPoints() const { return (int) m_positions.size(); }
	const Vector2	GetPosition( int pointIndex ) const;
	const Vector2	GetVelocity( int pointIndex ) const;
	int				GetPositions( std::vector<Vector2>& out_positions )		const ;
	int				GetVelocities( std::vector<Vector2>& out_velocities )	const ;
	Vector2			EvaluateAtCumulativeParametric( float t ) const;
	Vector2			EvaluateAtNormalizedParametric( float t ) const;

protected:
	std::vector<Vector2>	m_positions;
	std::vector<Vector2>	m_velocities;
};



