#include "Heatmap.hpp"


Heatmap::Heatmap(IntVector2 dimensions, float initialValue)
{
	m_dimensions = dimensions;
	m_heatValues = std::vector<float>(m_dimensions.x * m_dimensions.y, initialValue);
}

void Heatmap::ClearHeat(const float & newVal)
{
	m_heatValues = std::vector<float>(m_dimensions.x * m_dimensions.y, newVal);
}

void Heatmap::SetHeat(const IntVector2 & cellCoords, const float& heatVal)
{
	int heatIndex = GetIndexFromCoordinates(cellCoords, m_dimensions);
	m_heatValues[heatIndex] = heatVal;
}

void Heatmap::SetHeat(const int & index, const float & heatVal)
{
	m_heatValues[index] = heatVal;
}

void Heatmap::AddHeat(const IntVector2 & cellCoords, const float& heatVal)
{
	int heatIndex = GetIndexFromCoordinates(cellCoords, m_dimensions);
	m_heatValues[heatIndex]+= heatVal;
}

void Heatmap::AddHeat(const int & index, const float & heatVal)
{
	m_heatValues[index]+= heatVal;
}

float Heatmap::GetHeat(const int & x, const int & y) const
{
	return GetHeat(IntVector2(x,y));
}

float Heatmap::GetHeat(const IntVector2 & cellCoords) const
{
	int heatIndex = GetIndexFromCoordinates(cellCoords, m_dimensions);
	return GetHeat(heatIndex);
}

float Heatmap::GetHeat(const int & index) const
{
	return m_heatValues[index];
}

int Heatmap::GetIndexOfMinNeighbor(const int & startIndex) const
{
	int neighbors[4] = {-1,-1,-1,-1};
	unsigned int checkIndex;
	IntVector2 startCoords = GetCoordinatesFromIndex(startIndex, m_dimensions.x);
	checkIndex = GetIndexFromCoordinates(startCoords + STEP_NORTH, m_dimensions);
	if (checkIndex >= 0 && checkIndex < m_heatValues.size()){
		neighbors[MANHATTAN_NORTH] = checkIndex;
	}
	checkIndex =  GetIndexFromCoordinates(startCoords + STEP_WEST, m_dimensions);
	if (checkIndex >= 0 && checkIndex < m_heatValues.size()){
		neighbors[MANHATTAN_WEST] = checkIndex;
	}

	checkIndex = GetIndexFromCoordinates(startCoords + STEP_EAST, m_dimensions);
	if (checkIndex >= 0 && checkIndex < m_heatValues.size()){
		neighbors[MANHATTAN_EAST] = checkIndex;
	}
	checkIndex = GetIndexFromCoordinates(startCoords + STEP_SOUTH, m_dimensions);
	if (checkIndex >= 0 && checkIndex < m_heatValues.size()){
		neighbors[MANHATTAN_SOUTH] = checkIndex;
	}

	float minHeatVal = 9999.f;
	int minIndex = 0;
	for (int dir = 0; dir < 4; dir++){
		int index = neighbors[dir];
		if (index >=0 && m_heatValues[index] < minHeatVal){
			minHeatVal = m_heatValues[index];
			minIndex = index;
		}
	}
	return minIndex;
}

IntVector2 Heatmap::GetCoordsOfMinNeighbor(const IntVector2 & startCoords) const
{
	int startIndex = GetIndexFromCoordinates(startCoords, m_dimensions);
	int minIndex = GetIndexOfMinNeighbor(startIndex);
	return GetCoordinatesFromIndex(minIndex, m_dimensions.x);
}
