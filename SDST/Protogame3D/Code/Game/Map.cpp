#include "Map.hpp"
#include "Game.hpp"
#include "Game/MapGenStep.hpp"

Map::~Map()
{

}

Map::Map(std::string imageFile, AABB2 const & extents, float minHeight, float maxHeight, IntVector2 chunks)
{
	m_renderable = new Renderable();
	Image img =  Image("Data/Images/" + imageFile);
	IntVector2 imgSize = img.GetDimensions();
	m_dimensions = img.GetDimensions();
	m_extents = extents;
	float chunkWidth = extents.GetWidth() / (float) chunks.x;
	float chunkHeight = extents.GetHeight() / (float) chunks.y;
	float tileWidth = extents.GetWidth() / (float) imgSize.x;
	float tileHeight = extents.GetHeight()/ (float) imgSize.y;
	m_tileSize = Vector2(tileWidth, tileHeight);
	m_heightRange = Vector2(minHeight, maxHeight);
	//how many pixels per chunk?
	//IntVector2 pixelsPerTile = IntVector2(imgSize.x / )
	//m_dimensions = chunks
	RunMapGeneration(img);
	g_theGame->m_playState->m_scene->AddRenderable(m_renderable);
}



void Map::Render()
{
	//RenderTiles();
	//RenderEntities();
	
}





void Map::Update(float deltaSeconds)
{
	//UpdateEntities(deltaSeconds);

	//RunPhysics();

	CheckEntityInteractions();

}


//void Map::RenderEntities()
//{
//	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
//		m_allEntities.at(entityIndex)->Render();
//	}
//}


void Map::CheckEntityInteractions()
{
	

}


int Map::GetWidth() const 
{
	return m_dimensions.x;
}

int Map::GetHeight() const 
{
	return m_dimensions.y;
}

float Map::GetHeightForVertex(int x, int y)
{
	int index = GetIndexFromCoordinates(x, y, m_dimensions.x, m_dimensions.y);
	return m_heights[index];
}

Vector3 Map::GetVertexWorldPos(int x, int y)
{
	Vector2 xyPos = m_extents.mins + (Vector2(m_tileSize.x * x, m_tileSize.y * y));
	float height = GetHeightForVertex(x,y);
	return Vector3(xyPos.x, height, xyPos.y);
}

float Map::GetHeightAtCoord(Vector2 xzCoord)
{
	IntVector2 bl_coords = GetVertexCoordsFromWorldPos(xzCoord);
	//Vector3 bl_vert = GetVertexWorldPos(bl_coords.x, bl_coords.y);
	float height = GetHeightForVertex(bl_coords.x, bl_coords.y);
	return height;
}

IntVector2 Map::GetVertexCoordsFromWorldPos(Vector2 xzPos)
{
	if (m_extents.IsPointInside(xzPos)){
		Vector2 percentage = m_extents.GetPercentageOfPoint(xzPos);
		Vector2 coordFloat  = Vector2( percentage.x * (float)m_dimensions.x, percentage.y * (float) m_dimensions.y);
		IntVector2 coords = IntVector2((int)floor(coordFloat.x), (int)floor(coordFloat.y));
		return coords;
	} else {
		return IntVector2(0,0);
	}
}


//Tile* Map::TileAt(int x, int y) 
//{
//	return TileAt(IntVector2(x,y));
//}
//
//Tile* Map::TileAt(IntVector2 coordinates) 
//{
//	IntVector2 clampedCoords = IntVector2(ClampInt(coordinates.x, 0, m_dimensions.x -1), ClampInt(coordinates.y, 0, m_dimensions.y - 1));
//	return &m_tiles[GetIndexFromCoordinates(clampedCoords.x, clampedCoords.y, m_dimensions.x, m_dimensions.y)];
//}
//
//Tile* Map::TileAtFloat(float x, float y)
//{
//	
//	IntVector2 intCoords = IntVector2((int)floor(x), (int)floor(y));
//	return TileAt(intCoords);
//}
//
//Tile* Map::TileAtFloat(Vector2 floatCoordinates)
//{
//	IntVector2 intCoords = IntVector2((int) floor(floatCoordinates.x), (int) floor(floatCoordinates.y));
//	return TileAt(intCoords);
//}

bool Map::HasLineOfSight(const Vector2 & startPos, const Vector2 & endPos, float maxDistance)
{
	/*Vector2 direction = (endPos - startPos);
	if (direction.GetLength() > maxDistance){
		return false;
	} else {
		RaycastResult2D raycastResult = Raycast(startPos, direction.GetNormalized(), maxDistance);
		if (raycastResult.m_didImpact){
			if (raycastResult.m_impactDistance > direction.GetLength()){
				return true;
			} else {
				return false;
			}
		}
		
	}*/
	return true;
}




//RaycastResult2D Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) 
//{
//	int numSteps = (int)(maxDistance * RAYCAST_STEPS_PER_TILE);
//	Vector2 singleStep = direction/RAYCAST_STEPS_PER_TILE;
//	Tile previousTile = *TileAtFloat(startPos);
//	Tile currentTile = previousTile;
//	Vector2 currentPosition = startPos;
//	if (previousTile.m_tileDef->m_allowsSight){			//TODO: maybe need to think about this more (clear solid blocks? non-solid vision blocking blocks?)
//		return RaycastResult2D(true, startPos, previousTile.m_coordinates, 0.f,Vector2(0.f,0.f));
//	}
//	for (int stepNumber = 0; stepNumber < numSteps; stepNumber++){
//		currentPosition = startPos + (singleStep * (float) stepNumber);
//		currentTile = *TileAtFloat(currentPosition);
//		if (currentTile.m_coordinates == previousTile.m_coordinates){
//			continue;
//		} else {
//			if (currentTile.m_tileDef->m_allowsSight){
//				float distanceFromStart = (currentPosition - startPos).GetLength();
//				return RaycastResult2D(true, currentPosition, currentTile.m_coordinates, distanceFromStart, (currentTile.m_coordinates - previousTile.m_coordinates).GetVector2().GetNormalized());
//			} else {
//				previousTile = currentTile;
//			}
//		}
//	}
//	return RaycastResult2D(false, currentPosition, currentTile.m_coordinates, maxDistance, Vector2(0.f,0.f));
//}






void Map::RunMapGeneration(const Image& img)
{
	int numTexels = img.GetDimensions().x * img.GetDimensions().y;
	m_heights.resize(numTexels);
	//read vertex heights from image data
	for(int i = 0; i < numTexels; i++){
		float r = (float) img.GetTexel(i).r;
		m_heights[i] = RangeMapFloat(r, 0, 255.f, m_heightRange.x, m_heightRange.y);
	}

	//generate mesh
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	for (int x = 0; x < img.GetDimensions().x-1; x++){
		for (int y = 0; y < img.GetDimensions().y-1; y++){
			//for each vertex, append plane (as bottom right)
			int botLeftIndex = x;
			Vector3 blPos = GetVertexWorldPos(x,y);
			Vector3 brPos = GetVertexWorldPos(x+1, y);
			Vector3 tlPos = GetVertexWorldPos(x, y+1);
			Vector3 trPos = GetVertexWorldPos(x+1, y+1);
			mb.AppendPlane(blPos, brPos, tlPos, trPos, RGBA::WHITE, Vector2::ZERO, Vector2::ONE);
		}
	}
	mb.End();
	m_renderable->SetMesh(mb.CreateMesh());
	m_renderable->SetMaterial(Material::GetMaterial("wireframe"));

}


//RaycastResult2D::RaycastResult2D(bool didImpact, Vector2 impactPos, IntVector2 impactTileCoords, float impactDistance, Vector2 impactNormal)
//{
//	m_didImpact = didImpact;
//	m_impactPosition = impactPos;
//	m_impactTileCoordinates = impactTileCoords;
//	m_impactDistance = impactDistance;
//	m_impactNormal = impactNormal;
//}
