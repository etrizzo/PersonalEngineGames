#include "GameOfLife.hpp"

#pragma optimize("", off)

GameOfLife::GameOfLife(std::vector<IntVector2> liveCells)
{
	m_cells = std::vector<bool>(NUM_CELLS, false);
	for (IntVector2 coordinates : liveCells) {
		unsigned long int cellIndex = GetCellIndex(coordinates.x, coordinates.y);
		ASSERT_OR_DIE(cellIndex != -1, "Input cell has out of bounds coordinates");
		m_cells[cellIndex] = true;
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
	std::vector<bool> newCells = m_cells;
	//update board
	for (int i = 0; i < m_cells.size(); i++) {
		int numLiveNeighbors = 0;
		//check each neighbor and check live-ness
		if (CellAt(GetNorthWestNeighborIndex(i)	)) { numLiveNeighbors++; }
		if (CellAt(GetNorthNeighborIndex(i)		)) { numLiveNeighbors++; }
		if (CellAt(GetNorthEastNeighborIndex(i)	)) { numLiveNeighbors++; }
		if (CellAt(GetWestNeighborIndex(i)		)) { numLiveNeighbors++; }
		if (CellAt(GetEastNeighborIndex(i)		)) { numLiveNeighbors++; }
		if (CellAt(GetSouthWestNeighborIndex(i)	)) { numLiveNeighbors++; }
		if (CellAt(GetSouthNeighborIndex(i)		)) { numLiveNeighbors++; }
		if (CellAt(GetSouthEastNeighborIndex(i)	)) { numLiveNeighbors++; }

		//update new cell
		if (numLiveNeighbors == 3) {
			newCells[i] = true;

			//update debug information
			if (m_printDebugInformation) {
				if (!m_cells[i]) {
					m_debugNumCellsChanged++;
				}
				m_debugNumLiveCells++;
				m_debugLivingCellIndices.push_back(i);
			}
		}
		else if (numLiveNeighbors < 2 || numLiveNeighbors > 3) {
			newCells[i] = false;

			//update debug information
			if (m_printDebugInformation) {
				if (m_cells[i]) {
					m_debugNumCellsChanged++;
				}
			}
		}
		else {
			//exactly 2 neighbors alive, stays the same (update debug information)
			if (m_printDebugInformation) {
				if (m_cells[i]) {
					m_debugNumLiveCells++;
					m_debugLivingCellIndices.push_back(i);
				}
			}
		}
	}

	//update the cell array with the new vals
	m_cells = newCells;
}

Cell GameOfLife::CellAt(int x, int y)
{
	int index = GetCellIndex(x, y);
	return CellAt(index);
}

Cell GameOfLife::CellAt(int index)
{
	ASSERT_OR_DIE(index >= 0 && index < m_cells.size(), "Trying to access out a cell out of bounds");
	return  m_cells[index];
}

ul_int GameOfLife::GetCellIndex(int x, int y)
{
	ASSERT_OR_DIE(y >= 0 && y < BOARD_SIZE_Y && x >= 0 && x < BOARD_SIZE_X, "Trying to access cell out of bounds");
	return x + (y * BOARD_SIZE_X);
}

std::string GameOfLife::GetDebugInformation() const
{
	std::string info = Stringf("num cells: %i \nnum live cells: %i \nnum cells changed: %i\nLive Cells:", m_cells.size(), m_debugNumLiveCells, m_debugNumCellsChanged);
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
		return (index & (~BOARD_MASK_Y));
	}
	else {
		return index + BOARD_SIZE_X;
	}
}

int GameOfLife::GetWestNeighborIndex(int index) const
{
	if (IsOnWestEdge(index)) {
		//wrap around - the x component goes from 0 to max
		return (index | BOARD_MASK_X);
	}
	else {
		return index - 1;
	}
}

int GameOfLife::GetEastNeighborIndex(int index) const
{
	if (IsOnEastEdge(index)) {
		//wrap around - the x component goes from max to 0
		return (index & (~BOARD_MASK_X));
	}
	else {
		return index + 1;
	}
}

int GameOfLife::GetSouthNeighborIndex(int index) const
{
	if (IsOnSouthEdge(index)) {
		//wrap around - the y component should go from max to 0
		return (index | BOARD_MASK_Y);
	}
	else {
		return index - BOARD_SIZE_X;
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
	return (index & BOARD_MASK_Y) == BOARD_MASK_Y;
}

bool GameOfLife::IsOnSouthEdge(int index) const
{
	//anything on the south edge of the board will have no bits in the y component set
	return (index & BOARD_MASK_Y) == 0;
}

bool GameOfLife::IsOnEastEdge(int index) const
{
	//anything on the east edge of the board will have all bits in the x component set
	return (index & BOARD_MASK_X) == BOARD_MASK_X;
}

bool GameOfLife::IsOnWestEdge(int index) const
{
	//anything on the west edge of the board will have no bits in the x component set
	return (index & BOARD_MASK_X) == 0;
}
