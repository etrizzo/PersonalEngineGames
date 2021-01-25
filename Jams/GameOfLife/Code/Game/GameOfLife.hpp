#pragma once
#include "GameCommon.hpp"

/*
Thoughts:
	- the core of this problem comes from the potential size of the data set - input coordinates
		can be anywhere in the 64-bit integer range, so the board must be 64-bit x 64-bit large!
	- storing the board itself is a huge problem.
		- since cells exist in a purely binary state (alive or dead), we can store this in bits rather than
			whole data types
		- My plan is to divide the board up into 4-bit x 4-bit chunks, stored in a unsigned char
*/

typedef unsigned long long int ul_int;

constexpr int   CHUNK_BITS_X = 2;
constexpr int   CHUNK_BITS_Y = 2;

constexpr ul_int	CHUNK_SIZE_X = ((ul_int)1 << CHUNK_BITS_X);
constexpr ul_int	CHUNK_SIZE_Y = ((ul_int)1 << CHUNK_BITS_Y);

//Masks for the bitwise components of a coordinate
//for a 16x16 board (4 bits each for x & y):
//		BOARD_MASK_X = 0000 1111
//		BOARD_MASK_Y = 1111 0000
constexpr ul_int CHUNK_MASK_X = CHUNK_SIZE_X - 1;
constexpr ul_int CHUNK_MASK_Y = (CHUNK_SIZE_Y - 1) << CHUNK_BITS_X;

constexpr ul_int CELLS_PER_CHUNK = CHUNK_SIZE_X * CHUNK_SIZE_Y;

class Chunk {
public:
	ul_int m_cells = 0;
	//IntVector2 m_chunkcoords;

	void SetCell(int index, bool isAlive);
	void SetCell(int cellX, int cellY, bool isAlive);
	bool isCellAlive(int index);
	bool isCellAlive(int cellX, int cellY);

	int getCellIndex(int cellX, int cellY);
};

class GameOfLife {
public:
	GameOfLife(std::vector<IntVector2> liveCells);
	
	Chunk m_chunk;
	//std::vector<Cell> m_cells;

	void Tick();

	bool isCellAlive(int index);
	bool isCellAlive(int x, int y);
	//Cell CellAt(int x, int y);
	//Cell CellAt(int index);
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