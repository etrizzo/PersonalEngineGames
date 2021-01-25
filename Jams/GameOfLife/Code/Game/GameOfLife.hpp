#pragma once
#include "GameCommon.hpp"

typedef bool Cell;
typedef unsigned long long int ul_int;

constexpr int   BOARD_BITS_X = 5;
constexpr int   BOARD_BITS_Y = 5;

constexpr ul_int	BOARD_SIZE_X = ((ul_int)1 << BOARD_BITS_X);
constexpr ul_int	BOARD_SIZE_Y = ((ul_int)1 << BOARD_BITS_Y);

//Masks for the bitwise components of a coordinate
//for a 16x16 board (4 bits each for x & y):
//		BOARD_MASK_X = 0000 1111
//		BOARD_MASK_Y = 1111 0000
constexpr ul_int BOARD_MASK_X = BOARD_SIZE_X - 1;
constexpr ul_int BOARD_MASK_Y = (BOARD_SIZE_Y - 1) << BOARD_BITS_X;

constexpr ul_int NUM_CELLS = BOARD_SIZE_X * BOARD_SIZE_Y;

class GameOfLife {
public:
	GameOfLife(std::vector<IntVector2> liveCells);
	
	std::vector<Cell> m_cells;

	void Tick();

	Cell CellAt(int x, int y);
	Cell CellAt(int index);
	ul_int GetCellIndex(int x, int y);

	std::string GetDebugInformation() const;

protected:
	//these functions assume a 2^n width/height for the board
	int GetNorthWestNeighborIndex(int index) const;	
	int GetNorthNeighborIndex(int index) const;
	int GetNorthEastNeighborIndex(int index) const;
	int GetWestNeighborIndex(int index) const;
	int GetEastNeighborIndex(int index) const;
	int GetSouthWestNeighborIndex(int index) const;
	int GetSouthNeighborIndex(int index) const;
	int GetSouthEastNeighborIndex(int index) const;

	bool IsOnNorthEdge(int index) const;
	bool IsOnSouthEdge(int index) const;
	bool IsOnEastEdge(int index) const;
	bool IsOnWestEdge(int index) const;

	//debugging information for large boards (where graphical debugging is impossible)
	bool m_printDebugInformation = true;
	int m_debugNumLiveCells = 0;
	int m_debugNumCellsChanged = 0;
	std::vector<int> m_debugLivingCellIndices;
};