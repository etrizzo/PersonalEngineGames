#include "Actor.hpp"
#include "ActorDefinition.hpp"
#include "Game.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Item.hpp"
#include "ItemDefinition.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"

Actor::Actor(ActorDefinition * definition, Map * entityMap, Vector2 initialPos, float initialRotation)
	:Entity((EntityDefinition*)definition, entityMap, initialPos, initialRotation)
{
	//m_renderable->SetMaterial(Material::GetMaterial("cutout"));
	m_definition = definition;
	m_faction = definition->m_startingFaction;
	m_timeLastUpdatedDirection = GetRandomFloatInRange(-1.f,1.f);
	m_lastAttacked = 0.f;
	m_facing = Vector2::MakeDirectionAtDegrees(GetRandomFloatInRange(-180.f,180.f));
	GetRandomStatsFromDefinition();
	//m_animSets = std::vector<SpriteAnimSet*>();
	//m_animSets.resize(NUM_RENDER_SLOTS);
	for (int i = BODY_SLOT; i < NUM_RENDER_SLOTS; i++){
		if (m_definition->m_layerTextures[i] != nullptr){
			//m_animSets[i] = new SpriteAnimSet(m_definition->m_spriteSetDefs[i]);
			//m_renderable->SetDiffuseTexture(m_animSets[i]->GetCurrentTexture(), i);
			m_renderable->SetSubMesh(m_localDrawingBox, m_lastUVs, RGBA::WHITE, i);
			m_renderable->AddDiffuseTexture(m_definition->m_layerTextures[i], i);
		}
	}
	UpdateRenderable();
}

Actor::~Actor()
{
	

}

void Actor::Update(float deltaSeconds)
{
	Entity::Update(deltaSeconds);
	std::string animName = GetAnimName();
	if (m_isFiring){
		if (m_animSet->IsCurrentAnimFinished()){
			FireArrow();
		}
	} else {
		RunSimpleAI(deltaSeconds);
	}

}

void Actor::Render()
{
	//Entity::Render();
	Entity::RenderHealthBar();
	Entity::RenderName();
}

std::string Actor::GetAnimName()
{
	Vector2 dir = m_facing.GetNormalized();
	std::string direction = "South";
	if (DotProduct(dir, DIRECTION_NORTH) >= .75f){
		direction = "North";
	} else if (DotProduct(dir, DIRECTION_SOUTH) >=.75f){
		direction = "South";
	} else {
		if (dir.x > 0.f){
			direction = "East";
		} else {
			direction = "West";
		}
	}
	std::string action = "Idle";
	if (m_moving){
		//Vector2 dir = Vector2::MakeDirectionAtDegrees(controller->GetLeftThumbstickAngle()).GetNormalized();
		action = "Move";
	}
	if (m_isFiring){
		action = "Bow";
	}

	return action + direction;
}

void Actor::UpdateRenderable()
{
	AABB2 uvs = m_animSet->GetCurrentUVs();
	if (!(uvs == m_lastUVs)){		//every anim set should have the same UVs so this should be fine
		m_lastUVs = uvs;
		for (int i = BODY_SLOT; i  < NUM_RENDER_SLOTS; i++){
			if (m_definition->m_layerTextures[i] != nullptr){
				//Texture* baseTexture = m_animSets[i]->GetCurrentTexture();	//base, legs, torso, head (, weapon)
				m_renderable->SetSubMesh(m_localDrawingBox, uvs, RGBA::WHITE, i);

			}
		}
	}
}

void Actor::RunCorrectivePhysics()
{
	if (!m_noClipMode){
		RunWorldPhysics();
		RunEntityPhysics();
	}
}

void Actor::RunWorldPhysics()
{
	Vector2 pos = GetPosition();
	//Get Tile
	Tile* currentTile = m_map->TileAtFloat(pos);
	//Get Neighbors
	Tile neighbors[8];
	m_map->GetNeighbors(currentTile, neighbors);
	Vector2 tileCenter = currentTile->GetCenter();

	//If a neighbor is solid, check if overlapping



	if (!CanEnterTile(neighbors[0])){
		CheckTileForCollisions(neighbors[0], tileCenter + Vector2(-.5f,.5f));
	}
	if (!CanEnterTile(neighbors[1])){
		CheckTileForCollisions(neighbors[1], Vector2(pos.x, tileCenter.y + .5f));
	}
	if (!CanEnterTile(neighbors[2])){
		CheckTileForCollisions(neighbors[2], tileCenter + Vector2(.5f,.5f));
	}
	if (!CanEnterTile(neighbors[3])){
		CheckTileForCollisions(neighbors[3], Vector2(tileCenter.x - .5f, pos.y));
	}
	if (!CanEnterTile(neighbors[4])){
		CheckTileForCollisions(neighbors[4], Vector2(tileCenter.x + .5f, pos.y));
	}
	if (!CanEnterTile(neighbors[5])){
		CheckTileForCollisions(neighbors[5], tileCenter + Vector2(-.5f,-.5f));
	}
	if (!CanEnterTile(neighbors[6])){
		CheckTileForCollisions(neighbors[6], Vector2(pos.x, tileCenter.y - .5f));
	}
	if (!CanEnterTile(neighbors[7])){
		CheckTileForCollisions(neighbors[7], tileCenter + Vector2(.5f,-.5f));
	}
}

void Actor::RunEntityPhysics()
{
	for (int actorIndex = 0; actorIndex < (int) m_map->m_allActors.size(); actorIndex++){
		Actor* collidingActor = (Actor*) m_map->m_allActors[actorIndex];
		if (collidingActor->GetPosition() != GetPosition()){
			CheckActorForCollision(collidingActor);
		}
	}
}

void Actor::SetPosition(Vector2 newPos, Map * newMap)
{
	if (newMap != nullptr){
		m_map->RemoveActorFromMap(this);
		m_map = newMap;
		m_map->AddActorToMap(this);
	}
	Entity::SetPosition(newPos);
}

void Actor::EnterTile(Tile * tile)
{
	Entity::EnterTile(tile);
}

void Actor::TakeDamage(int dmg)
{
	int damageToTake = dmg - m_stats.GetStat(STAT_DEFENSE);
	if (damageToTake < 0){
		damageToTake = 0;
	}
	m_health -= damageToTake;
	if (m_health <=0){
		m_aboutToBeDeleted = true;
	}
}

void Actor::EquipOrUnequipItem(Item * itemToEquip)
{
	EQUIPMENT_SLOT slotToEquipIn = itemToEquip->m_definition->m_equipSlot;
	if (slotToEquipIn != NOT_EQUIPPABLE){
		if (itemToEquip->m_currentlyEquipped){		//un-equip item
			m_equippedItems[slotToEquipIn] = nullptr;
			itemToEquip->m_currentlyEquipped = false;
		} else {
			if (m_equippedItems[slotToEquipIn] == nullptr){		//equip to empty slot	
				m_equippedItems[slotToEquipIn] = itemToEquip;
			} else {
				m_equippedItems[slotToEquipIn]->m_currentlyEquipped = false;		//un-equip item in slot, and equip selected item
				m_equippedItems[slotToEquipIn] = itemToEquip;
			}
			itemToEquip->m_currentlyEquipped = true;
		}
		UpdateStats();
	}
}

//void Actor::RunEntityPhysics()
//{
//	for (int actorIndex = 0; actorIndex < (int) m_map->m_allActors.size(); actorIndex++){
//		Actor* collidingActor = (Actor*) m_map->m_allActors[actorIndex];
//		CheckActorForCollision(collidingActor);
//	}
//}


void Actor::StartFiringArrow()
{
	if (m_ageInSeconds - m_lastAttacked > 2.f){
		if (!m_isFiring){
			m_isFiring = true;
		}
	}
}
std::string Actor::GetEquipSlotByID(EQUIPMENT_SLOT equipID)
{
	if (equipID == EQUIP_SLOT_HEAD){
		return "Head";
	}
	if (equipID == EQUIP_SLOT_CHEST){
		return "Chest";
	}
	if (equipID == EQUIP_SLOT_LEGS){
		return "Legs";
	}
	if (equipID == EQUIP_SLOT_WEAPON){
		return "Weapon";
	}
	if (equipID == NOT_EQUIPPABLE){
		return "Not Equippable";
	}
	//if (equipID == BODY_SLOT){
	//	return "Body";
	//}

	ERROR_AND_DIE("No equip slot for equipID");
}

void Actor::ParseLayersElement()
{
}

void Actor::UpdateWithController(float deltaSeconds)
{
	if (g_primaryController->GetLeftThumbstickMagnitude() > 0){
		m_moving = true;
		float mag = g_primaryController->GetLeftThumbstickMagnitude();
		Vector2 direction = m_facing;
		Translate(direction * mag * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * .3f))); 
		//m_rotationDegrees = controller->GetLeftThumbstickAngle();
		m_rotationDegrees = 0.f;
		m_facing = Vector2::MakeDirectionAtDegrees(g_primaryController->GetLeftThumbstickAngle()).GetNormalized() * .5f;
	} else {
		Vector2 arrowDirections = Vector2(0.f,0.f);
		if (g_theInput->IsKeyDown(VK_LEFT)){
			arrowDirections += Vector2(-.5f,0.f);
		}
		if (g_theInput->IsKeyDown(VK_RIGHT)){
			arrowDirections += Vector2(.5f,0.f);
		}
		if (g_theInput->IsKeyDown(VK_DOWN)){
			arrowDirections += Vector2(0.f, -.5f);
		}
		if (g_theInput->IsKeyDown(VK_UP)){
			arrowDirections += Vector2(0.f, .5f);
		}

		if (arrowDirections != Vector2(0.f,0.f)){
			m_moving = true;
			Translate(arrowDirections * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * .3f))); 
			m_rotationDegrees = 0.f;
			m_facing = arrowDirections;
		} else {
			m_moving = false;
		}

	}
}

void Actor::RunSimpleAI(float deltaSeconds)
{
	if (g_theGame->m_player == nullptr){
		Wander(deltaSeconds);
	} else {
		Vector2 playerPos = g_theGame->m_player->GetPosition();
		//RaycastResult2D raycast = m_map->Raycast(m_position, playerPos - m_position, m_definition->m_range);
		//Vector2 hitPos = raycast.m_impactPosition;
		//if the player is visible, update target
		if (g_theGame->m_player->m_dead){
			Wander(deltaSeconds);
		} else if (m_map->HasLineOfSight(GetPosition(), playerPos, m_definition->m_range)){
			//pursue target
			Vector2 distance = playerPos - GetPosition();
			m_facing = distance.GetNormalized();
			if (distance.GetLengthSquared() > 4){
				m_moving = true;
				Translate(m_facing * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * .3f))); 
			} else {
				m_moving = false;
			}
			StartFiringArrow();
		} else {
			Wander(deltaSeconds);
		}
		//m_facing = Vector2::MakeDirectionAtDegrees(m_rotationDegrees).GetNormalized() * .3f;
	}
}
void Actor::Wander(float deltaSeconds)
{
	float diff = m_ageInSeconds - m_timeLastUpdatedDirection;
	if (diff < 5.f ){
		Translate(m_facing * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * .3f))); 
	}
	if (diff > 5.f && diff < 10.5f){
		if (m_moving){
			m_moving = false;
		}
	}
	if (diff > 10.5f){
		m_moving = true;
		//m_facing = Vector2::MakeDirectionAtDegrees(GetRandomFloatInRange(-180.f,180.f));
		m_facing = m_facing - (2 * m_facing);
		m_timeLastUpdatedDirection = m_ageInSeconds;
	}
}

void Actor::FireArrow()
{

	
	m_isFiring = false;
	Vector2 facingScaled = m_facing * .5f;
	m_map->SpawnNewProjectile("Arrow", GetPosition() + facingScaled, facingScaled.GetOrientationDegrees(), m_faction, m_stats.GetStat(STAT_STRENGTH));
	m_lastAttacked = m_ageInSeconds;
}

void Actor::CheckActorForCollision(Actor * actor)
{
	Vector2 pos = GetPosition();
	Vector2 actorPos = actor->GetPosition();
	if (DoDiscsOverlap(m_physicsDisc, actor->m_physicsDisc)){
		Vector2 distance = pos - actorPos;
		Vector2 midPoint = pos - (distance * .5f);
		Vector2 myPushTo = m_physicsDisc.PushOutOfPoint(midPoint);
		Vector2 entityPushTo = actor->m_physicsDisc.PushOutOfPoint(midPoint);
		
		SetPosition(myPushTo);
		actor->SetPosition(entityPushTo);
		distance = pos - actorPos;
	}
}

void Actor::GetRandomStatsFromDefinition()
{
	m_stats = Stats();
	for (int i = 0; i < NUM_STAT_IDS; i++){
		STAT_ID statID = (STAT_ID) i;
		int randomStatValue = GetRandomIntInRange(m_definition->m_minStats.GetStat(statID), m_definition->m_maxStats.GetStat(statID));
		m_stats.SetStat(statID, randomStatValue);
		m_baseStats.SetStat(statID, randomStatValue);
	}
}

void Actor::UpdateStats()
{
	m_stats = Stats(m_baseStats);
	for (int slotNum = 0; slotNum < NUM_EQUIP_SLOTS; slotNum++){
		Item* equippedItem = m_equippedItems[slotNum];
		if (equippedItem != nullptr){
			m_stats.Add(equippedItem->m_stats);
		}
	}
}

