#pragma once
#include "GameCommon.hpp"

/*
Thoughts:
	- the core of this problem comes from the potential size of the data set - input coordinates
		can be anywhere in the 64-bit integer range, so the board must be 2^64 x 2^64 large!
	- storing the board itself is a huge problem.
		- since cells exist in a purely binary state (alive or dead), we can store this in bits rather than
			whole data types
		- My plan is to divide the board up into 4-bit x 4-bit chunks, stored in a unsigned char
*/

typedef unsigned long int ul_int;

constexpr int   CHUNK_BITS_X = 2;
constexpr int   CHUNK_BITS_Y = 2;

constexpr int	CHUNK_SIZE_X = ((ul_int)1 << CHUNK_BITS_X);
constexpr int	CHUNK_SIZE_Y = ((ul_int)1 << CHUNK_BITS_Y);

//Masks for the bitwise components of a coordinate
//for a 16x16 board (4 bits each for x & y):
//		BOARD_MASK_X = 0000 1111
//		BOARD_MASK_Y = 1111 0000
constexpr int CHUNK_MASK_X = CHUNK_SIZE_X - 1;
constexpr int CHUNK_MASK_Y = (CHUNK_SIZE_Y - 1) << CHUNK_BITS_X;

constexpr ul_int CELLS_PER_CHUNK = CHUNK_SIZE_X * CHUNK_SIZE_Y;

constexpr int NUM_CHUNKS_X = 3;
constexpr int NUM_CHUNKS_Y = 3;

constexpr int NUM_CHUNKS = NUM_CHUNKS_X * NUM_CHUNKS_Y;


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
	
	Chunk m_chunks[NUM_CHUNKS];
	//std::vector<Cell> m_cells;

	void Tick();

	int getChunkIndexForCellCoordinates(IntVector2 coords);

	//bool isCellAlive(int index);
	bool isCellAlive(int x, int y);
	bool isCellAliveForChunk(int cellIndex, int chunkIndex);
	//Cell CellAt(int x, int y);
	//Cell CellAt(int index);
	ul_int GetCellIndex(int x, int y);

	std::string GetDebugInformation() const;

protected:
	bool isNorthwestCellAlive(int chunkIndex, int cellIndex) const;

	//these functions assume a 2^n width/height for the board
	int GetNorthWestCellIndex(int index) const;	
	int GetNorthCellIndex(int index) const;
	int GetNorthEastCellIndex(int index) const;
	int GetWestCellIndex(int index) const;
	int GetEastCellIndex(int index) const;
	int GetSouthWestCellIndex(int index) const;
	int GetSouthCellIndex(int index) const;
	int GetSouthEastCellIndex(int index) const;

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