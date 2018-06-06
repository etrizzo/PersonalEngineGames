#include "Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Tile.hpp"
#include "Game/Map.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/Item.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"

Entity::~Entity()
{
	for (Item* item : m_inventory){
		delete item;
	}
	//delete (m_animSet);
}

Entity::Entity()
{
	m_renderable = new Renderable2D();
	m_renderable->SetMaterial(Material::GetMaterial("default"));
}

Entity::Entity(EntityDefinition * entityDef, Map * entityMap, Vector2 initialPos, float initialRotation)
{
	
	m_definition = entityDef;
	m_renderable = new Renderable2D();
	m_renderable->SetMaterial(Material::GetMaterial("default"));
	m_renderable->m_zOrder = 1;
	SetPosition(initialPos);
	m_rotationDegrees = initialRotation;

	m_ageInSeconds = 0.f;
	//m_texturePath = texture;
	m_aboutToBeDeleted = false;
	m_map = entityMap;
	m_health = m_definition->m_startingHealth;

	m_animSet = new SpriteAnimSet(m_definition->m_spriteAnimSetDef);

	//create disc2 for physics
	m_physicsDisc = Disc2(initialPos, m_definition->m_physicsRadius);

	//set up the drawing box on top of the physics disc
	if (m_definition->m_drawWithBounds){
		m_localDrawingBox = m_definition->m_boundingBox;
	} else {
		Vector2 mins = Vector2(-1.f - (.5f * m_definition->m_drawingRadius), -1.f);
		Vector2 maxs = Vector2(+1.f + (.5f * m_definition->m_drawingRadius), 1.2f + m_definition->m_drawingRadius);
		m_localDrawingBox = AABB2(mins, maxs);
	}

	if (entityDef->m_canFly){
		m_speed = entityDef->m_flySpeed;
	} else if (entityDef->m_canWalk){
		m_speed = entityDef->m_walkSpeed;
	} else if (entityDef->m_canSwim){
		m_speed = entityDef->m_swimSpeed;
	} else {
		m_speed = 0.f;
	}

	m_noClipMode = false;

	m_faction = "none";
	m_inventory = std::vector<Item*>();
	FillInventory();
	UpdateRenderable();
}



void Entity::Update(float deltaSeconds)
{
	//m_rotationDegrees+= (m_spinDegreesPerSecond*deltaSeconds);
	m_physicsDisc.center=GetPosition();
	m_ageInSeconds+=deltaSeconds;

	Tile* newTile = m_map->TileAtFloat(GetPosition());
	if (newTile != m_currentTile){
		EnterTile(newTile);
	}

	//UpdateSpeed();

	std::string animName = GetAnimName();
	m_animSet->SetCurrentAnim(animName);		//sets IF it's different from the last frame
	m_animSet->Update(deltaSeconds);

	UpdateRenderable();

}

void Entity::UpdateRenderable()
{
	Texture* entityTexture = m_animSet->GetCurrentTexture();
	AABB2 uvs = m_animSet->GetCurrentUVs();
	if (!(uvs == m_lastUVs)){
		m_lastUVs = uvs;
		//regenerate mesh
		//m_lastUVs = uvs;
		//MeshBuilder mb = MeshBuilder();
		//mb.Begin(PRIMITIVE_TRIANGLES, true);
		//mb.AppendPlane2D(m_localDrawingBox, RGBA::WHITE, uvs);
		//mb.End();

		//m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_3DPCU));
		m_renderable->SetMesh(m_localDrawingBox, uvs, RGBA::WHITE);
	}
	if (m_renderable->GetEditableMaterial()->m_textures[0] != entityTexture){
		m_renderable->SetDiffuseTexture(entityTexture);
	}
}

void Entity::Render()
{
	TODO("Clean up entity rendering pipeline");
	//g_theRenderer->PushAndTransform2(GetPosition(), m_rotationDegrees, m_definition->m_physicsRadius);
	//
	////Texture* entityTexture = g_theRenderer->CreateOrGetTexture(m_texturePath);
	//const Texture* entityTexture = m_animSet->GetCurrentTexture();
	//AABB2 texCoords = m_animSet->GetCurrentUVs();
	//g_theRenderer->DrawTexturedAABB2(m_localDrawingBox, *entityTexture,texCoords.mins, texCoords.maxs, RGBA());
	//g_theRenderer->Pop();

	RenderHealthBar();
	if (g_theGame->m_devMode){
		RenderDevMode();
	}
}

void Entity::RenderDevMode()
{
	TODO("Replace dev mode drawing with debug render system");
	Vector2 pos = GetPosition();
	//draw cosmetic radius
	g_theRenderer->PushAndTransform2(pos,m_rotationDegrees, m_definition->m_drawingRadius * .5f);
	RenderDevDisc(RGBA(255,0,255));
	g_theRenderer->Pop();

	g_theRenderer->PushAndTransform2(pos,m_rotationDegrees,  m_definition->m_physicsRadius);
	g_theRenderer->DrawAABB2Outline(m_localDrawingBox, RGBA(255,0,255,200));
	g_theRenderer->Pop();

	//draw physics radius
	g_theRenderer->PushAndTransform2(m_physicsDisc.center, 0.f, m_physicsDisc.radius);
	RenderDevDisc(RGBA(0,255,255));
	g_theRenderer->Pop();

	if (m_definition->m_forwardViewAngle != DEFAULT_FORWARD_VIEW_ANGLE){
		Vector2 leftTarget = pos + (Vector2::MakeDirectionAtDegrees(m_rotationDegrees + m_definition->m_forwardViewAngle) * m_definition->m_range);
		Vector2 rightTarget = pos + (Vector2::MakeDirectionAtDegrees(m_rotationDegrees - m_definition->m_forwardViewAngle) * m_definition->m_range);
		g_theRenderer->DrawDottedLine2D(pos, leftTarget, RGBA(0,255,0,64), .15f);
		g_theRenderer->DrawDottedLine2D(pos, rightTarget, RGBA(0,255,0, 64), .15f);
	}

}


void Entity::RenderDevDisc(RGBA color)
{
	g_theRenderer->DrawRegularPolygon2(40,Vector2(0,0), 1.f,0.f,color,true);
}

std::string Entity::GetAnimName()
{
	return (std::string) "Idle";
}


bool Entity::IsAboutToBeDeleted()
{
	return m_aboutToBeDeleted;
}

void Entity::RunCorrectivePhysics()
{
	if (!m_noClipMode){
		RunWorldPhysics();
		RunEntityPhysics();
	}
}

void Entity::RunWorldPhysics()
{
	//Get Tile
	//Tile* currentTile = m_map->TileAtFloat(m_position);
	//Get Neighbors
	Tile neighbors[8];
	m_map->GetNeighbors(m_currentTile, neighbors);
	Vector2 tileCenter = m_currentTile->GetCenter();

	//If a neighbor is solid, check if overlapping

	Vector2 pos = GetPosition();

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

void Entity::RunEntityPhysics()
{
}

void Entity::CheckTileForCollisions(Tile tile, Vector2 closestPoint)
{
	if (m_physicsDisc.IsPointInside(closestPoint)){
		//push out :)
		Vector2 pushTo = m_physicsDisc.PushOutOfPoint(closestPoint);
		SetPosition(pushTo);
	}
}

void Entity::EnterTile(Tile* tile)
{
	m_currentTile = tile;
	UpdateSpeed();
}

void Entity::RenderHealthBar()
{

	
	//if (percentFull < 1.f){
	Vector2 offset = Vector2(0.f, (m_localDrawingBox.maxs.y * m_physicsDisc.radius) - .1f);
	g_theRenderer->PushAndTransform2(GetPosition() + offset, 0.f, .4f);
	Vector2 mins = Vector2(-1.f, 0.f);
	Vector2 maxs = Vector2(1.f, .3f);
	AABB2 boundsBorder = AABB2(mins, maxs);
	RenderHealthInBox(boundsBorder);

	
	g_theRenderer->Pop();
}

void Entity::RenderHealthInBox(AABB2 healthBox)
{
	float percentFull = (float) m_health / (float) m_definition->m_maxHealth;
	float healthHeight = healthBox.GetHeight();
	g_theRenderer->DrawAABB2(healthBox, RGBA(0,0,0));	//draw black outline
	AABB2 boundsBackground = healthBox;
	boundsBackground.AddPaddingToSides(-.3f * healthHeight,-.3f * healthHeight);
	g_theRenderer->DrawAABB2(boundsBackground, RGBA(255,0,0));// draw red background;
	float healthWidth = boundsBackground.GetWidth();
	AABB2 boundsHealth = AABB2(boundsBackground.mins, Vector2(boundsBackground.mins.x + (percentFull * healthWidth), boundsBackground.maxs.y));
	g_theRenderer->DrawAABB2(boundsHealth, RGBA(0,255,0));
	//g_theRenderer->Pop2();
}

void Entity::RenderName()
{
	Vector2 offset = Vector2(0.f, (m_localDrawingBox.maxs.y * m_physicsDisc.radius) + .05f);
	//g_theRenderer->PushAndTransform2(GetPosition() + offset, 0.f, .4f);
	////g_theRenderer->DrawText2D(m_definition->m_name, Vector2(-1.f,0.f), .3f);
	//AABB2 nameBounds =  AABB2(-1.f,0.f,1.f,1.f);
	//g_theRenderer->DrawTextInBox2D(m_definition->m_name, nameBounds, Vector2(.5f,0.f), .3f, TEXT_DRAW_SHRINK_TO_FIT);
	g_theRenderer->DrawTextAsSprite(m_definition->m_name, Vector3(GetPosition() + offset));
	//g_theRenderer->Pop();
}



void Entity::SetPosition(Vector2 newPosition, Map* newMap)
{
	m_renderable->m_transform.SetLocalPosition(newPosition);
	m_physicsDisc.center = newPosition;
	if (newMap != nullptr){
		m_map = newMap;
	}
}

void Entity::SetRotation(float newRot)
{
	m_renderable->m_transform.SetRotationEuler(newRot);
}

void Entity::SetScale(float uniformScale)
{
	SetScale(Vector2(uniformScale, uniformScale));
}

void Entity::SetScale(Vector2 scale)
{
	m_renderable->m_transform.SetScale(scale);
}

void Entity::Rotate(float offset)
{
	m_renderable->m_transform.RotateByEuler(offset);
}

void Entity::Translate(Vector2 offset)
{
	m_renderable->m_transform.TranslateLocal(offset);
}

bool Entity::IsPointInForwardView(Vector2 point)
{
	return IsPointInConicSector2D(point, GetPosition(), m_facing, m_definition->m_forwardViewAngle, m_definition->m_range);
}

bool Entity::IsSameFaction(Entity * otherEntity) const
{
	if (m_faction == otherEntity->m_faction){
		return true;
	} else {
		return false;
	}
}

void Entity::AddItemToInventory(Item * itemToAdd)
{
	m_inventory.push_back(itemToAdd);
}

Vector2 Entity::GetPosition() const
{

	Vector3 pos =  m_renderable->m_transform.GetLocalPosition();
	return pos.XY();
}

float Entity::GetRotation() const
{
	return m_rotationDegrees;	//feeds transform
}

Renderable2D * Entity::GetRenderable() const
{
	return m_renderable;
}

Transform2D&  Entity::GetTransform() const
{
	return m_renderable->m_transform;
}

bool Entity::CanEnterTile(Tile tileToEnter)
{
	TileDefinition* tileDef = tileToEnter.GetTileDefinition();
	if (tileDef != nullptr){
		if (tileDef->m_allowsWalking && m_definition->m_canWalk){
			return true;
		}
		if (tileDef->m_allowsSwimming && m_definition->m_canSwim){
			return true;
		}
		if (tileDef->m_allowsFlying && m_definition->m_canFly){
			return true;
		}
	} else {
		return false;
	}
	return false;
}

void Entity::UpdateSpeed()
{
	Tile* currentTile = m_map->TileAtFloat(GetPosition());
	float defaultSpeed = 0.f;
	if (m_definition->m_canFly && currentTile->m_tileDef->m_allowsFlying){
		defaultSpeed = m_definition->m_flySpeed;
	} else if (m_definition->m_canWalk && currentTile->m_tileDef->m_allowsWalking){
		defaultSpeed = m_definition->m_walkSpeed;
	} else if (m_definition->m_canSwim && currentTile->m_tileDef->m_allowsSwimming){
		defaultSpeed = m_definition->m_swimSpeed;
	}
	//apply movement penalties by tile here
	m_speed = defaultSpeed;

}

//fill initial inventory based on actor definition
void Entity::FillInventory()
{
	for (unsigned int itemIndex = 0; itemIndex< m_definition->m_startingInventory.size(); itemIndex++){
		//check each possible item's chance for spawning
		if (CheckRandomChance(m_definition->m_inventoryProbabilities[itemIndex])){
			Item* newItem = new Item(m_definition->m_startingInventory[itemIndex], m_map, GetPosition());
			m_inventory.push_back(newItem);
		}
	}
}

//void Entity::CheckTile(Tile tileToEnter)
//{
//
//}


