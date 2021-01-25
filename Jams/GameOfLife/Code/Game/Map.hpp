#pragma once
#include "Game/Entity.hpp"
#include "Game/MapDefinition.hpp"

class GameOfLife;


class Map{
	

public:
	Map(){}
	~Map();
	Map(std::vector<IntVector2> liveCells);

	void Render();
	void RenderTiles();
	void RenderCellInformation();

	void Update(float deltaSeconds);


	int GetWidth() const;
	int GetHeight() const;

	GameOfLife* m_gameOfLife = nullptr;

	Renderable2D* m_renderable;

	void SetAutoTick(bool value) { m_autoTick = value; };
	bool IsAutoTick() const { return m_autoTick; };

	void IncreaseAutoTickRate(const float increment);

private:
	bool m_renderBoard = true;
	bool m_autoTick = false;

	StopWatch m_autoTickWatch;
	float m_autoTickRate = 0.1f;
};



