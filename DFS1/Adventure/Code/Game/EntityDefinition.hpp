#pragma once
#include "Game/GameCommon.hpp"

class Map;
class SpriteAnimSetDef;
class ItemDefinition;

class EntityDefinition{
public: 
	EntityDefinition(){};
	EntityDefinition(tinyxml2::XMLElement* entityDefElement);
	~EntityDefinition();

	std::string m_name = "NONAME";
	bool m_useHumanoidAnimSet = false;

	//drawing
	float	m_drawingRadius			= .5f;
	float	m_physicsRadius			= .35f;
	float	m_aspectRatio			= 1.f;
	AABB2	m_boundingBox			= AABB2(0.f,0.f,0.f,0.f);
	bool	m_drawWithBounds		= false;
	Vector2 m_pivot					= Vector2::HALF;

	//health
	int		m_maxHealth				= 1;
	int		m_startingHealth		= 1;

	//movement
	bool	m_canWalk	= false;
	bool	m_canFly	= false;
	bool	m_canSwim	= false;
	float	m_walkSpeed	= 0.f;
	float	m_flySpeed	= 0.f;
	float	m_swimSpeed	= 0.f;

	//sight
	float	m_spinDegreesPerSecond	= 0.f;
	float	m_forwardViewAngle		= 0.f;
	float	m_range					= 0.f;

	std::vector<ItemDefinition*> m_startingInventory;
	std::vector<float> m_inventoryProbabilities;
	//IntRange m_startingStats[NUM_STATS];
	SpriteAnimSetDef* m_spriteAnimSetDef;


private:
	void ParseMovement(tinyxml2::XMLElement* movementElement);
	void ParseSize(tinyxml2::XMLElement* sizeElement);
	void ParseHealth(tinyxml2::XMLElement* healthElement);
	void ParseStartingInventory(tinyxml2::XMLElement* inventoryElment);
};