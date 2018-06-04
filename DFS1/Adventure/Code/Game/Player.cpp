#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Adventure.hpp"

Player::Player(ActorDefinition* actorDef, Vector2 initialPos, Map* map)
	: Actor(actorDef, map, initialPos)
{
	m_dead = false;
}

void Player::Update(float deltaSeconds)
{
	if (!m_dead){
		if (!m_isFiring){
			Actor::UpdateWithController(deltaSeconds);
		}
		m_physicsDisc.center=GetPosition();
		m_ageInSeconds+=deltaSeconds;

		Tile* newTile = m_map->TileAtFloat(GetPosition());
		if (newTile != m_currentTile){
			EnterTile(newTile);
		}
		//Entity::UpdateSpeed();

		std::string animName = GetAnimName();
		m_animSet->SetCurrentAnim(animName);		//sets IF it's different from the last frame
		m_animSet->Update(deltaSeconds);
		
		if (g_theGame->m_currentState == STATE_PLAYING){
			if (g_primaryController != nullptr){
				if ((g_primaryController->WasButtonJustPressed(XBOX_A) || g_theInput->WasKeyJustPressed(VK_SPACE)) && !m_isFiring){
					m_isFiring = true;
				}
			}

			if (m_isFiring){
				if (m_animSet->IsCurrentAnimFinished()){
					FireArrow();
				}
			}
		}
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

	AABB2 pictureBox = AABB2(boxToDrawIn.mins + padding, Vector2(boxToDrawIn.maxs.x - (widthOfBox*.5f), boxToDrawIn.maxs.y - fontSize) - padding);
	pictureBox.AddPaddingToSides(-.1f, -.15f);
	g_theRenderer->DrawAABB2Outline(pictureBox, RGBA(255,255,255,64));
	pictureBox.AddPaddingToSides(-.1f, -.1f);
	const Texture* entityTexture = m_animSet->GetTextureForAnim("IdleSouth");
	AABB2 texCoords = m_animSet->GetUVsForAnim("IdleSouth", 0.f);
	g_theRenderer->DrawTexturedAABB2(pictureBox, *entityTexture,texCoords.mins, texCoords.maxs, RGBA());

	AABB2 statsBox = pictureBox;
	statsBox.Translate((widthOfBox * .5f), 0.f);
	statsBox.AddPaddingToSides(.1f,.1f);
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

void Player::SetPosition(Vector2 newPos, Map * newMap)
{
	Actor::SetPosition(newPos, newMap);
	if (newMap != nullptr){
		g_theGame->m_currentAdventure->SetCurrentMap(newMap);
	}
}

void Player::EnterTile(Tile * tile)
{
	Actor::EnterTile(tile);
	UpdateDistanceMap();

}

void Player::TakeDamage(int dmg)
{
	m_health-=dmg;
	if (m_health <= 0){
		m_dead = true;
		g_theGame->StartStateTransition(STATE_DEFEAT, .1f, RGBA(0,0,0,200));
	}
}

void Player::Respawn()
{
	m_dead = false;
	m_health = m_definition->m_startingHealth;
	g_theGame->StartStateTransition(STATE_PLAYING, .1f, RGBA(0,0,0,200));
}

std::string Player::GetAnimName()
{
	XboxController* controller = g_theInput->GetController(0);
	Vector2 dir = m_facing.GetNormalized();
	std::string direction = "South";
	if (DotProduct(dir, DIRECTION_NORTH) >= .7f){
		direction = "North";
	} else if (DotProduct(dir, DIRECTION_SOUTH) >=.7f){
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
		action = "Attack";
	}

	return action + direction;

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
