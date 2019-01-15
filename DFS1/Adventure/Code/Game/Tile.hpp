#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Tags.hpp"

enum eTileSpriteLayer{
	//SPRITE_BASE,
	SPRITE_COSMETIC_BASE,		//this is default set to the tiledef's sprite
	SPRITE_LOW_PRIORITY_EDGE,
	SPRITE_HIGH_PRIORITY_EDGE,
	SPRITE_SHORE_EDGE,
	SPRITE_OVERLAY,
	NUM_SPRITE_LAYERS
};

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

	bool HasTerrainDefinition(TileDefinition* def) const;
	int GetTerrainLevel() const;
	eTerrainLayer GetCosmeticTerrainLayer() const;
	eTerrainLayer GetTerrainLayer() const;
	eGroundLayer GetGroundLayer() const;

	bool HasBeenSpawnedOn() const;
	void MarkAsSpawned();
	void DamageTile(TileDefinition* typeIfDestroyed, int damage = 1);
	//void GetNeighbors(Tile (&neighbors)[8]);
	AABB2 GetBounds() const;
	Vector2 GetCenter() const;
	Vector2 GetApproximateCenter() const;
	TileDefinition* GetTileDefinition();

	int GetWeight() const ;
	void SetWeight(int weight) ;

	void RenderTag();

	void SetType(TileDefinition* newType);
	void AddOverlaySpriteFromTileSheet(AABB2 spriteCoords, eTileSpriteLayer layer);

private:
	


};


class TileExtraInfo{
public:
	TileExtraInfo();
	void SetSpriteCoords(const AABB2& coords, eTileSpriteLayer layer);
	void AddTag(std::string tag);
	void RemoveTag(std::string tag);
	int m_variant = 0;
	int m_weight = 0;
	bool m_spawnedOn = false;
	//float m_lightLevel = 1.f;		//[0-1]?
	Tags m_tags = Tags();

	TileDefinition* m_terrainDef = nullptr;
	TileDefinition* m_cosmeticBaseDefinition = nullptr;
	std::vector<AABB2*> m_spriteCoords = std::vector<AABB2*>();
};