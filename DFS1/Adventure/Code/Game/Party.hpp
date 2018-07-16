#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"

class Item;
class Actor;

class Party{
public:
	Party();

	void Update(float ds);
	void CheckForKilledPlayers();
	void RenderPartyUI(AABB2 renderBox);
	void RunCorrectivePhysics();
	void HandleInput();
	
	void VoidMaps();	//get rid of any leftover map pointers
	void MovePartyToMap(Map* newMap, Vector2 playerPos = Vector2::ZERO);
	void AddActorToParty(Actor* newActor);
	void AddActorAndSetAsPlayer(Actor* newPlayer);
	void SetMap(Map* currentMap);
	void AddItemToInventory(Item* item);
	void RemoveItemFromInventory(Item* item);
	void EquipOrUnequipItem(Item* itemToEquip);

	void SwapPlayer(int direction = 1);

	Actor* GetPlayerCharacter() const;
	bool IsEmpty() const;

	int m_currentPlayerIndex = 0;
	Actor* m_currentPlayer;
	Map* m_currentMap;

	//no actors in the party will be updated in the maps update
	std::vector<Actor*> m_partyMembers;
	std::vector<Item*> m_inventory;

private:
	void RenderPartyMemberUI(Actor* actor, AABB2 memberBox);
};
