#include "CubicSpline.hpp"

CubicSpline2D::CubicSpline2D(const Vector2 * positionsArray, int numPoints, const Vector2 * velocitiesArray)
{
	m_positions = std::vector<Vector2>();
	m_velocities = std::vector<Vector2>();

	for (int i = 0; i < numPoints; i++){
		m_positions.push_back(positionsArray[i]);
		if (velocitiesArray != nullptr){
			m_velocities.push_back(velocitiesArray[i]);
		} else {
			m_velocities.push_back(Vector2::ZERO);
		}
	}

}

void CubicSpline2D::AppendPoint(const Vector2 & position, const Vector2 & velocity)
{
	m_positions.push_back(position);
	m_velocities.push_back(velocity);
}

void CubicSpline2D::AppendPoints(const Vector2 * positionsArray, int numPoints, const Vector2 * velocitiesArray)
{
	for (int i = 0; i < numPoints; i++){
		m_positions.push_back(positionsArray[i]);
		if (velocitiesArray != nullptr){
			m_velocities.push_back(velocitiesArray[i]);
		} else {
			m_velocities.push_back(Vector2::ZERO);
		}
	}
}

void CubicSpline2D::InsertPoint(int insertBeforeIndex, const Vector2 & position, const Vector2 & velocity)
{
	m_positions.insert( m_positions.begin() + insertBeforeIndex, position);
	m_velocities.insert(m_velocities.begin() + insertBeforeIndex, velocity);
}

void CubicSpline2D::RemovePoint(int pointIndex)
{
	std::vector<Vector2>::iterator start = m_positions.begin();
	m_positions.erase( m_positions.begin() + pointIndex);
	m_velocities.erase( m_velocities.begin() + pointIndex);
}

void CubicSpline2D::RemoveAllPoints()
{
	m_positions.clear();
	m_velocities.clear();
}

void CubicSpline2D::SetPoint(int pointIndex, const Vector2 & newPosition, const Vector2 & newVelocity)
{
	m_positions[pointIndex] = newPosition;
	m_velocities[pointIndex] = newVelocity;
}

void CubicSpline2D::SetPosition(int pointIndex, const Vector2 & newPosition)
{
	m_positions[pointIndex] = newPosition;
}

void CubicSpline2D::SetVelocity(int pointIndex, const Vector2 & newVelocity)
{
	m_velocities[pointIndex] = newVelocity;
}

void CubicSpline2D::SetCardinalVelocities(float tension, const Vector2 & startVelocity, const Vector2 & endVelocity)
{
	m_velocities[0] = startVelocity;
	m_velocities[m_positions.size()-1] = endVelocity;
	float tensionFactor = 1.f - tension;
	for (unsigned int i = 1; i < m_positions.size() -1; i++){
		Vector2 displacement = m_positions[i+1] - m_positions[i-1];
		m_velocities[i] = displacement * .5f * tensionFactor;
	}
}

const Vector2 CubicSpline2D::GetPosition(int pointIndex) const
{
	return m_positions[pointIndex];
}

const Vector2 CubicSpline2D::GetVelocity(int pointIndex) const
{
	return m_velocities[pointIndex];
}

int CubicSpline2D::GetPositions(std::vector<Vector2>& out_positions) const
{
	int numPositions = (int) m_positions.size();
	out_positions = std::vector<Vector2>(m_positions);
	return numPositions;
}

int CubicSpline2D::GetVelocities(std::vector<Vector2>& out_velocities) const
{
	int numPositions = (int) m_velocities.size();
	out_velocities = std::vector<Vector2>(m_velocities);
	return numPositions;
}

Vector2 CubicSpline2D::EvaluateAtCumulativeParametric(float t) const
{
	int startIndex = (int) floorf(t);
	Vector2 startPos = m_positions[startIndex];
	Vector2 startVel = m_velocities[startIndex];
	Vector2 endPos = m_positions[startIndex+1];
	Vector2 endVel = m_velocities[startIndex+1];

	return EvaluateCubicHermite(startPos, startVel, endPos, endVel, t - (float) startIndex);
}

Vector2 CubicSpline2D::EvaluateAtNormalizedParametric(float t) const
{
	int numCurves = (int) m_positions.size() -1;
	float cumulativeT = RangeMapFloat(t, 0.f, 1.f, 0.f, (float) numCurves);
	return EvaluateAtCumulativeParametric(cumulativeT);
}
