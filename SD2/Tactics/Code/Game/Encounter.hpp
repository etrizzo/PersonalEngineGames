#pragma once
#include "Game/GameCommon.hpp"


enum Faction{
	FACTION_RED,
	FACTION_BLUE,
	NUM_FACTIONS
};

class Map;
class Actor;
class FlyoutText;

class Encounter 
{
public:
	Encounter(); 
	~Encounter(); 

	void Update(float deltaSeconds);
	void Render();
	void RenderEntities();
	void RenderTurnWidget();
	//void HandleInput();

	bool LoadMap( char const *filename ); 
	Map* GetMap() { return m_map; }

	Actor* AddActor( IntVector2 pos, Faction team, std::string name = ""); 
	FlyoutText* AddFlyoutText(const std::string& text, const Vector3& pos, float textSize = .5f, float durationSeconds= .5f, const RGBA& tint = RGBA::WHITE, float speed = 3.f);
	void DestroyAllActors(); 

	unsigned int GetActorCount();
	Actor* GetActor( unsigned int index ) const;
	Actor *GetActorInColumn( IntVector2 tilePos ) const; 

	void SortActorsByWaitTime();

	//void MoveCursor(IntVector2 direction);
	//void SetMapCursorPos(IntVector2 newPos);

	bool CheckForVictory();
	Actor* AdvanceToNextReadyActor();

public:
	


private:
	

	void RemoveDeadActors();

	Map* m_map;
	std::vector<Actor*> m_actors;		//the map will also store actors in the entity list, this will manage actor interactions (??)
};
