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
	if (m_tileDef->m_isTerrain){
		m_extraInfo->m_terrainDef = m_tileDef;
	}
	m_extraInfo->SetSpriteCoords(m_tileDef->GetTexCoords(m_extraInfo->m_variant), SPRITE_COSMETIC_BASE);
	if (m_tileDef->m_overlayCoords != IntVector2(0,0)){
		m_extraInfo->SetSpriteCoords(m_tileDef->GetOverlayTexCoords(), SPRITE_OVERLAY);
	}
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
		if (m_tileDef->m_isTerrain){
			m_extraInfo->m_terrainDef = m_tileDef;
		}
	}
	m_extraInfo->SetSpriteCoords(m_tileDef->GetTexCoords(m_extraInfo->m_variant), SPRITE_COSMETIC_BASE);
	if (m_tileDef->m_overlayCoords != IntVector2(0,0)){
		m_extraInfo->SetSpriteCoords(m_tileDef->GetOverlayTexCoords(), SPRITE_OVERLAY);
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

bool Tile::HasTerrainDefinition(TileDefinition * def) const
{
	return m_extraInfo->m_terrainDef == def;
}

int Tile::GetTerrainLevel() const
{
	//if (m_extrainfo->m_cosmeticbasedef != nullptr){
	//	return m_extrainfo->m_cosmeticbasedef->m_terrainlevel;
	//}
	if (m_extraInfo->m_terrainDef != nullptr){
		return m_extraInfo->m_terrainDef->m_terrainLevel;
	} else {
		return -1;
	}
}

eTerrainLayer Tile::GetCosmeticTerrainLayer() const
{
	if (m_extraInfo->m_cosmeticBaseDefinition != nullptr){
		return m_extraInfo->m_cosmeticBaseDefinition->m_terrainLayer;
	}
	if (m_extraInfo->m_terrainDef != nullptr){
		return m_extraInfo->m_terrainDef->m_terrainLayer;
	} else {
		ConsolePrintf("Not terrain");
		return NOT_TERRAIN;
	}
}

eTerrainLayer Tile::GetTerrainLayer() const
{
	//if (m_extraInfo->m_cosmeticBaseDefinition != nullptr){
	//	return m_extraInfo->m_cosmeticBaseDefinition->m_terrainLayer;
	//}
	if (m_extraInfo->m_terrainDef != nullptr){
		return m_extraInfo->m_terrainDef->m_terrainLayer;
	} else {
		ConsolePrintf("Not terrain");
		return NOT_TERRAIN;
	}
}

eGroundLayer Tile::GetGroundLayer() const
{
	if (m_extraInfo->m_cosmeticBaseDefinition != nullptr && m_extraInfo->m_cosmeticBaseDefinition != m_tileDef){
		return m_extraInfo->m_cosmeticBaseDefinition->m_groundLayer;
	}
	//return m_tileDef->m_groundLayer;
	if (m_extraInfo->m_terrainDef != nullptr){
		return m_extraInfo->m_terrainDef->m_groundLayer;
	} else {
		ConsolePrintf("Not terrain");
		return NOT_GROUND;
	}
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

int Tile::GetWeight() const
{
	return m_extraInfo->m_weight;
}

void Tile::SetWeight(int weight)
{
	m_extraInfo->m_weight = weight;
}

void Tile::RenderTag()
{
	//if (m_extraInfo->m_tags.GetNumTags() > 0){
	//	//std::string tags = m_extraInfo->m_tags.GetTagsAsString();
	//	std::string tags = "";
	//	if (m_extraInfo->m_terrainDef != nullptr){
	//		tags = m_extraInfo->m_terrainDef->m_name;
	//	}

	//	g_theGame->m_debugRenderSystem->MakeDebugRender3DText(tags, 0.f, Vector3(GetCenter()), .1f);
	//	g_theRenderer->DrawTextInBox2D(tags, GetBounds(), Vector2::HALF, .01f, TEXT_DRAW_WORD_WRAP);
	//}

	if (m_extraInfo->m_terrainDef != nullptr){
		std::string terrainstuff = "";
		terrainstuff += ("Base:\n" + m_extraInfo->m_terrainDef->m_name + "\nCosmetic:\n" );
		if (m_extraInfo->m_cosmeticBaseDefinition != nullptr){
			terrainstuff += ( m_extraInfo->m_cosmeticBaseDefinition->m_name);
		}
		//terrainstuff += "\nDef: " + std::to_string(GetTerrainLevel());
		g_theGame->m_debugRenderSystem->MakeDebugRender3DText(terrainstuff, 0.f, Vector3(GetCenter()), .1f);
		g_theRenderer->DrawTextInBox2D(terrainstuff, GetBounds(), Vector2::HALF, .01f, TEXT_DRAW_WORD_WRAP);
	}
}

void Tile::SetType(TileDefinition* newType)
{
	m_tileDef = newType;
	m_extraInfo->m_variant = GetRandomIntLessThan(newType->m_spriteCoords.size());

	m_extraInfo->m_terrainDef = newType;
	
	//if (newType->m_isTerrain){
	//	m_extraInfo->m_terrainDef = newType;
	//} else {
	//	m_extraInfo->m_terrainDef = nullptr;
	//}

	m_extraInfo->SetSpriteCoords(m_tileDef->GetTexCoords(m_extraInfo->m_variant), SPRITE_COSMETIC_BASE);
	if (m_tileDef->m_overlayCoords != IntVector2(0,0)){
		m_extraInfo->SetSpriteCoords(m_tileDef->GetOverlayTexCoords(), SPRITE_OVERLAY);
	}
}

void Tile::AddOverlaySpriteFromTileSheet(AABB2 spriteCoords, eTileSpriteLayer layer)
{
	m_extraInfo->SetSpriteCoords(spriteCoords, layer);
}

TileExtraInfo::TileExtraInfo()
{
	for(unsigned int i = 0; i < NUM_SPRITE_LAYERS; i++){
		m_spriteCoords.push_back(nullptr);
	}
}

void TileExtraInfo::SetSpriteCoords(const AABB2 & coords, eTileSpriteLayer layer)
{
	if (m_spriteCoords[layer] != nullptr){
		delete m_spriteCoords[layer];
		m_spriteCoords[layer] = nullptr;
	}
	m_spriteCoords[layer] = new AABB2(coords);
}

void TileExtraInfo::AddTag(std::string tag)
{
	m_tags.SetTag(tag);
}

void TileExtraInfo::RemoveTag(std::string tag)
{
	m_tags.RemoveTag(tag);
}
