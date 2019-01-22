#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Block.hpp"

class Map;

#define MAX_SIDES 200

class Entity{
public:
	~Entity();									
	Entity() ;									
	Entity(IntVector2 initialPos, Map* entityMap, std::string texture = "");

	virtual void Update(float deltaSeconds);

	virtual void Render();
	virtual void RenderDevMode();

	virtual bool IsAboutToBeDeleted();

	void RenderHealthBar();

	void SetPosition(IntVector2 newPosition);
	void SetPosition(Vector3 newPosition);
	bool IsPointInForwardView(Vector2 point);

	Map* m_map;
	IntVector2 m_position;
	Vector3 m_worldPosition;
	float m_rotationDegrees;
	float m_ageInSeconds;
	IntVector2 m_facing;
	std::string m_texturePath;

	bool m_aboutToBeDeleted;
	bool m_noClipMode;


};
