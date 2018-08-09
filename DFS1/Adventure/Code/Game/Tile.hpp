#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Tags.hpp"

class TileExtraInfo;

class Tile{
public:
	~Tile();
	Tile() {};
	Tile(int x, int y, TileDefinition* tileDef = nullptr);
	Tile (IntVector2& coords, TileDefinition* tileDef = nullptr);

	IntVector2 m_coordinates;
	TileExtraInfo* m_extraInfo = nullptr;
	TileDefinition* m_tileDef = nullptr;
	//int m_type;

	void AddTag(std::string tag);
	bool HasTag(std::string tag);

	bool HasBeenSpawnedOn() const;
	void MarkAsSpawned();
	void DamageTile(TileDefinition* typeIfDestroyed, int damage = 1);
	//void GetNeighbors(Tile (&neighbors)[8]);
	AABB2 GetBounds() const;
	Vector2 GetCenter() const;
	Vector2 GetApproximateCenter() const;
	TileDefinition* GetTileDefinition();

	void RenderTag();

	void SetType(TileDefinition* newType);
	void AddOverlaySpriteFromTileSheet(AABB2 spriteCoords);

private:
	


};


class TileExtraInfo{
public:
	TileExtraInfo(){};
	void AddTag(std::string tag);
	void RemoveTag(std::string tag);
	int m_variant = 0;
	bool m_spawnedOn = false;
	float m_lightLevel = 1.f;		//[0-1]?
	Tags m_tags = Tags();

	TileDefinition* m_terrainDef = nullptr;
	TileDefinition* m_cosmeticBaseDef = nullptr;
	std::vector<AABB2> m_overlaySpriteCoords = std::vector<AABB2>();
};