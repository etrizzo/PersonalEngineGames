#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/BlockDefinition.hpp"

class Block{
public:
	~Block(){};
	Block() {};
	Block(int x, int y, int z = 0, BlockDefinition* tileDef = nullptr);
	Block (IntVector3& coords, BlockDefinition* tileDef = nullptr);

	IntVector3 m_coordinates;
	BlockDefinition* m_blockDef = nullptr;
	float m_health;

	//void GetNeighbors(Block (&neighbors)[8]);
	AABB3 GetBounds() const;
	Vector3 GetDimensions() const;
	Vector3 GetCenter() const;
	Vector3 GetTopCenter() const;
	Plane GetTopPlane() const;
	BlockDefinition* GetTileDefinition();
	IntVector2 GetXYCoords() const;
	bool IsAir() const { return m_blockDef == nullptr; };

	void SetType(BlockDefinition* newType);

};