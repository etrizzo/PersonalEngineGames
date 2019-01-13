#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"

class Map;

class TileNeighborSet{
public:

	TileNeighborSet(Tile* center, Map* map);

	/*
	0 1 2
	3 x 4
	5 6 7
	*/
	Tile* m_neighbors[8];
	TileDefinition* m_edgeToLookFor = nullptr;
	Tile* m_center;

	TileDefinition* FindEdgeTileDefinition() ;
	AABB2 GetTileEdge(TileDefinition * edgeTileDef);

	bool IsTileInvalid() const;

	bool MatchesEdgeDefinition(Tile* tile) const;

	Tile* GetLeft() const;
	Tile* GetRight() const;
	Tile* GetTop() const;
	Tile* GetBottom() const;
	Tile* GetTopLeft() const;
	Tile* GetTopRight() const;
	Tile* GetBottomLeft() const;
	Tile* GetBottomRight() const;

	Tile* GetWest() const		{ return GetLeft();			};
	Tile* GetEast() const		{ return GetRight();		};
	Tile* GetNorth() const		{ return GetTop();			};
	Tile* GetSouth() const		{ return GetBottom();		};
	Tile* GetNorthEast() const	{ return GetTopRight();		};
	Tile* GetNorthWest() const	{ return GetTopLeft();		};
	Tile* GetSouthEast() const	{ return GetBottomRight();	};
	Tile* GetSouthWest() const	{ return GetBottomLeft();	};



	bool CheckConcaveTopLeft() const;
	bool CheckConcaveTopRight() const;
	bool CheckConcaveBottomLeft() const;
	bool CheckConcaveBottomRight() const;
	bool CheckConvexTopLeft() const;
	bool CheckConvexTopRight() const;
	bool CheckConvexBottomLeft() const;
	bool CheckConvexBottomRight() const;
	bool CheckPureLeft() const;
	bool CheckPureRight() const;
	bool CheckPureTop() const;
	bool CheckPureBottom() const;


};