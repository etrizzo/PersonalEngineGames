#include "Game/GameCommon.hpp"

class Item;
class Actor;

class Party{
	Party();



	std::vector<Actor*> m_partyMembers;
	std::vector<Item*> m_inventory;
};
