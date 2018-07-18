#include "Actor.hpp"
#include "ActorDefinition.hpp"
#include "Game.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Item.hpp"
#include "ItemDefinition.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Game/ClothingSetDefinition.hpp"
#include "Game/Quest.hpp"
#include "Game/Party.hpp"

#define SPEED_RATIO (.15f)

Actor::Actor(ActorDefinition * definition, Map * entityMap, Vector2 initialPos, float initialRotation, int difficulty)
	:Entity((EntityDefinition*)definition, entityMap, initialPos, initialRotation)
{
	//m_renderable->SetMaterial(Material::GetMaterial("cutout"));
	m_definition = definition;
	m_faction = definition->m_startingFaction;
	m_timeLastUpdatedDirection = GetRandomFloatInRange(-1.f,1.f);
	m_lastAttacked = 0.f;
	m_defaultBehavior = m_definition->m_defaultBehavior;
	if (m_defaultBehavior == BEHAVIOR_NONE){
		m_facing = Vector2(0.f, -1.f);
	} else {
		m_facing = Vector2::MakeDirectionAtDegrees(GetRandomFloatInRange(-180.f,180.f));
	}



	GetRandomStatsFromDefinition();
	//m_animSets = std::vector<SpriteAnimSet*>();
	//m_animSets.resize(NUM_RENDER_SLOTS);
	//m_layerTextures = std::vector<Texture*>();
	int numTextures = BODY_SLOT;
	m_currentLook = m_definition->m_clothingSetDef->GetRandomSet();
	for (int i = BODY_SLOT; i < NUM_RENDER_SLOTS; i++){
		if (m_currentLook->GetTexture(i) != nullptr){
			m_renderable->SetSubMesh(m_localDrawingBox, m_lastUVs, m_currentLook->GetTint(i), numTextures);
			m_renderable->AddDiffuseTexture(m_currentLook->GetTexture(i), numTextures);
			numTextures++;
		}
	}
	
	m_health+= (difficulty * 5);
	Stats difficultyMod = Stats(IntRange(0, difficulty));
	m_stats.Add(difficultyMod);

	m_dialogue = new DialogueSet(m_definition->m_dialogueDefinition);

	m_healthRenderable = new Renderable2D();
	m_healthRenderable->SetMaterial(Material::GetMaterial("default"));
	//m_healthRenderable->m_transform.SetParent(&m_renderable->m_transform);
	UpdateHealthBar();
	UpdateRenderable();
}

Actor::~Actor()
{
	

}

void Actor::Update(float deltaSeconds)
{
	m_physicsDisc.center=GetPosition();
	m_ageInSeconds+=deltaSeconds;

	Tile* newTile = m_map->TileAtFloat(GetPosition());
	if (newTile != m_currentTile){
		EnterTile(newTile);
	}

	//UpdateSpeed();

	std::string animName = GetAnimName();
	m_animSet->SetCurrentAnim(animName);		//sets IF it's different from the last frame
	//update anim set - scale speed of anim if you've got movement stat
	if (m_stats.GetStat(STAT_MOVEMENT) > 4){
		m_animSet->Update(deltaSeconds * ((float) m_stats.GetStat(STAT_MOVEMENT) * .25f));
	} else {
		m_animSet->Update(deltaSeconds);
	}
	
	UpdateHealthBar();
	UpdateRenderable();
	if (g_theGame->m_devMode){
		g_theGame->m_debugRenderSystem->MakeDebugRenderCircle(0.f, m_physicsDisc, true , DEBUG_RENDER_IGNORE_DEPTH, RGBA::MAGENTA, RGBA::MAGENTA);
		g_theGame->m_debugRenderSystem->MakeDebugRenderCircle(0.f, GetPosition(), m_localDrawingBox.GetHeight() * .5f, true, DEBUG_RENDER_IGNORE_DEPTH, RGBA::YELLOW);
	}
	if (m_isFiring){
		if (m_animSet->IsCurrentAnimFinished()){
			FireArrow();
		}
	} else {
		if (!m_isPlayer){
			RunSimpleAI(deltaSeconds);
		}
	}
	if (m_faction == "evil"){
		EquipItemsInInventory();
	}

}

void Actor::Render()
{
	Entity::Render();
	//Entity::RenderHealthBar();
	//Entity::RenderName();
	//RenderDialogue();
}

void Actor::HandleInput()
{
	if (m_isPlayer && !m_isFiring){
		if (!m_map->IsDialogueOpen()){
			UpdateWithController(g_theGame->GetDeltaSeconds());
		}

		if (g_theInput->WasMouseButtonJustPressed(MOUSE_BUTTON_LEFT)){
			SpeakToOtherActor();
		}
		m_physicsDisc.center=GetPosition();
	}
	
}

void Actor::EquipItemsInInventory()
{
	for (Item* item : m_inventory){
		EquipOrUnequipItem(item);	
	}
}

void Actor::AssignAsQuestGiver(Quest * questToGive)
{
	m_questGiven = questToGive;
	m_dialogue = questToGive->GetCurrentDialogueSet();
}

void Actor::AdvanceQuest()
{
	m_questGiven->m_currentIndex++;
	m_dialogue = m_questGiven->GetCurrentDialogueSet();
}

void Actor::FinishQuest()
{
	m_dialogue = new DialogueSet(m_definition->m_dialogueDefinition);
}


void Actor::RenderStatsInBox(AABB2 boxToDrawIn, RGBA tint)
{
	//g_theRenderer->DrawAABB2Outline(boxToDrawIn, RGBA(255,0,0));
	float widthOfBox = boxToDrawIn.GetWidth();
	float heightOfBox = boxToDrawIn.GetHeight();
	float fontSize = heightOfBox * .05f;
	Vector2 padding = Vector2(fontSize * .2f, fontSize * .2f);
	AABB2 healthBox = AABB2(boxToDrawIn.maxs - Vector2(widthOfBox, fontSize), boxToDrawIn.maxs);
	healthBox.Translate(0.f, heightOfBox * -.03f);
	//g_theRenderer->DrawTextInBox2D("Player Stats", boxToDrawIn, Vector2(.5f, .97f), fontSize, TEXT_DRAW_SHRINK_TO_FIT, tint);
	g_theRenderer->DrawTextInBox2D("HP:" , healthBox, Vector2(0.05f,.5f), fontSize, TEXT_DRAW_SHRINK_TO_FIT, tint);
	healthBox.AddPaddingToSides(-fontSize * 4.f, 0.f);
	healthBox.Translate(fontSize *2.f, - fontSize * .1f);
	Entity::RenderHealthInBox(healthBox);

	//AABB2 pictureBox = AABB2(boxToDrawIn.mins + padding, Vector2(boxToDrawIn.maxs.x - (widthOfBox*.5f), boxToDrawIn.maxs.y - fontSize) - padding);
	AABB2 pictureBox = boxToDrawIn.GetPercentageBox(.05f, .15f, .45f, .72f);
	pictureBox.TrimToAspectRatio(GetAspectRatio());
	float height = pictureBox.GetHeight();
	//pictureBox.AddPaddingToSides(height * -.1f,height * -.15f);
	g_theRenderer->DrawAABB2Outline(pictureBox, RGBA(255,255,255,64));
	//pictureBox.AddPaddingToSides(height *-.1f, height *-.1f);
	AABB2 texCoords = m_animSet->GetUVsForAnim("IdleSouth", 0.f);
	for (int i = BODY_SLOT; i < NUM_RENDER_SLOTS; i++){
		if (m_currentLook->GetTexture(i) != nullptr){
			//const Texture* entityTexture = m_animSets[i]->GetTextureForAnim("IdleSouth");
			g_theRenderer->DrawTexturedAABB2(pictureBox, *m_currentLook->GetTexture(i), texCoords.mins, texCoords.maxs, m_currentLook->GetTint(i));
		}
	}

	AABB2 statsBox = pictureBox;
	statsBox.Translate((widthOfBox * .5f), 0.f);
	//statsBox.AddPaddingToSides(.1f,.1f);
	//g_theRenderer->DrawAABB2Outline(statsBox, RGBA(255,255,0));
	std::string statsString = "";
	for ( int statIDNum = 0; statIDNum < NUM_STAT_IDS; statIDNum++){
		STAT_ID statID = (STAT_ID)statIDNum;
		statsString = statsString + Stats::GetNameForStatID(statID) + " : " + std::to_string(m_stats.GetStat(statID)) + "\n\n\n";
	}
	g_theRenderer->DrawTextInBox2D(statsString, statsBox, Vector2(0.f, 0.5f), fontSize * 1.f, TEXT_DRAW_SHRINK_TO_FIT, tint);

}

void Actor::RenderBoyInBox(AABB2 boyBox, RGBA tint)
{
	boyBox.TrimToAspectRatio(GetAspectRatio());
	float height = boyBox.GetHeight();
	AABB2 texCoords = m_animSet->GetUVsForAnim("IdleSouth", 0.f);
	for (int i = BODY_SLOT; i < NUM_RENDER_SLOTS; i++){
		if (m_currentLook->GetTexture(i) != nullptr){
			//const Texture* entityTexture = m_animSets[i]->GetTextureForAnim("IdleSouth");
			g_theRenderer->DrawTexturedAABB2(boyBox, *m_currentLook->GetTexture(i), texCoords.mins, texCoords.maxs, m_currentLook->GetTint(i));
		}
	}
}

void Actor::RenderFaceInBox(AABB2 faceBox, RGBA tint)
{
	g_theRenderer->DrawAABB2Outline(faceBox, RGBA::WHITE);
	//faceBox.AddPaddingToSides(-.05f,-.05f);
	//faceBox.TrimToAspectRatio(GetAspectRatio());
	float height = faceBox.GetHeight();
	AABB2 texCoords = m_animSet->GetUVsForAnim("IdleSouth", 0.f);
	AABB2 faceTexCoords = texCoords.GetPercentageBox(.3f, .4f, .7f, .9f);
	//faceTexCoords.ExpandToAspectRatio(faceBox.GetAspect());
	for (int i = BODY_SLOT; i < NUM_RENDER_SLOTS; i++){
		if (m_currentLook->GetTexture(i) != nullptr){
			//const Texture* entityTexture = m_animSets[i]->GetTextureForAnim("IdleSouth");
			g_theRenderer->DrawTexturedAABB2(faceBox, *m_currentLook->GetTexture(i), faceTexCoords.mins, faceTexCoords.maxs, m_currentLook->GetTint(i));
		}
	}
}

void Actor::RenderEquippedWeaponInBox(AABB2 weaponBox, RGBA tint)
{
	Item* weapon = m_equippedItems[EQUIP_SLOT_WEAPON];
	weaponBox.TrimToSquare();
	if (weapon != nullptr){
		weapon->RenderImageInBox(weaponBox);
	}
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
	if (!(uvs == m_lastUVs) || m_changedClothes){		//every anim set should have the same UVs so this should be fine
		if (m_changedClothes){
			m_renderable->Clear();		//remake the renderable
		}
		m_lastUVs = uvs;
		int numTextures = BODY_SLOT;
		for (int i = BODY_SLOT; i  < NUM_RENDER_SLOTS; i++){
			if (m_currentLook->GetTexture(i) != nullptr){
				//Texture* baseTexture = m_animSets[i]->GetCurrentTexture();	//base, legs, torso, head (, weapon)
				m_renderable->SetSubMesh(m_localDrawingBox, uvs, m_currentLook->GetTint(i), numTextures);
				if (m_changedClothes){
					m_renderable->AddDiffuseTexture(m_currentLook->GetTexture(i), numTextures);
				}
				numTextures++;
			}
		}
		m_changedClothes = false;
	}

	//m_healthRenderable->Clear();
	m_healthRenderable->SetSubMesh(m_healthBox, AABB2::ZERO_TO_ONE, RGBA::BLACK, 0);
	m_healthRenderable->SetSubMesh(m_healthBarBG, AABB2::ZERO_TO_ONE, RGBA::RED, 1);
	m_healthRenderable->SetSubMesh(m_currentHealthBar, AABB2::ZERO_TO_ONE, RGBA::GREEN, 2);

}

void Actor::UpdateHealthBar()
{
	if (m_isPlayer || !IsSameFaction(g_theGame->m_party->GetPlayerCharacter())){
		Vector2 offset = Vector2(0.f, m_localDrawingBox.GetHeight() * .6f );
		Vector2 boxPos = offset + GetPosition();
		Vector2 mins = Vector2(-.5f, 0.f) + boxPos;
		Vector2 maxs = Vector2(.5f, .15f) + boxPos;
		m_healthBox = AABB2(mins, maxs);
		float percentFull = (float) m_health / (float) m_definition->m_maxHealth;
		float healthHeight = m_healthBox.GetHeight();
		//g_theRenderer->DrawAABB2(healthBox, RGBA(0,0,0));	//draw black outline
		m_healthBarBG = m_healthBox;
		m_healthBarBG.AddPaddingToSides(-.2f * healthHeight,-.2f * healthHeight);
		//g_theRenderer->DrawAABB2(m_healthBarBG, RGBA(255,0,0));// draw red background;
		float healthWidth = m_healthBarBG.GetWidth();
		m_currentHealthBar = AABB2(m_healthBarBG.mins, Vector2(m_healthBarBG.mins.x + (percentFull * healthWidth), m_healthBarBG.maxs.y));
		//g_theRenderer->DrawAABB2(boundsHealth, RGBA(0,255,0));
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
	if(currentTile != nullptr){
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
		if (m_map != nullptr){
			m_map->RemoveActorFromMap(this);
		}
		m_map = newMap;
		m_map->AddActorToMap(this);
	}
	Entity::SetPosition(newPos);
	if (m_isPlayer){
		if (newMap != nullptr){
			g_theGame->SetCurrentMap(newMap);
		}
	}
}

void Actor::CheckTargetStatus()
{
	if (m_followTarget != nullptr && m_followTarget->m_aboutToBeDeleted){
		m_followTarget = nullptr;
	}
	if (m_attackTarget != nullptr && m_attackTarget->m_aboutToBeDeleted){
		m_attackTarget = nullptr;
	}
}

void Actor::EnterTile(Tile * tile)
{
	Entity::EnterTile(tile);
}

void Actor::SetFollowTarget(Actor * actorToFollow)
{
	m_followTarget = actorToFollow;
	m_defaultBehavior = BEHAVIOR_FOLLOW;
}

void Actor::Speak()
{
	if (m_dialogue != nullptr){
		if (!m_map->IsDialogueOpen()){
			m_map->StartDialogue(m_dialogue);
		} else {
			bool finished = m_map->ProgressDialogueAndCheckFinish();
			if (finished){
				if (m_questGiven != nullptr){
					m_questGiven->SpeakToGiver();
				}
			}
		}
	}
}

void Actor::SpeakToOtherActor()
{
	Actor* closestActor = nullptr;
	float closestDistance = m_speakRadius;
	for (Actor* actor : m_map->m_allActors){
		if (actor != this){
			float dist = GetDistance(actor->GetPosition(), GetPosition());
			if ( dist < closestDistance){
				closestActor = actor;
				closestDistance = dist;
			}
		}
	}
	if (closestActor != nullptr){
		closestActor->Speak();
	}
}

void Actor::TakeDamage(int dmg)
{
	if (!m_godMode){
		int damageToTake = dmg - m_stats.GetStat(STAT_DEFENSE);
		if (damageToTake < 0){
			damageToTake = 0;
		}
		m_health -= damageToTake;
		if (m_health <=0){
			if (!m_isPlayer){
				m_aboutToBeDeleted = true;
			} 
			m_dead = true;
			if (m_isPlayer){
				m_map->m_scene->RemoveRenderable(m_renderable);
				m_map->m_scene->RemoveRenderable(m_healthRenderable);
			}
			
		}
	}
}

void Actor::EquipOrUnequipItem(Item * itemToEquip)
{
	EQUIPMENT_SLOT slotToEquipIn = itemToEquip->m_definition->m_equipSlot;
	if (slotToEquipIn != NOT_EQUIPPABLE){
		RENDER_SLOT texSlot =GetRenderSlotForEquipSlot(slotToEquipIn);
		//if the head item shows hair, add it to the hat slot instead
		if (texSlot == HAT_SLOT){
			if (!itemToEquip->ShowsHair()){
				m_currentLook->SetLayer(HEAD_SLOT, (ClothingLayer*) nullptr);
				//m_layerTextures[HAT_SLOT] = nullptr;	//remove the hat texture
			}
		}
		if (itemToEquip->m_currentlyEquipped){		//un-equip item
			m_equippedItems[slotToEquipIn] = nullptr;
			itemToEquip->m_currentlyEquipped = false;
			m_currentLook->SetDefaultLayer(texSlot);
			if (texSlot == HAT_SLOT){
				m_currentLook->SetDefaultLayer(HEAD_SLOT);		//reset hair to normal
			}
			//m_layerTextures[texSlot] = m_definition->m_layerTextures[texSlot];
			m_changedClothes = true;
		} else {		//equip item
			if (m_equippedItems[slotToEquipIn] == nullptr){		//equip to empty slot	
				m_equippedItems[slotToEquipIn] = itemToEquip;
			} else {
				m_equippedItems[slotToEquipIn]->m_currentlyEquipped = false;		//un-equip item in slot, and equip selected item
				m_equippedItems[slotToEquipIn] = itemToEquip;
			}
			itemToEquip->m_currentlyEquipped = true;
			m_currentLook->SetLayer(texSlot, itemToEquip->GetEquipLayer());
			//m_layerTextures[texSlot] = itemToEquip->GetEquipTexture();
			m_changedClothes = true;
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
	if (m_isPlayer || m_ageInSeconds - m_lastAttacked > 2.f){
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
		Translate(direction * mag * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * SPEED_RATIO))); 
		//m_rotationDegrees = controller->GetLeftThumbstickAngle();
		m_rotationDegrees = 0.f;
		m_facing = Vector2::MakeDirectionAtDegrees(g_primaryController->GetLeftThumbstickAngle()).GetNormalized() * .5f;
	} else {
		Vector2 arrowDirections = Vector2(0.f,0.f);
		if (IsLeftKeyDown()){
			arrowDirections += Vector2::WEST;
		}
		if (IsRightKeyDown()){
			arrowDirections += Vector2::EAST;
		}
		if (IsDownKeyDown()){
			arrowDirections += Vector2::SOUTH;
		}
		if (IsUpKeyDown()){
			arrowDirections += Vector2::NORTH;
		}

		if (arrowDirections != Vector2::ZERO){
			m_moving = true;
			Translate(arrowDirections * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * SPEED_RATIO))); 
			m_rotationDegrees = 0.f;
			m_facing = arrowDirections;
		} else {
			m_moving = false;
		}

	}

	/*if (g_theInput->WasMouseButtonJustPressed(MOUSE_BUTTON_LEFT)){
		SpeakToOtherActor();
	}*/
}

void Actor::RunSimpleAI(float deltaSeconds)
{
	UpdateBehavior();
	switch (m_currentBehavior){
	case (BEHAVIOR_WANDER):
		Wander(deltaSeconds);
		//g_theGame->m_debugRenderSystem->MakeDebugRender3DText("Wander", 0.f, Vector3(GetPosition()), .1f);
		break;
	case (BEHAVIOR_FOLLOW):
		FollowPlayer(deltaSeconds);
		//g_theGame->m_debugRenderSystem->MakeDebugRender3DText("Follow", 0.f, Vector3(GetPosition()), .1f);
		break;
	case (BEHAVIOR_ATTACK):
		AttackEnemyActor(deltaSeconds);
		//g_theGame->m_debugRenderSystem->MakeDebugRender3DText("Attack", 0.f, Vector3(GetPosition()), .1f);
		break;
	}

	//if (g_theGame->m_player == nullptr){
	//	Wander(deltaSeconds);
	//} else {
	//	Vector2 playerPos = g_theGame->m_player->GetPosition();
	//	//RaycastResult2D raycast = m_map->Raycast(m_position, playerPos - m_position, m_definition->m_range);
	//	//Vector2 hitPos = raycast.m_impactPosition;
	//	//if the player is visible, update target
	//	if (g_theGame->m_player->m_dead){
	//		Wander(deltaSeconds);
	//	} else if (m_map->HasLineOfSight(GetPosition(), playerPos, m_definition->m_range)){
	//		//pursue target
	//		Vector2 distance = playerPos - GetPosition();
	//		m_facing = distance.GetNormalized();
	//		if (distance.GetLengthSquared() > 4){
	//			m_moving = true;
	//			Translate(m_facing * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * .3f))); 
	//		} else {
	//			m_moving = false;
	//		}
	//		StartFiringArrow();
	//	} else {
	//		Wander(deltaSeconds);
	//	}
	//	//m_facing = Vector2::MakeDirectionAtDegrees(m_rotationDegrees).GetNormalized() * .3f;
	//}
}
void Actor::Wander(float deltaSeconds)
{
	float diff = m_ageInSeconds - m_timeLastUpdatedDirection;
	if (diff < 5.f ){
		Translate(m_facing * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * SPEED_RATIO))); 
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

void Actor::UpdateBehavior()
{
	if (m_definition->m_isAggressive){
		UpdateAttackTarget();
		if (m_attackTarget != nullptr){
			m_currentBehavior = BEHAVIOR_ATTACK;
		} else {
			m_currentBehavior = m_defaultBehavior;
		}
	} else {
		m_currentBehavior = m_defaultBehavior;
	}
}

void Actor::FollowPlayer(float deltaSeconds)
{

	if (m_followTarget != nullptr){
		Vector2 distance = m_followTarget->GetPosition() - GetPosition();
		m_facing = distance.GetNormalized();
		float lengthSquared = distance.GetLengthSquared();

		if (lengthSquared > 100.f){
			Vector2 newPos = m_followTarget->GetPosition() - (m_followTarget->m_facing.GetNormalized() * .2f);
			SetPosition(newPos);
		}

		if (lengthSquared > 3.f){
			if (m_moving){
				Translate(m_facing * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * SPEED_RATIO))); 
			}
			if (lengthSquared > 5.f){
				m_moving = true;
				//Translate(m_facing * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * .3f))); 
			}
		} else {
			m_moving = false;
		}
	}
}

void Actor::AttackEnemyActor(float deltaSeconds)
{
	if (m_attackTarget != nullptr){
		Vector2 distance = m_attackTarget->GetPosition() - GetPosition();
		m_facing = distance.GetNormalized();
		if (distance.GetLengthSquared() > 4){
			m_moving = true;
			Translate(m_facing * deltaSeconds * (m_speed + (m_stats.GetStat(STAT_MOVEMENT) * SPEED_RATIO))); 
		} else {
			m_moving = false;
		}
		StartFiringArrow();
		if (m_attackTarget->m_aboutToBeDeleted){
			m_attackTarget = nullptr;		//if the enemy was killed this frame, delete it
		}
	}
}

void Actor::UpdateAttackTarget()
{
	//if it has been long enough since your last update, loop through all actors on the map and find closest enemy
	if (g_theGame->m_gameClock->GetCurrentSeconds() > m_lastFoundTarget + m_targetUpdateRate){
		m_lastFoundTarget = g_theGame->m_gameClock->GetCurrentSeconds();
		Actor* closestEnemy  = nullptr;
		float minDistance = 100000;
		for (Actor* actor : m_map->m_allActors){
			if (actor->m_faction != m_faction){
				if (m_map->HasLineOfSight(GetPosition(), actor->GetPosition(), m_definition->m_range)){
					float distToActor = GetDistance(actor->GetPosition(), GetPosition());
					if (distToActor < m_definition->m_range && distToActor < minDistance ){
						closestEnemy = actor;
						minDistance = distToActor;
					}
				}
			}
		}
		m_attackTarget = closestEnemy;
	}
}

void Actor::RenderDialogue()
{
	if (m_dialogue != nullptr){
		m_dialogue->Render(g_theGame->m_dialogueBox);
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

