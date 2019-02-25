#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Adventure.hpp"
#include "Game/DebugRenderSystem.hpp"

Player::Player(ActorDefinition* actorDef, Vector2 initialPos, Map* map)
	: Actor(actorDef, map, initialPos)
{
	m_dead = false;

}

void Player::Update(float deltaSeconds)
{
	if (!m_dead){

		m_ageInSeconds+=deltaSeconds;

		Tile* newTile = m_map->TileAtFloat(GetPosition());
		if (newTile != m_currentTile){
			EnterTile(newTile);
		}
		//Entity::UpdateSpeed();

		std::string animName = GetAnimName();
		m_animSet->SetCurrentAnim(animName);		//sets IF it's different from the last frame
		//update anim set - scale speed of anim if you've got movement stat
		if (m_stats.GetStat(STAT_MOVEMENT) > 4){
			m_animSet->Update(deltaSeconds * ((float) m_stats.GetStat(STAT_MOVEMENT) * .25f));
		} else {
			m_animSet->Update(deltaSeconds);
		}

		UpdateRenderable();


		if (m_isFiring){
			if (m_animSet->IsCurrentAnimFinished()){
				FireArrow();
			}
		}
		//g_theGame->m_debugRenderSystem->MakeDebugRenderSphere(0.f, Vector3(GetPosition(), 0.f), m_definition->m_drawingRadius);
		//g_theGame->m_debugRenderSystem->MakeDebugRender2DQuad(0.f, m_localDrawingBox);
		if (g_theGame->m_devMode){
			g_theGame->m_debugRenderSystem->MakeDebugRenderCircle(0.f, m_physicsDisc, true , DEBUG_RENDER_IGNORE_DEPTH, RGBA::MAGENTA, RGBA::MAGENTA);
			g_theGame->m_debugRenderSystem->MakeDebugRenderCircle(0.f, GetPosition() + m_localDrawingBox.GetCenter(), m_localDrawingBox.GetWidth() * .5f, true, DEBUG_RENDER_IGNORE_DEPTH, RGBA::CYAN);
			g_theGame->m_debugRenderSystem->MakeDebugRenderCircle(0.f, GetPosition() + m_localDrawingBox.GetCenter(), m_localDrawingBox.GetHeight() * .5f, true, DEBUG_RENDER_IGNORE_DEPTH, RGBA::YELLOW);
		}
		//g_theGame->m_debugRenderSystem->MakeDebugRender2DQuad(0.)
	}
	
}

void Player::Render()
{
	if (!m_dead){
		Entity::Render();
		if (g_theGame->m_devMode){
			RenderDistanceMap();
		}
		//Entity::RenderHealthBar();
	} else {
		if (g_theGame->m_devMode){
			Entity::RenderDevMode();
			RenderDistanceMap();
		}
	}
}

void Player::RenderStatsInBox(AABB2 boxToDrawIn, RGBA tint)
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

void Player::RenderDistanceMap() const
{
	for (int i = 0; i < m_map->m_numTiles; i++){
		Tile tile = m_map->m_tiles[i];
		AABB2 tileBox = tile.GetBounds();
		float heatVal = m_distanceMap.GetHeat(i);
		g_theRenderer->DrawTextInBox2D(std::to_string(heatVal), tileBox, Vector2 (.5f,.5f), .1f, TEXT_DRAW_SHRINK_TO_FIT);
	}
}

void Player::HandleInput()
{
	if (!m_isFiring){
		if (!m_map->IsDialogueOpen()){
			Actor::UpdateWithController(g_theGame->GetDeltaSeconds());
		}

		if (g_theInput->WasMouseButtonJustPressed(MOUSE_BUTTON_LEFT)){
			SpeakToActor();
		}

	}
	m_physicsDisc.center=GetPosition();
}

void Player::SetPosition(Vector2 newPos, Map * newMap)
{
	Actor::SetPosition(newPos, newMap);
	if (newMap != nullptr){
		g_theGame->SetCurrentMap(newMap);
	}
}

void Player::EnterTile(Tile * tile)
{
	Actor::EnterTile(tile);
	UpdateDistanceMap();

}

void Player::TakeDamage(int dmg)
{
	if (!m_godMode){
		m_health-=dmg;
		if (m_health <= 0){
			m_dead = true;
			//g_theGame->StartStateTransition(STATE_DEFEAT, .1f, RGBA(0,0,0,200));
		}
	}
}

void Player::Respawn()
{
	m_dead = false;
	m_health = m_definition->m_startingHealth;
	//g_theGame->StartStateTransition(STATE_PLAYING, .1f, RGBA(0,0,0,200));
}

std::string Player::GetAnimName()
{
	//XboxController* controller = g_theInput->GetController(0);
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



void Player::SpeakToActor()
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

void Player::UpdateDistanceMap()
{
	m_distanceMap = Heatmap(m_map->m_dimensions, 9999.f);
	m_distanceMap.SetHeat(m_currentTile->m_coordinates, 0.f);
	m_map->FillSeededDistanceMap(m_distanceMap, nullptr, 50.f, 0.f);
}

void Player::FireArrow()
{
	m_isFiring = false;
	Vector2 closestAngle = GetClosestAngle() * .5f;
	m_map->SpawnNewProjectile("Arrow", GetPosition() + closestAngle, closestAngle.GetOrientationDegrees(), m_faction, m_stats.GetStat(STAT_STRENGTH));
}

Vector2 Player::GetClosestAngle()
{
	float maxDot = checkDot(STEP_NORTH);
	IntVector2 directionToPoint = STEP_NORTH;
	if (checkDot(STEP_EAST)> maxDot){
		maxDot =checkDot(STEP_EAST);
		directionToPoint = STEP_EAST;
	}
	if (checkDot(STEP_SOUTH)> maxDot){
		maxDot =checkDot(STEP_SOUTH);
		directionToPoint = STEP_SOUTH;
	}
	if (checkDot(STEP_WEST)> maxDot){
		maxDot =checkDot(STEP_WEST);
		directionToPoint = STEP_WEST;
	}

	XboxController* controller = g_theInput->GetController(0);
	if (controller->GetLeftThumbstickMagnitude() > 0){
		if (checkDot(STEP_NORTHEAST)> maxDot){
			maxDot =checkDot(STEP_NORTHEAST);
			directionToPoint = STEP_NORTHEAST;
		}
		if (checkDot(STEP_NORTHWEST)> maxDot){
			maxDot =checkDot(STEP_NORTHWEST);
			directionToPoint = STEP_NORTHWEST;
		}
		if (checkDot(STEP_SOUTHWEST)> maxDot){
			maxDot =checkDot(STEP_SOUTHWEST);
			directionToPoint = STEP_SOUTHWEST;
		}
		if (checkDot(STEP_SOUTHEAST)> maxDot){
			maxDot =checkDot(STEP_SOUTHEAST);
			directionToPoint = STEP_SOUTHEAST;
		}
	}
	return directionToPoint.GetVector2();
}

float Player::checkDot(IntVector2 direction)
{
	float dot = DotProduct(m_facing, direction.GetVector2().GetNormalized());
	return dot;
}
