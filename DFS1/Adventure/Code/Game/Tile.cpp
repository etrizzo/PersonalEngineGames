#include "Tile.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/DebugRenderSystem.hpp"


Tile::~Tile()
{
	
}

Tile::Tile(int x, int y, TileDefinition* tileDef)
{
	m_coordinates = IntVector2(x,y);
	m_tileDef = tileDef;
	m_extraInfo = new TileExtraInfo();
	m_extraInfo->m_variant = GetRandomIntLessThan(tileDef->m_spriteCoords.size());
}

Tile::Tile(IntVector2 & coords, TileDefinition* tileDef)
{
	m_coordinates = coords;
	m_tileDef = tileDef;
	m_extraInfo = new TileExtraInfo();
	if (tileDef == nullptr){
		m_extraInfo->m_variant = 0;
	} else {
		m_extraInfo->m_variant = GetRandomIntLessThan(tileDef->m_spriteCoords.size());
	}
}


//	returns neighbors of the current tile x:
// 0 1 2
// 3 x 4
// 5 6 7

void Tile::AddTag(std::string tag)
{
	m_extraInfo->AddTag(tag);
}

bool Tile::HasTag(std::string tag)
{
	return m_extraInfo->m_tags.HasTag(tag);
}

bool Tile::HasBeenSpawnedOn() const
{
	return m_extraInfo->m_spawnedOn;
}

void Tile::MarkAsSpawned()
{
	m_extraInfo->m_spawnedOn = true;
}

//for destructible tiles, if i'm #feelinthat
void Tile::DamageTile(TileDefinition* typeIfDestroyed, int damage)
{
	damage;
	typeIfDestroyed;
	/*if (m_tileDef->m_isDestructible){
		AABB2 oldtexCoords = m_tileDef->GetTexCoordsAtHealth(m_health);
		m_health -=damage;
		AABB2 newtexCoords = m_tileDef->GetTexCoordsAtHealth(m_health);
		
		if (m_health <= 0){
			m_tileDef = typeIfDestroyed;
		}

	}*/
}

AABB2 Tile::GetBounds() const
{
	IntVector2 maxs = m_coordinates + IntVector2(TILE_WIDTH, TILE_WIDTH);
	return AABB2(m_coordinates.GetVector2(), maxs.GetVector2());
}

Vector2 Tile::GetCenter() const
{
	return (m_coordinates.GetVector2() + Vector2(TILE_WIDTH * .5f, TILE_WIDTH * .5f));
}

Vector2 Tile::GetApproximateCenter() const
{
	return (GetCenter() + Vector2(GetRandomFloatInRange(TILE_WIDTH * -.001f, TILE_WIDTH *.001f), GetRandomFloatInRange(TILE_WIDTH *-.001f, TILE_WIDTH *.001f)));
}

TileDefinition* Tile::GetTileDefinition()
{
	return m_tileDef;
}

void Tile::RenderTag()
{
	if (m_extraInfo->m_tags.GetNumTags() > 0){
		std::string tags = m_extraInfo->m_tags.GetTagsAsString();

		g_theGame->m_debugRenderSystem->MakeDebugRender3DText(tags, 0.f, Vector3(GetCenter()), .1f);
		g_theRenderer->DrawTextInBox2D(tags, GetBounds(), Vector2::HALF, .01f, TEXT_DRAW_WORD_WRAP);
	}
}

void Tile::SetType(TileDefinition* newType)
{
	m_tileDef = newType;
	m_extraInfo->m_variant = GetRandomIntLessThan(newType->m_spriteCoords.size());
}

void TileExtraInfo::AddTag(std::string tag)
{
	m_tags.SetTag(tag);
}

void TileExtraInfo::RemoveTag(std::string tag)
{
	m_tags.RemoveTag(tag);
}
