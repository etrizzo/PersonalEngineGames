#include "GameOfLife.hpp"

#pragma optimize("", off)

GameOfLife::GameOfLife(std::vector<IntVector2> liveCells)
{
	m_chunk = Chunk();
	for (IntVector2 coordinates : liveCells) {
		unsigned long int cellIndex = GetCellIndex(coordinates.x, coordinates.y);
		ASSERT_OR_DIE(cellIndex != -1, "Input cell has out of bounds coordinates");
		m_chunk.SetCell(cellIndex, true);
	}
}

void GameOfLife::Tick()
{
	//reset debug information
	if (m_printDebugInformation) {
		m_debugNumLiveCells = 0;
		m_debugNumCellsChanged = 0;
		m_debugLivingCellIndices.clear();
	}

	//clone temp board for reference
	Chunk newCells = Chunk(m_chunk);
	//update board
	for (int i = 0; i < CELLS_PER_CHUNK; i++) {
		int numLiveNeighbors = 0;
		//check each neighbor and check live-ness
		if (isCellAlive(GetNorthWestNeighborIndex(i)	)) { numLiveNeighbors++; }
		if (isCellAlive(GetNorthNeighborIndex(i)		)) { numLiveNeighbors++; }
		if (isCellAlive(GetNorthEastNeighborIndex(i)	)) { numLiveNeighbors++; }
		if (isCellAlive(GetWestNeighborIndex(i)		)) { numLiveNeighbors++; }
		if (isCellAlive(GetEastNeighborIndex(i)		)) { numLiveNeighbors++; }
		if (isCellAlive(GetSouthWestNeighborIndex(i)	)) { numLiveNeighbors++; }
		if (isCellAlive(GetSouthNeighborIndex(i)		)) { numLiveNeighbors++; }
		if (isCellAlive(GetSouthEastNeighborIndex(i)	)) { numLiveNeighbors++; }

		//update new cell
		if (numLiveNeighbors == 3) {
			newCells.SetCell(i, true);
			//newCells[i] = true;

			//update debug information
			if (m_printDebugInformation) {
				if (!m_chunk.isCellAlive(i)) {
					m_debugNumCellsChanged++;
				}
				m_debugNumLiveCells++;
				m_debugLivingCellIndices.push_back(i);
			}
		}
		else if (numLiveNeighbors < 2 || numLiveNeighbors > 3) {
			newCells.SetCell(i, false);

			//update debug information
			if (m_printDebugInformation) {
				if (m_chunk.isCellAlive(i)) {
					m_debugNumCellsChanged++;
				}
			}
		}
		else {
			//exactly 2 neighbors alive, stays the same (update debug information)
			if (m_printDebugInformation) {
				if (m_chunk.isCellAlive(i)) {
					m_debugNumLiveCells++;
					m_debugLivingCellIndices.push_back(i);
				}
			}
		}
	}

	//update the cell array with the new vals
	m_chunk = newCells;
}

bool GameOfLife::isCellAlive(int index)
{
	//todo: get the correct chunk
	return m_chunk.isCellAlive(index);
}

bool GameOfLife::isCellAlive(int x, int y)
{
	//TODO: get the correct chunk
	return m_chunk.isCellAlive(x,y);
}

//Cell GameOfLife::CellAt(int x, int y)
//{
//	//TODO: get the correct chunk
//	return m_chunk.isCellAlive(x, y);
//	/*int index = GetCellIndex(x, y);
//	return CellAt(index);*/
//}
//
//Cell GameOfLife::CellAt(int index)
//{
//	return m_chunk.isCellAlive(index);
//	/*ASSERT_OR_DIE(index >= 0 && index < m_cells.size(), "Trying to access out a cell out of bounds");
//	return  m_cells[index];*/
//}

ul_int GameOfLife::GetCellIndex(int x, int y)
{
	ASSERT_OR_DIE(y >= 0 && y < CHUNK_SIZE_Y && x >= 0 && x < CHUNK_SIZE_X, "Trying to access cell out of bounds");
	return x + (y * CHUNK_SIZE_X);
}

std::string GameOfLife::GetDebugInformation() const
{
	std::string info = Stringf("num cells: %i \nnum live cells: %i \nnum cells changed: %i\nLive Cells:", CELLS_PER_CHUNK, m_debugNumLiveCells, m_debugNumCellsChanged);
	int maxCellsToPrint = min(m_debugNumLiveCells, 10);
	for (int i = 0; i < maxCellsToPrint; i++) {
		std::string cellInfo = Stringf("\n  index: %i", m_debugLivingCellIndices[i]);
		info += cellInfo;
	}
	if (maxCellsToPrint < m_debugLivingCellIndices.size()) {
		info += "\n  ...";
	}
	
	return info;
}

int GameOfLife::GetNorthNeighborIndex(int index) const
{
	if (IsOnNorthEdge(index)) {
		//wrap around - the y component should go from max to 0
		return (index & (~CHUNK_MASK_Y));
	}
	else {
		return index + CHUNK_SIZE_X;
	}
}

int GameOfLife::GetWestNeighborIndex(int index) const
{
	if (IsOnWestEdge(index)) {
		//wrap around - the x component goes from 0 to max
		return (index | CHUNK_MASK_X);
	}
	else {
		return index - 1;
	}
}

int GameOfLife::GetEastNeighborIndex(int index) const
{
	if (IsOnEastEdge(index)) {
		//wrap around - the x component goes from max to 0
		return (index & (~CHUNK_MASK_X));
	}
	else {
		return index + 1;
	}
}

int GameOfLife::GetSouthNeighborIndex(int index) const
{
	if (IsOnSouthEdge(index)) {
		//wrap around - the y component should go from max to 0
		return (index | CHUNK_MASK_Y);
	}
	else {
		return index - CHUNK_SIZE_X;
	}
}

int GameOfLife::GetNorthWestNeighborIndex(int index) const
{
	int north = GetNorthNeighborIndex(index);
	return GetWestNeighborIndex(north);
}

int GameOfLife::GetNorthEastNeighborIndex(int index) const
{
	int north = GetNorthNeighborIndex(index);
	return GetEastNeighborIndex(north);
}


int GameOfLife::GetSouthWestNeighborIndex(int index) const
{
	int south = GetSouthNeighborIndex(index);
	return GetWestNeighborIndex(south);
}

int GameOfLife::GetSouthEastNeighborIndex(int index) const
{
	int south = GetSouthNeighborIndex(index);
	return GetEastNeighborIndex(south);
}

bool GameOfLife::IsOnNorthEdge(int index) const
{
	//anything on the north edge of the board will have all bits in the y component set
	return (index & CHUNK_MASK_Y) == CHUNK_MASK_Y;
}

bool GameOfLife::IsOnSouthEdge(int index) const
{
	//anything on the south edge of the board will have no bits in the y component set
	return (index & CHUNK_MASK_Y) == 0;
}

bool GameOfLife::IsOnEastEdge(int index) const
{
	//anything on the east edge of the board will have all bits in the x component set
	return (index & CHUNK_MASK_X) == CHUNK_MASK_X;
}

bool GameOfLife::IsOnWestEdge(int index) const
{
	//anything on the west edge of the board will have no bits in the x component set
	return (index & CHUNK_MASK_X) == 0;
}

void Chunk::SetCell(int index, bool isAlive)
{
	ul_int bitToSet = 1 << index;
	if (isAlive) {
		//set the bit
		m_cells |= bitToSet;
	}
	else {
		//clear the bit
		m_cells &= (~bitToSet);
	}
}

void Chunk::SetCell(int cellX, int cellY, bool isAlive)
{
	int index = getCellIndex(cellX, cellY);
	SetCell(index, isAlive);
}

bool Chunk::isCellAlive(int index)
{
	ul_int bitToCheck = 1 << index;
	return ((m_cells & bitToCheck) == bitToCheck);
}

bool Chunk::isCellAlive(int cellX, int cellY)
{
	int index = getCellIndex(cellX, cellY);
	return isCellAlive(index);
}

int Chunk::getCellIndex(int cellX, int cellY)
{
	return cellX | (cellY << CHUNK_BITS_X);
}
