//#pragma once
//#include "Engine/Math/IntVector2.hpp"
//#include "Game/GameCommon.hpp"
//#include "Game/TileDefinition.hpp"
//
//class Tile{
//public:
//	~Tile(){};
//	Tile() {};
//	Tile(int x, int y, TileDefinition* tileDef = nullptr);
//	Tile (IntVector2& coords, TileDefinition* tileDef = nullptr);
//
//	IntVector2 m_coordinates;
//	TileDefinition* m_tileDef = nullptr;
//	float m_health;
//
//	void DamageTile(TileDefinition* typeIfDestroyed, int damage = 1);
//	void GetNeighbors(Tile (&neighbors)[8]);
//	AABB2 GetBounds() const;
//	Vector2 GetCenter() const;
//	Vector2 GetApproximateCenter() const;
//	TileDefinition* GetTileDefinition();
//
//	void SetType(TileDefinition* newType);
//
//};