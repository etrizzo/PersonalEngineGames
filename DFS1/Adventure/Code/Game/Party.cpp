#include "Party.hpp"
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Game/Item.hpp"

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

void Party::CheckForKilledPlayers()
{
	for (int i = m_partyMembers.size() - 1; i >= 0; i--){
		Actor* member = m_partyMembers[i];
		if (member->m_dead || member->m_aboutToBeDeleted){
			if (member->m_isPlayer){
				if (m_partyMembers.size() > 1){
					SwapPlayer(1);
				} else {
					g_theGame->TransitionToState(new GameState_Defeat(g_theGame->m_currentState));
				}
			} 
			RemoveAtFast(m_partyMembers, i);
		}
	}
}

void Party::RenderPartyUI(AABB2 renderBox)
{
	float height = renderBox.GetHeight();
	for (int i = 0; i < m_partyMembers.size(); i++){
		int index = (m_currentPlayerIndex + i) % m_partyMembers.size();
		Actor* actor = m_partyMembers[index];
		RenderPartyMemberUI(actor, renderBox);
		renderBox.Translate(0.f, -height);
		if (i == 0){
			renderBox = renderBox.GetPercentageBox(0.f, .3f, .7f, 1.f);
			height *= .7f;
		}
	}

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

	if (g_theInput->WasKeyJustPressed(VK_OEM_6) || g_theInput->WasKeyJustPressed(XBOX_D_RIGHT)){
		SwapPlayer(1);
	}
	if (g_theInput->WasKeyJustPressed(VK_OEM_4) || g_theInput->WasKeyJustPressed(XBOX_D_LEFT)){
		SwapPlayer(-1);
	}
}

void Party::VoidMaps()
{
	m_currentMap = nullptr;
	for (Actor* actor : m_partyMembers){
		actor->m_map = nullptr;
	}
}

void Party::MovePartyToMap(Map * newMap, Vector2 playerPos)
{
	if (m_currentMap != nullptr){
		for (Actor* actor : m_partyMembers){
			m_currentMap->RemoveActorFromMap(actor);
		}
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
	newActor->EquipItemsInInventory();
	m_partyMembers.push_back(newActor);
	for (Item* item : newActor->m_inventory){
		m_inventory.push_back(item);
	}
	//newActor->m_map->m_scene->AddRenderable(newActor->m_renderable);
	newActor->SetFollowTarget(m_currentPlayer);
	newActor->m_questGiven = nullptr;
	
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

void Party::RemoveItemFromInventory(Item * item)
{
	for (int i = m_inventory.size() - 1; i >= 0; i--){
		if (m_inventory[i] == item){
			if (item->m_currentlyEquipped){
				for (Actor* actor : m_partyMembers){
					if (actor->IsItemEquipped(item)){
						actor->EquipOrUnequipItem(item);
					}
				}
			}
			RemoveAtFast(m_inventory, i);
			break;
		}
	}
}

void Party::EquipOrUnequipItem(Item * itemToEquip)
{
	m_currentPlayer->EquipOrUnequipItem(itemToEquip);
}

void Party::SwapPlayer(int direction)
{
	int size = m_partyMembers.size();
	m_currentPlayerIndex = (m_currentPlayerIndex + direction + size) % size;
	Actor* oldPlayer = m_currentPlayer;
	m_currentPlayer = m_partyMembers[m_currentPlayerIndex];
	m_currentPlayer->m_isPlayer = true;
	oldPlayer->m_isPlayer = false;
	if (m_currentMap != nullptr){
		m_currentMap->m_scene->RemoveRenderable(oldPlayer->m_healthRenderable);
		m_currentMap->m_scene->AddRenderable(m_currentPlayer->m_healthRenderable);
	}
	for (Actor* actor : m_partyMembers){
		actor->SetFollowTarget(m_currentPlayer);
	}
}

Actor * Party::GetPlayerCharacter() const
{
	return m_currentPlayer;
}

bool Party::IsEmpty() const
{
	return m_partyMembers.size() == 0;
}

void Party::RenderPartyMemberUI(Actor* actor, AABB2 memberBox)
{
	float height = memberBox.GetHeight();
	float width = memberBox.GetWidth();
	AABB2 faceBox;
	AABB2 healthBox;
	memberBox.SplitAABB2Vertical(height / width, faceBox, healthBox);
	AABB2 weaponBox = faceBox.GetPercentageBox(.6f, .6f, 1.f, 1.f);

	healthBox.AddPaddingToSides(-height * .3f, -height * .3f);

	actor->RenderFaceInBox(faceBox);
	actor->RenderEquippedWeaponInBox(weaponBox);
	actor->RenderHealthInBox(healthBox);
}
