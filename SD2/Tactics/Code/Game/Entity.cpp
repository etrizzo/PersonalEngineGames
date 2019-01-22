#include "Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Block.hpp"
#include "Game/Map.hpp"

Entity::~Entity()
{
}

Entity::Entity()
{
}

Entity::Entity(IntVector2 initialPos, Map* entityMap, std::string texture)
{

	

	m_ageInSeconds = 0.f;
	m_texturePath = texture;
	m_aboutToBeDeleted = false;
	m_map = entityMap;

	SetPosition(initialPos);

	m_noClipMode = false;


}

void Entity::Update(float deltaSeconds)
{
	m_ageInSeconds+=deltaSeconds;

}

void Entity::Render()
{
	Block* block = m_map->GetHighestTileAtXY(m_position.x, m_position.y);
	Vector3 pos = block->GetTopCenter();
	g_theRenderer->DrawCube(pos, Vector3((float)BLOCK_WIDTH * .5f, (float)BLOCK_WIDTH, (float)BLOCK_WIDTH * .5f));
	RenderHealthBar();
	if (g_theGame->m_devMode){
		RenderDevMode();
	}
}

void Entity::RenderDevMode()
{
	//draw cosmetic radius

	//determine what to do for this shit...
}





bool Entity::IsAboutToBeDeleted()
{
	return m_aboutToBeDeleted;
}

//move to actor
void Entity::RenderHealthBar()
{

	//float percentFull = (float) m_health / (float) m_maxHealth;
	////if (percentFull < 1.f){
	//g_theRenderer->PushAndTransform2(m_position.GetVector2() + Vector2(0.f, .5f), 0.f, .4f);
	//Vector2 mins = Vector2(-1.f, 0.f);
	//Vector2 maxs = Vector2(1.f, .3f);
	//AABB2 boundsBorder = AABB2(mins, maxs);
	//g_theRenderer->DrawAABB2(boundsBorder, RGBA(0,0,0));	//draw black outline

	//g_theRenderer->PushAndTransform2(Vector2(0.f, .1f), 0.f, .9f);
	//AABB2 boundsBackground = AABB2(mins, Vector2(maxs.x, maxs.y-.15f));
	//g_theRenderer->DrawAABB2(boundsBackground, RGBA(255,0,0));// draw red background;
	//Vector2 maxsPercentage = Vector2(RangeMapFloat(percentFull,0.f,1.f,-1.f,1.f), maxs.y-.15f);
	//AABB2 boundsHealth = AABB2(mins, maxsPercentage);
	//g_theRenderer->DrawAABB2(boundsHealth, RGBA(0,255,0));
	//g_theRenderer->Pop2();
	//g_theRenderer->Pop2();
	////}



}

void Entity::SetPosition(IntVector2 newPosition)
{
	m_position = newPosition;
	m_worldPosition = m_map->GetWorldPosition(newPosition);
}

void Entity::SetPosition(Vector3 newPosition)
{

	m_position = IntVector2(RoundToNearestInt(newPosition.x), RoundToNearestInt(newPosition.z));

	m_worldPosition = Vector3(newPosition.x, m_map->GetHeightAtXY(m_position), newPosition.z);		//check that the height is correct ( i.e. if moving onto a new tile this frame)

	
}

bool Entity::IsPointInForwardView(Vector2 point)
{
	//refactor for 3D
	UNIMPLEMENTED();
	return false;
	//return IsPointInConicSector2D(point, m_position.GetVector2(), m_facing.GetVector2(), m_forwardViewAngle, 10.f);
}


