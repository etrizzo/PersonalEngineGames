#include "GameOfLife.hpp"

#pragma optimize("", off)

GameOfLife::GameOfLife(std::vector<IntVector2> liveCells)
{
	for (IntVector2 coordinates : liveCells) {
		int chunkIndex = getChunkIndexForCellCoordinates(coordinates);
		unsigned long int cellIndex = GetCellIndex(coordinates.x, coordinates.y);
		ASSERT_OR_DIE(cellIndex != -1, "Input cell has out of bounds coordinates");
		m_chunks[chunkIndex].SetCell(cellIndex, true);
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

	for (int chunkIndex = 0; chunkIndex < NUM_CHUNKS; chunkIndex++) {
		//update this chunk
			//TODO: skip chunks with no live cells in it or in Celling chunks
		//clone temp board for reference
		Chunk newCells = Chunk(m_chunks[chunkIndex]);
		//update board
		for (int i = 0; i < CELLS_PER_CHUNK; i++) {
			int numLiveCells = 0;
			//check each Cell and check live-ness
			//new:
			if (isNorthwestCellAlive(chunkIndex, i)) { numLiveCells++; }
			if (isCellAlive(GetNorthWestCellIndex(i))) { numLiveCells++; }
			if (isCellAlive(GetNorthCellIndex(i))) { numLiveCells++; }
			if (isCellAlive(GetNorthEastCellIndex(i))) { numLiveCells++; }
			if (isCellAlive(GetWestCellIndex(i))) { numLiveCells++; }
			if (isCellAlive(GetEastCellIndex(i))) { numLiveCells++; }
			if (isCellAlive(GetSouthWestCellIndex(i))) { numLiveCells++; }
			if (isCellAlive(GetSouthCellIndex(i))) { numLiveCells++; }
			if (isCellAlive(GetSouthEastCellIndex(i))) { numLiveCells++; }

			//update new cell
			if (numLiveCells == 3) {
				newCells.SetCell(i, true);
				//newCells[i] = true;

				//update debug information
				if (m_printDebugInformation) {
					if (!m_chunks[chunkIndex].isCellAlive(i)) {
						m_debugNumCellsChanged++;
					}
					m_debugNumLiveCells++;
					m_debugLivingCellIndices.push_back(i);
				}
			}
			else if (numLiveCells < 2 || numLiveCells > 3) {
				newCells.SetCell(i, false);

				//update debug information
				if (m_printDebugInformation) {
					if (m_chunks[chunkIndex].isCellAlive(i)) {
						m_debugNumCellsChanged++;
					}
				}
			}
			else {
				//exactly 2 Cells alive, stays the same (update debug information)
				if (m_printDebugInformation) {
					if (m_chunks[chunkIndex].isCellAlive(i)) {
						m_debugNumLiveCells++;
						m_debugLivingCellIndices.push_back(i);
					}
				}
			}
		}

		//update the cell array with the new vals
		m_chunks[chunkIndex] = newCells;
	}
	
}

int GameOfLife::getChunkIndexForCellCoordinates(IntVector2 coords)
{
	int chunkCoordsX = coords.x / CHUNK_SIZE_X;
	int chunkCoordsY = coords.y / CHUNK_SIZE_Y;
	int index = chunkCoordsX + (chunkCoordsY * NUM_CHUNKS_X);
	return index;
}

//bool GameOfLife::isCellAlive(int index)
//{
//	//todo: get the correct chunk
//	return m_chunk.isCellAlive(index);
//}

bool GameOfLife::isCellAlive(int worldX, int worldY)
{
	//TODO: get the correct chunk
	int chunkIndex = getChunkIndexForCellCoordinates(IntVector2(worldX, worldY));
	int chunkCoordsX = worldX / CHUNK_SIZE_X;
	int chunkCoordsY = worldY / CHUNK_SIZE_Y;
	int cellCoordsX = worldX - (chunkCoordsX * CHUNK_SIZE_X);
	int cellCoordsY = worldY - (chunkCoordsY * CHUNK_SIZE_Y);
	return m_chunks[chunkIndex].isCellAlive(cellCoordsX,cellCoordsY);
}

bool GameOfLife::isCellAliveForChunk(int cellIndex, int chunkIndex)
{
	return m_chunks[chunkIndex].isCellAlive(cellIndex);
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

bool GameOfLife::isNorthwestCellAlive(int chunkIndex, int cellIndex) const
{
	int cellIndex = GetNorthWestCellIndex(cellIndex);
	if (IsOnNorthEdge(cellIndex)) {
		if (IsOnWestEdge(cellIndex)) {
			//get northwest chunk
			
		}
		else {
			//get north chunk
		}
	}
	return false;
}

int GameOfLife::GetNorthCellIndex(int index) const
{
	if (IsOnNorthEdge(index)) {
		//wrap around - the y component should go from max to 0
		return (index & (~CHUNK_MASK_Y));
	}
	else {
		return index + CHUNK_SIZE_X;
	}
}

int GameOfLife::GetWestCellIndex(int index) const
{
	if (IsOnWestEdge(index)) {
		//wrap around - the x component goes from 0 to max
		return (index | CHUNK_MASK_X);
	}
	else {
		return index - 1;
	}
}

int GameOfLife::GetEastCellIndex(int index) const
{
	if (IsOnEastEdge(index)) {
		//wrap around - the x component goes from max to 0
		return (index & (~CHUNK_MASK_X));
	}
	else {
		return index + 1;
	}
}

int GameOfLife::GetSouthCellIndex(int index) const
{
	if (IsOnSouthEdge(index)) {
		//wrap around - the y component should go from max to 0
		return (index | CHUNK_MASK_Y);
	}
	else {
		return index - CHUNK_SIZE_X;
	}
}

int GameOfLife::GetNorthWestCellIndex(int index) const
{
	int north = GetNorthCellIndex(index);
	return GetWestCellIndex(north);
}

int GameOfLife::GetNorthEastCellIndex(int index) const
{
	int north = GetNorthCellIndex(index);
	return GetEastCellIndex(north);
}


int GameOfLife::GetSouthWestCellIndex(int index) const
{
	int south = GetSouthCellIndex(index);
	return GetWestCellIndex(south);
}

int GameOfLife::GetSouthEastCellIndex(int index) const
{
	int south = GetSouthCellIndex(index);
	return GetEastCellIndex(south);
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
