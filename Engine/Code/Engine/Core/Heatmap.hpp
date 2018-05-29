#pragma once
#include "Engine/Core/EngineCommon.hpp"

class Heatmap{
public:
	Heatmap(){};
	Heatmap(IntVector2 dimensions, float initialValue);
	~Heatmap(){};

	void ClearHeat(const float& newVal = 9999.f);
	void SetHeat(const IntVector2& cellCoords, const float& heatVal);
	void SetHeat(const int& index, const float& heatVal);
	void AddHeat(const IntVector2& cellCoords, const float& heatVal);
	void AddHeat(const int& index, const float& heatVal);
	float GetHeat(const int& x, const int& y) const;
	float GetHeat(const IntVector2& cellCoords) const;
	float GetHeat(const int& index) const;
	int GetIndexOfMinNeighbor(const int& startIndex) const;
	IntVector2 GetCoordsOfMinNeighbor(const IntVector2& startCoords) const;

protected:
	std::vector<float> m_heatValues = std::vector<float>();
	IntVector2 m_dimensions = IntVector2(0,0);
};