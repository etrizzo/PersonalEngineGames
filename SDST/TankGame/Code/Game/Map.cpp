#include "Map.hpp"
#include "Game.hpp"
#include "Game/MapGenStep.hpp"
#include "DebugRenderSystem.hpp"
#include "Game/Spawner.hpp"

Map::~Map()
{

}

Map::Map(std::string imageFile, AABB2 const & extents, float minHeight, float maxHeight, IntVector2 chunks, float tilesPerChunk)
{
	m_renderable = new Renderable();
	Image img =  Image("Data/Images/" + imageFile);
	IntVector2 imgSize = img.GetDimensions();
	m_extents = extents;
	
	m_chunkDimensions = chunks;
	float chunkWidth = extents.GetWidth() / (float) chunks.x;
	float chunkHeight = extents.GetHeight() / (float) chunks.y;
	float tileWidth = chunkWidth / tilesPerChunk;
	float tileHeight = chunkHeight/ tilesPerChunk;
	m_tileSize = Vector2(tileWidth, tileHeight);
	m_heightRange = Vector2(minHeight, maxHeight);
	m_tilesPerChunk = tilesPerChunk;

	int numTilesWidth = (int) (m_extents.GetWidth() / m_tileSize.x	);
	int numTilesHeight = (int)( m_extents.GetHeight() / m_tileSize.y);
	m_dimensions = IntVector2 (numTilesWidth, numTilesHeight);
	//how many pixels per chunk?
	//IntVector2 pixelsPerTile = IntVector2(imgSize.x / )
	//m_dimensions = chunks
	RunMapGeneration(img);

	//If there is actual height to the terrain, add water renderable
	if (maxHeight - minHeight > .001f){
		float waterHeight = Interpolate(minHeight, maxHeight, .3f);
		Vector3 planeCenter = Vector3(m_extents.GetCenter().x, waterHeight, m_extents.GetCenter().y);
		m_waterRenderable = new Renderable();
		MeshBuilder mb = MeshBuilder();
		mb.Begin(PRIMITIVE_TRIANGLES, true);
		mb.AppendPlane(planeCenter, FORWARD, RIGHT, m_extents.GetDimensions(), RGBA::WHITE.GetColorWithAlpha(200), Vector2::ZERO, (Vector2::ONE * 128));
		mb.End();

		m_waterRenderable->SetMesh(mb.CreateMesh());
		m_waterRenderable->SetMaterial(Material::GetMaterial("water"));
		g_theGame->m_playState->m_scene->AddRenderable(m_waterRenderable);
	}

	g_theGame->m_playState->m_scene->AddRenderable(m_renderable);
	
	
}



void Map::Render()
{
	//RenderTiles();
	//RenderEntities();
	
}





void Map::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
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

float Map::GetHeightForVertex(int x, int y) const
{
	int index = GetIndexFromCoordinates(x, y, m_dimensions.x, m_dimensions.y);
	if (index >= 0){
		return m_heights[index];
	} else {
		return 0.f;
	}
}

Vector3 Map::GetVertexWorldPos(int x, int y) const
{
	Vector2 xyPos = m_extents.mins + (Vector2(m_tileSize.x * x, m_tileSize.y * y));
	float height = GetHeightForVertex(x,y);
	return Vector3(xyPos.x, height, xyPos.y);
}

Vector3 Map::GetVertexWorldPos(IntVector2 xy) const
{
	return GetVertexWorldPos(xy.x, xy.y);
}

float Map::GetHeightAtCoord(Vector2 xzCoord) const
{
	IntVector2 bl_coords = GetVertexCoordsFromWorldPos(xzCoord);
	if (bl_coords == IntVector2::INVALID_INDEX){
		return -INFINITY;
	} else {
		Vector3 blVert = GetVertexWorldPos(bl_coords.x, bl_coords.y);
		Vector3 brVert = GetVertexWorldPos(bl_coords.x + 1, bl_coords.y);
		Vector3 tlVert = GetVertexWorldPos(bl_coords.x, bl_coords.y + 1);
		Vector3 trVert = GetVertexWorldPos(bl_coords.x + 1, bl_coords.y + 1);



		AABB2 bounds = AABB2(blVert.XZ(), trVert.XZ());
		Vector2 percentage = bounds.GetPercentageOfPoint(xzCoord);

		//find heights along u edges
		float heightbottom = Interpolate(blVert.y, brVert.y, percentage.x);
		float heightTop = Interpolate(tlVert.y, trVert.y, percentage.x);
		float height = Interpolate(heightbottom, heightTop, percentage.y);

		return height;
	}
}

Vector3 Map::GetPositionAtCoord(Vector2 xzCoord) const
{
	return Vector3(xzCoord.x, GetHeightAtCoord(xzCoord), xzCoord.y);
}

Vector3 Map::GetNormalAtPosition(Vector2 xzCoord) const
{
	//IntVector2 bl_coords = GetVertexCoordsFromWorldPos(xzCoord);
	///*int numTilesWidth = (int) (m_extents.GetWidth() / m_tileSize.x	);
	//int numTilesHeight = (int)( m_extents.GetHeight() / m_tileSize.y);
	//int idx = GetIndexFromCoordinates(bl_coords.x, bl_coords.y, numTilesWidth, numTilesHeight);
	//Vector3 normal = m_normals[idx];
	//return normal;*/

	//Vector3 du = GetVertexWorldPos( bl_coords + IntVector2(1, 0) ) 
	//	- GetVertexWorldPos( bl_coords - IntVector2(1, 0) ); 
	//Vector3 tangent = du.GetNormalized(); 

	//Vector3 dv = GetVertexWorldPos( bl_coords + IntVector2(0, 1) ) 
	//	- GetVertexWorldPos( bl_coords - IntVector2(0, 1) ); 
	//Vector3 bitan = dv.GetNormalized(); 

	//Vector3 normal = Cross( bitan, tangent ); 
	//return normal.GetNormalized(); 

	IntVector2 bl_coords = GetVertexCoordsFromWorldPos(xzCoord);
	//Vector3 bl_vert = GetVertexWorldPos(bl_coords.x, bl_coords.y);
	Vector3 blNorm = GetNormalForVertex(bl_coords.x, bl_coords.y);
	Vector3 brNorm = GetNormalForVertex(bl_coords.x + 1, bl_coords.y);
	Vector3 tlNorm = GetNormalForVertex(bl_coords.x, bl_coords.y + 1);
	Vector3 trNorm = GetNormalForVertex(bl_coords.x + 1, bl_coords.y + 1);

	Vector3 blVert = GetVertexWorldPos(bl_coords.x, bl_coords.y);

	Vector3 trVert = GetVertexWorldPos(bl_coords.x + 1, bl_coords.y + 1);


	AABB2 bounds = AABB2(blVert.XZ(), trVert.XZ());
	Vector2 percentage = bounds.GetPercentageOfPoint(xzCoord);

	//find heights along u edges
	Vector3 normbottom = Interpolate(blNorm, brNorm, percentage.x);
	Vector3 normTop = Interpolate(tlNorm, trNorm, percentage.x);
	Vector3 norm = Interpolate(normbottom, normTop, percentage.y);



	return norm.GetNormalized();
}

Vector3 Map::GetNormalForVertex(IntVector2 vertCoords) const
{
	return GetNormalForVertex(vertCoords.x, vertCoords.y);
}

Vector3 Map::GetNormalForVertex(int x, int y) const
{
	int idx = GetIndexFromCoordinates(x, y, m_dimensions.x, m_dimensions.y);
	idx = ClampInt(idx, 0 , m_dimensions.x * m_dimensions.y);
	return m_normals[idx];
}

IntVector2 Map::GetVertexCoordsFromWorldPos(Vector2 xzPos) const
{
	if (m_extents.IsPointInside(xzPos)){
		Vector2 percentage = m_extents.GetPercentageOfPoint(xzPos);
		Vector2 coordFloat  = Vector2( percentage.x * (float)m_dimensions.x, percentage.y * (float) m_dimensions.y);
		IntVector2 coords = IntVector2((int)floor(coordFloat.x), (int)floor(coordFloat.y));
		return coords;
	} else {
		return IntVector2::INVALID_INDEX;
	}
}

bool Map::Raycast(Contact3D & contact, int maxHits, const Ray3D & ray, float maxDistance)
{
	UNUSED(maxHits);
	float stepSize = .2f * m_tileSize.x;
	if (!HitRaycastTarget(ray.m_position)){		//check that you're not starting at a hit
		//step along the ray until max distance is reached, or you hit something/cross the terrain
		float t = stepSize;
		Vector3 abovePoint = ray.m_position;	//the previous/current step position
		Vector3 belowPoint = ray.Evaluate(t);	//the next step position
		while (!HitRaycastTarget(belowPoint) && t < maxDistance){
			t+=stepSize;
			abovePoint = belowPoint;
			belowPoint = ray.Evaluate(t);
		}
		if ((t >= maxDistance) || !HitRaycastTarget(belowPoint)){
			//no hit in specified distance - either went outside of terrain bounds, or went too far
			return false;
		}


		//focus in by checking midpoint until you find a point close enough to the terrain
		Vector3 avgPoint = Average(abovePoint, belowPoint);
		int checkMidpointCount = 0;
		while ((GetVerticalDistanceFromTerrain(avgPoint) > SMALL_VALUE) && (checkMidpointCount < MAX_RAYCAST_REFINEMENTS)){
			if (!HitRaycastTarget(avgPoint)){
				abovePoint = avgPoint;
			} else {
				belowPoint = avgPoint;
			}
			avgPoint = Average(abovePoint, belowPoint);
			checkMidpointCount++;
		}
		contact.m_position = avgPoint;
		contact.m_normal = GetNormalAtPosition(avgPoint.XZ());
		return true;

	}
	return false;
}

bool Map::IsPointAboveTerrain(const Vector3 & point) const
{
	float terrainHeight = GetHeightAtCoord(Vector2(point.x, point.z));

	if (terrainHeight <= point.y){
		return true;
	} else {
		return false;
	}
}

bool Map::IsPointInEnemy(const Vector3 & point) const
{
	//check all enemies
	for (Enemy* baddy : g_theGame->m_playState->m_enemies){
		if (baddy->IsPointInside(point)){
			return true;
		}
	}
	return false;
}

bool Map::IsPointInSpawner(const Vector3 & point) const
{
	for (Spawner* baddy : g_theGame->m_playState->m_spawners){
		if (baddy->IsPointInside(point)){
			return true;
		}
	}
	return false;
}

bool Map::HitRaycastTarget(const Vector3 & point) const
{
	bool inTerrain = !IsPointAboveTerrain(point);
	bool inSpawner = IsPointInSpawner(point);
	bool inEnemy = IsPointInEnemy(point);
	return inTerrain || inSpawner || inEnemy;
}

float Map::GetVerticalDistanceFromTerrain(const Vector3 & point) const
{
	float terrainHeight = GetHeightAtCoord(Vector2(point.x, point.z));
	float dist = point.y - terrainHeight;
	return fabs(dist);
}

void Map::DebugShowTileAtPoint(Vector2 xzPos)
{
	IntVector2 bl_coords = GetVertexCoordsFromWorldPos(xzPos);
	Vector3 blVert = GetVertexWorldPos(bl_coords.x, bl_coords.y);
	Vector3 brVert = GetVertexWorldPos(bl_coords.x + 1, bl_coords.y);
	Vector3 tlVert = GetVertexWorldPos(bl_coords.x, bl_coords.y + 1);
	Vector3 trVert = GetVertexWorldPos(bl_coords.x + 1, bl_coords.y + 1);

	g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.f, blVert, RGBA::RED);
	g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.f, brVert, RGBA::GREEN);
	g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.f, tlVert, RGBA::BLUE);
	g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.f, trVert, RGBA::MAGENTA);
}

void Map::DebugShowNormalsAtPoint(Vector2 xzPos)
{
	IntVector2 bl_coords = GetVertexCoordsFromWorldPos(xzPos);
	Vector3 blVert = GetVertexWorldPos(bl_coords.x, bl_coords.y);
	Vector3 brVert = GetVertexWorldPos(bl_coords.x + 1, bl_coords.y);
	Vector3 tlVert = GetVertexWorldPos(bl_coords.x, bl_coords.y + 1);
	Vector3 trVert = GetVertexWorldPos(bl_coords.x + 1, bl_coords.y + 1);

	Vector3 blNorm = GetNormalForVertex(bl_coords.x, bl_coords.y);
	Vector3 brNorm = GetNormalForVertex(bl_coords.x + 1, bl_coords.y);
	Vector3 tlNorm = GetNormalForVertex(bl_coords.x, bl_coords.y + 1);
	Vector3 trNorm = GetNormalForVertex(bl_coords.x + 1, bl_coords.y + 1);

	AABB2 bounds = AABB2(blVert.XZ(), trVert.XZ());
	Vector2 percentage = bounds.GetPercentageOfPoint(xzPos);

	Vector3 normbottom = Interpolate(blNorm, brNorm, percentage.x);
	Vector3 normTop = Interpolate(tlNorm, trNorm, percentage.x);
	Vector3 norm = Interpolate(normbottom, normTop, percentage.y);

	//DEBUG DRAWING

	g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(blVert, blVert + blNorm, RGBA::RED);
	g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(brVert, brVert + brNorm, RGBA::GREEN);
	g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(tlVert, tlVert + tlNorm, RGBA::BLUE);
	g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(trVert, trVert + trNorm, RGBA::MAGENTA);

	Vector3 pos = GetPositionAtCoord(xzPos);
	g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(pos, (pos + norm.GetNormalized()), RGBA::YELLOW, RGBA::YELLOW);
	

	// END DEBUG DRAWING
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

//bool Map::HasLineOfSight(const Vector2 & startPos, const Vector2 & endPos, float maxDistance)
//{
//	/*Vector2 direction = (endPos - startPos);
//	if (direction.GetLength() > maxDistance){
//		return false;
//	} else {
//		RaycastResult2D raycastResult = Raycast(startPos, direction.GetNormalized(), maxDistance);
//		if (raycastResult.m_didImpact){
//			if (raycastResult.m_impactDistance > direction.GetLength()){
//				return true;
//			} else {
//				return false;
//			}
//		}
//		
//	}*/
//	return true;
//}




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
	//int numTilesWidth = (int) (m_extents.GetWidth() / m_tileSize.x	);
	//int numTilesHeight = (int)( m_extents.GetHeight() / m_tileSize.y);
	m_bounds = AABB3(Vector3(INFINITY, INFINITY, INFINITY), Vector3(-INFINITY, -INFINITY, -INFINITY));
	m_heights.resize(m_dimensions.x * m_dimensions.y);
	m_normals.resize(m_dimensions.x * m_dimensions.y);
	//read vertex heights from image data
	for(int x = 0; x < m_dimensions.x; x++){
		for (int y = 0; y < m_dimensions.y; y++){
			Vector2 xzPos = Vector2(x * m_tileSize.x, y * m_tileSize.y) + m_extents.mins;
			//if (m_extents.IsPointInside(xzPos)){
				Vector2 uvs = m_extents.GetPercentageOfPoint(xzPos);
				uvs = ClampVector2(uvs, Vector2::ZERO, Vector2::ONE);
				//Vector2 coordFloat  = Vector2( percentage.x * (float)img.GetDimensions().x, percentage.y * (float) img.GetDimensions().y);
				//IntVector2 coords = IntVector2((int)floor(coordFloat.x), (int)floor(coordFloat.y));
				//float r = (float) img.GetTexel(coords.x, coords.y).r;
				float r = img.GetTexelAtUVS(uvs).r;
				int idx = GetIndexFromCoordinates(x, y, m_dimensions.x, m_dimensions.y);
				m_heights[idx] = RangeMapFloat(r, 0, 255.f, m_heightRange.x, m_heightRange.y);
			//}
		}
	}
	MeshBuilder mb = MeshBuilder();
	Material* terrainMat = Material::GetMaterial("terrain");
	for (int chunkX = 0; chunkX < m_chunkDimensions.x; chunkX++){
		for (int chunkY = 0; chunkY < m_chunkDimensions.y; chunkY++){
			int startX = (int) (chunkX * m_tilesPerChunk);
			int startY = (int) (chunkY * m_tilesPerChunk);
			int endX = (int) (startX + m_tilesPerChunk);
			int endY = (int) (startY + m_tilesPerChunk);

			//generate mesh
			mb.Clear();
			mb.Begin(PRIMITIVE_TRIANGLES, true);
			
			for (int x = startX; x < endX; x++){
				for (int y = startY; y < endY; y++){
					//for each vertex, append plane (as bottom right)
					
					Vector3 blPos = GetVertexWorldPos(x,y);
					Vector3 brPos = GetVertexWorldPos(x+1, y);
					Vector3 tlPos = GetVertexWorldPos(x, y+1);
					Vector3 trPos = GetVertexWorldPos(x+1, y+1);
					
					Vector3 right = (brPos - blPos).GetNormalized();
					Vector3 up = (tlPos - blPos).GetNormalized();
					int idx = GetIndexFromCoordinates(x, y, m_dimensions.x, m_dimensions.y);
					Vector3 normal = Cross(up, right).GetNormalized();
					m_normals[idx] = normal;
					if ( (x + 1) < m_dimensions.x && (y + 1 ) < m_dimensions.y){
						mb.AppendPlane(blPos, brPos, tlPos, trPos, RGBA::WHITE, Vector2::ZERO, Vector2::ONE);
					}
					m_bounds.StretchToIncludePoint(blPos);
				}
			}
			mb.End();
			Chunk* chunk = new Chunk();
			chunk->Setup(this, IntVector2(chunkX, chunkY));
			chunk->SetMesh(mb.CreateMesh());
			chunk->SetMaterial(terrainMat);

			g_theGame->m_playState->m_scene->AddRenderable(chunk->m_renderable);
			m_chunks.push_back(chunk);
			//m_renderable->SetMesh(mb.CreateMesh());
			//m_renderable->SetMaterial(terrainMat);
		}
	}
	//m_renderable->SetMaterial(Material::GetMaterial("wireframe"));

}


//RaycastResult2D::RaycastResult2D(bool didImpact, Vector2 impactPos, IntVector2 impactTileCoords, float impactDistance, Vector2 impactNormal)
//{
//	m_didImpact = didImpact;
//	m_impactPosition = impactPos;
//	m_impactTileCoordinates = impactTileCoords;
//	m_impactDistance = impactDistance;
//	m_impactNormal = impactNormal;
//}
