#include "Party.hpp"
#include "Game/Actor.hpp"

Party::Party()
{
	m_partyMembers = std::vector<Actor*>();
	m_inventory = std::vector<Item*>();
	m_currentPlayer = nullptr;
	m_currentMap = nullptr;
	m_currentPlayerIndex = -1;
}

void Party::Update(float ds)
{
	/*for (Actor* actor : m_partyMembers){
		actor->Update(ds);
	}*/
}

void Party::RunCorrectivePhysics()
{
	/*for (Actor* actor : m_partyMembers){
		actor->RunCorrectivePhysics();
	}*/
}

void Party::HandleInput()
{
	m_currentPlayer->HandleInput();
	if (g_primaryController != nullptr){
		if ((g_primaryController->WasButtonJustPressed(XBOX_A) || g_theInput->WasKeyJustPressed(VK_SPACE))){
			m_currentPlayer->StartFiringArrow();
		}
	}
}

void Party::MovePartyToMap(Map * newMap, Vector2 playerPos)
{
	for (Actor* actor : m_partyMembers){
		m_currentMap->RemoveActorFromMap(actor);
	}
	m_currentMap = newMap;
	float offsetSize = .1f;
	for (Actor* actor : m_partyMembers){
		Vector2 offset = Vector2(GetRandomFloatInRange(-offsetSize, offsetSize), GetRandomFloatInRange(-offsetSize, offsetSize));
		actor->SetPosition(playerPos + offset, newMap);
	}

}

void Party::AddActorToParty(Actor * newActor)
{
	//newActor->m_map->RemoveActorFromMap(newActor);
	m_partyMembers.push_back(newActor);
	for (Item* item : newActor->m_inventory){
		m_inventory.push_back(item);
	}
	//newActor->m_map->m_scene->AddRenderable(newActor->m_renderable);
	newActor->SetFollowTarget(m_currentPlayer);
}

void Party::AddActorAndSetAsPlayer(Actor * newPlayer)
{
	m_partyMembers.push_back(newPlayer);
	for (Item* item : newPlayer->m_inventory){
		m_inventory.push_back(item);
	}
	m_currentPlayer = newPlayer;
	m_currentPlayerIndex = m_partyMembers.size() -1;
}

void Party::SetMap(Map * currentMap)
{
	m_currentMap = currentMap;
}

void Party::AddItemToInventory(Item * item)
{
	m_inventory.push_back(item);
}

void Party::EquipOrUnequipItem(Item * itemToEquip)
{
	m_currentPlayer->EquipOrUnequipItem(itemToEquip);
}

Actor * Party::GetPlayerCharacter() const
{
	return m_currentPlayer;
}

bool Party::IsEmpty() const
{
	return m_partyMembers.size() == 0;
}
