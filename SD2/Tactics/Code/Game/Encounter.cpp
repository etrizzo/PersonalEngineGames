#include "Encounter.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Game/FlyoutText.hpp"
#include "Game/Warrior.hpp"
#include "Game/Archer.hpp"
#include "Game/Mage.hpp"

Encounter::Encounter()
{
}

Encounter::~Encounter()
{
}

void Encounter::Update(float deltaSeconds)
{
	m_map->Update(deltaSeconds);
	RemoveDeadActors();
}

void Encounter::Render()
{
	m_map->Render();
	
}

void Encounter::RenderEntities()
{
	m_map->RenderEntities();
	if (g_theGame->m_devMode){
		g_theGame->SetUICamera();
		AABB2 actorBox = AABB2::ZERO_TO_ONE;
		std::string actorString = "";
		for (Actor* actor : m_actors){
			actorString+= "Actor ";
			actorString= actorString + std::to_string((int)actor->m_faction) + ": wait =" + std::to_string(actor->m_waitTime) + " health=" + std::to_string(actor->m_health) + "\n";
		}
		g_theRenderer->DrawTextInBox2D(actorString, actorBox, Vector2::ZERO, .1f, TEXT_DRAW_SHRINK_TO_FIT);
		g_theGame->SetMainCamera();
	}
}

void Encounter::RenderTurnWidget()
{
	AABB2 UIBounds = g_theGame->GetUIBounds();

	AABB2 turnBox = UIBounds.GetPercentageBox(.3f, .87f, .7f, .97f);
	AABB2 actorBox = turnBox.GetPercentageBox(0.f, 0.f, .125f, 1.f);	//get 1/8 the size of the turn widget
	float width = actorBox.GetWidth();
	AABB2 innerActorBox = actorBox.GetPercentageBox(.05f,.05f,.95f,.95f);

	g_theRenderer->DrawAABB2(turnBox, RGBA::BLACK.GetColorWithAlpha(200));
	g_theRenderer->DrawAABB2Outline(turnBox, RGBA::WHITE);
	for (int i = 0; i < 8; i++){
		//draw each actor
		unsigned char alpha = 200;
		if (i == 0){
			alpha = 255;
		}
		Actor* actor = m_actors[i % m_actors.size()];		//when fewer than 8 actors, assume same turn order
		Sprite* portrait = actor->GetDisplaySprite();
		
		g_theRenderer->DrawAABB2(innerActorBox, GetFactionColor(actor->GetFaction()).GetColorWithAlpha((unsigned char) ((float)alpha * .5f)));
		g_theRenderer->DrawAABB2Outline(innerActorBox, RGBA::WHITE.GetColorWithAlpha(alpha));
		g_theRenderer->DrawSpriteOnAABB2(innerActorBox, portrait, RGBA::WHITE.GetColorWithAlpha(alpha));
		//g_theRenderer->DrawAABB2Outline(border, RGBA::WHITE);

		//actorBox.Translate(width, 0.f);
		innerActorBox.Translate(width, 0.f);
	}
	actorBox.Translate(0.f, (actorBox.GetHeight() * -.21f));
	g_theRenderer->DrawTextInBox2D("^", actorBox, Vector2(.5f,0.f), actorBox.GetHeight() * .2f);
}


bool Encounter::LoadMap(char const * filename)
{
	m_map = new Map(this);
	return m_map->LoadFromHeightMap(filename, g_gameConfigBlackboard.GetValue("mapHeight", 32));;
}

Actor * Encounter::AddActor(IntVector2 pos, Faction team, std::string name)
{
	//std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	for (int i=0; i< (int) name.length(); i++)
		name[i] =  (char) std::tolower(name[i]);
	//error checking
	if (!m_map->IsOnBoard(pos)){
		return nullptr;
	}

	Actor* actor =nullptr;
	if (name == "warrior"){
		actor = (Actor*) (new Warrior(pos, m_map, team));		//replace with like a factory function
	}
	if (name == "archer"){
		actor = (Actor*) (new Archer(pos, m_map, team));
	}
	if (name=="mage"){
		actor = (Actor*) (new Mage(pos, m_map, team));
	}

	if (actor != nullptr){
		m_actors.push_back(actor);
		m_map->AddEntityToMap((Entity*) actor);
	}
	return actor;
}

FlyoutText * Encounter::AddFlyoutText(const std::string & text, const Vector3 & pos, float textSize, float durationSeconds, const RGBA & tint, float speed)
{
	FlyoutText* flyout = new FlyoutText(text, pos, textSize, durationSeconds, tint, speed);
	m_map->AddEntityToMap((Entity*) flyout);
	return flyout;
}

void Encounter::DestroyAllActors()
{
	for (Actor* actor : m_actors){
		m_map->RemoveEntityFromMap((Entity*) actor);
	}
	for(Actor* actor : m_actors){
		delete actor;
	}
	m_actors.clear();
}

unsigned int Encounter::GetActorCount()
{
	return m_actors.size();
}

Actor * Encounter::GetActor(unsigned int index) const
{
	return m_actors[index];
}

Actor * Encounter::GetActorInColumn(IntVector2 tilePos) const
{
	for(Actor* actor : m_actors){
		if (actor->m_position == tilePos){
			return actor;
		}
	}
	return nullptr;
}

void Encounter::SortActorsByWaitTime()
{
 	bool sorted = false;
	if (m_actors.size() > 1){
		while (!sorted){
			sorted = true;
			for (unsigned int checkIndex = 0; checkIndex < m_actors.size() - 1; checkIndex++){
				Actor* currentActor = m_actors[checkIndex];
				Actor* nextActor = m_actors[checkIndex+1];
				if (nextActor != nullptr && currentActor != nullptr){
					if (currentActor->m_waitTime > nextActor->m_waitTime){
						sorted = false;
						Actor* temp = m_actors[checkIndex];
						m_actors[checkIndex] = m_actors[checkIndex+1];
						m_actors[checkIndex+1] = temp;
					}
				}
			}
		}
	}
}

//void Encounter::MoveCursor(IntVector2 direction)
//{
//	IntVector2 newNorth = FindNewNorth();		//figure out which direction is the new "North" (away from camera)
//	IntVector2 newDir = MapDirectionToNorth(direction, newNorth);		//given the original direction (north, east, south, west) return the actual direction to move
//	m_map->MoveCursor(newDir);
//}

//void Encounter::SetMapCursorPos(IntVector2 newPos)
//{
//	m_map->SetCursorPos(newPos);
//}

bool Encounter::CheckForVictory()
{
	if (GetActorCount() == 0){
		return true;		//not really a victory??
	}
	//if you find more than 1 faction, return false
	Faction winningFaction = m_actors[0]->m_faction;
	for (Actor* actor : m_actors){
		if (actor->m_faction != winningFaction){
			return false;
		}
	}
	return true;
}

Actor * Encounter::AdvanceToNextReadyActor()
{
	float minWaitTime = 100.f;
	Actor* nextActor = nullptr;
	for(Actor* actor : m_actors){
		if (actor->m_waitTime <= minWaitTime){
			if (actor->m_waitTime == minWaitTime){		//check for matching wait times
				//cover case where both actors start with MAX_WAIT_TIME
				if (nextActor == nullptr){
					nextActor = actor;
					minWaitTime = actor->m_waitTime;
				} else {  //choose actor with smallest wait time
					if (nextActor->m_turnsSinceLastMove < actor->m_turnsSinceLastMove ){
						nextActor = actor;
						minWaitTime = actor->m_waitTime;
					}
				}
			} else {
				//actor should move sooner
				nextActor = actor;
				minWaitTime = actor->m_waitTime;
			}
		}
	}

	//pass time for other actors 
	
	for (Actor* actor : m_actors){
		actor->PassTime(minWaitTime);
	}
	return nextActor;
}



void Encounter::RemoveDeadActors()
{
	for (int actorIndex = 0; actorIndex < (int)m_actors.size(); actorIndex++){
		if (m_actors[actorIndex] != nullptr && m_actors[actorIndex]->IsAboutToBeDeleted()){
			delete(m_actors[actorIndex]);
			m_actors.erase(m_actors.begin() + actorIndex);
			
		}
	}
}
