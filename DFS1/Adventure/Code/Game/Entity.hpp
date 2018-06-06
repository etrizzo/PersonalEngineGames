#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"

class Map;
class SpriteAnimSet;
class EntityDefinition;
class Item;

class Entity {
public:
	~Entity();									
	Entity() ;									
	Entity(EntityDefinition* entityDef, Map* entityMap, Vector2 initialPos, float initialRotation);
	virtual void Update(float deltaSeconds);
	virtual void UpdateRenderable();

	virtual void Render();
	virtual void RenderDevMode();
	virtual void RenderDevDisc(RGBA color);
	virtual std::string GetAnimName();		//returns current animation name

	virtual bool IsAboutToBeDeleted();

	virtual void RunCorrectivePhysics();
	virtual void RunWorldPhysics();
	virtual void RunEntityPhysics();
	void CheckTileForCollisions(Tile tile, Vector2 closestPoint);
	virtual void EnterTile(Tile* tile);

	void RenderHealthBar();
	void RenderHealthInBox(AABB2 healthBox);
	void RenderName();

	
	
	bool IsPointInForwardView(Vector2 point);
	bool IsSameFaction(Entity* otherEntity) const;
	void AddItemToInventory(Item* itemToAdd);

	Vector2		GetPosition()	const;
	float		GetRotation()	const;
	Renderable2D* GetRenderable() const;
	Transform2D&  GetTransform()	const;

	virtual void SetPosition(Vector2 newPosition, Map* newMap = nullptr);
	void SetRotation(float newRot);
	void SetScale(float uniformScale);
	void SetScale(Vector2 scale);
	void Rotate(float offset);
	void Translate(Vector2 offset);



	Renderable2D* m_renderable;

	const EntityDefinition* m_definition;
	Map* m_map;
	//Vector2 m_position;
	float m_rotationDegrees;
	float m_ageInSeconds;
	Vector2 m_facing;
	//std::string m_texturePath;
	int m_health;
	float m_speed;
	Tile* m_currentTile;
	AABB2 m_lastUVs;
	

	std::string m_faction;
	std::vector<Item*> m_inventory;

	AABB2 m_localDrawingBox;
	Disc2 m_physicsDisc;

	bool m_aboutToBeDeleted;
	bool m_noClipMode;

	SpriteAnimSet* m_animSet;

	


protected:
	bool CanEnterTile(Tile tileToEnter);
	void UpdateSpeed();
	void FillInventory();

	//void CheckTile(Tile tileToEnter);

};
