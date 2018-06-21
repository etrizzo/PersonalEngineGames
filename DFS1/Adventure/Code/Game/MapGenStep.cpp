#include "MapGenStep.hpp"

MapGenStep::MapGenStep(const tinyxml2::XMLElement & genStepXmlElement)
	:m_subAreaCenter(.5f),
	m_subAreaSize(.5f)
{
	m_name = genStepXmlElement.Name();
	m_iterations = ParseXmlAttribute(genStepXmlElement, "iterations",m_iterations);
	m_chanceToRun = ParseXmlAttribute(genStepXmlElement,  "chanceToRun", 1.f);

	m_subAreaCenter = ParseXmlAttribute(genStepXmlElement, "subAreaCenter", m_subAreaCenter);
	m_subAreaSize = ParseXmlAttribute(genStepXmlElement, "subAreaSize", m_subAreaSize);

	m_centerDensity = ParseXmlAttribute(genStepXmlElement, "centerDensity", m_centerDensity);
	m_edgeDensity	= ParseXmlAttribute(genStepXmlElement, "edgeDensity", m_edgeDensity);
	m_featherRate	= ParseXmlAttribute(genStepXmlElement, "feather", m_featherRate);

	std::string areaType = ParseXmlAttribute(genStepXmlElement, "type", "NONE");
	if (areaType == "circle"){
		m_maskType = AREA_TYPE_CIRCLE;
	} else if (areaType == "rectangle"){
		m_maskType = AREA_TYPE_RECTANGLE;
	} else if (areaType == "perlin"){
		m_maskType = AREA_TYPE_PERLIN;
	} else {
		m_maskType = NUM_AREA_TYPES;
	}

	//set defaults for perlin noise seed and noise scale, then check for overwrites
	
	m_noiseScale = 20;
	m_perlinSeed = (unsigned int) ParseXmlAttribute(genStepXmlElement, "seed", (int) 0);
	m_noiseScale = ParseXmlAttribute(genStepXmlElement, "noiseScale", m_noiseScale);
}

MapGenStep::~MapGenStep()
{
	
}

void MapGenStep::RunIterations(Map & map)
{
	if (CheckRandomChance(m_chanceToRun)){
		int numIterations = m_iterations.GetRandomInRange();
		for (int i = 0; i < numIterations; i++){
			SetMask(map);
			Run(map);
		}
	}
}

void MapGenStep::SetMask(Map& map)
{
	TODO("Find a better way to adjust subarea to constraints (instead of just shrinking)");
	//if no mask type has been specified, use the maps current mask type
	if (m_maskType == NUM_AREA_TYPES){
		m_maskType = map.m_maskType;
	}

	//generate area mask based on type
	if (m_maskType == AREA_TYPE_RECTANGLE){
		m_mask = map.m_generationMask->GetSubArea(m_subAreaCenter, m_subAreaSize);
	} else if (m_maskType == AREA_TYPE_CIRCLE){
		float centerX = m_subAreaCenter.GetRandomInRange();
		float centerY = m_subAreaCenter.GetRandomInRange();
		IntVector2 center = map.m_generationMask->GetMins() + IntVector2((int) (centerX * map.m_generationMask->GetWidth()), (int) (centerY * map.m_generationMask->GetHeight()));
		float radius = m_subAreaSize.GetRandomInRange() * map.m_generationMask->GetWidth();
		m_mask = (AreaMask*) (new AreaMask_Circle(center, (int) radius, m_centerDensity, m_edgeDensity, m_featherRate));
	} else if (m_maskType == AREA_TYPE_PERLIN) {
		unsigned int seed = m_perlinSeed;
		if (map.m_generationMask->m_seed != 0){		//if the parent area has a seed, use that
			seed = map.m_generationMask->m_seed;
		}
		if (seed == 0){	//generate new random seed per iteration
			seed = (unsigned int) GetRandomIntLessThan(2000);
		}
		if (m_subAreaSize.min == m_subAreaSize.max){
			m_subAreaSize = map.m_generationMask->m_acceptableRange;
		}
		m_mask = (AreaMask*) (new AreaMask_Perlin(m_subAreaSize, IntVector2(0, 0), map.m_dimensions, seed, m_noiseScale));
	}
	//copy values from map if they weren't defined in XML
	if (m_centerDensity < 0.f){
		m_centerDensity = map.m_generationMask->m_centerDensity;
	}
	if (m_edgeDensity < 0.f){
		m_edgeDensity = map.m_generationMask->m_edgeDensity;
	}
	if (m_featherRate < 0.f){
		m_featherRate = map.m_generationMask->m_featherRate;
	}
	m_mask->SetDensity(m_centerDensity, m_edgeDensity, m_featherRate);
}

MapGenStep * MapGenStep::CreateMapGenStep(const tinyxml2::XMLElement & genStepXmlElement)
{
	std::string genName = genStepXmlElement.Name();
	MapGenStep* newMapGenStep = nullptr;
	if (genName == "FillAndEdge"){
		newMapGenStep = (MapGenStep*) new MapGenStep_FillAndEdge(genStepXmlElement);
	}
	if (genName == "FromFile"){
		newMapGenStep = (MapGenStep*) new MapGenStep_FromFile(genStepXmlElement);
	}
	if (genName == "Mutate"){
		newMapGenStep = (MapGenStep*) new MapGenStep_Mutate(genStepXmlElement);
	}
	if (genName == "PerlinNoise"){
		newMapGenStep = (MapGenStep*) new MapGenStep_PerlinNoise(genStepXmlElement);
	}
	if (genName == "CellularAutomata"){
		newMapGenStep = (MapGenStep*) new MapGenStep_CellularAutomata(genStepXmlElement);
	}
	if (genName=="Conway"){
		newMapGenStep = (MapGenStep*) new MapGenStep_Conway(genStepXmlElement);
	}
	if (genName == "RoomsAndPaths"){
		newMapGenStep = (MapGenStep*) new MapGenStep_RoomAndPath(genStepXmlElement);
	}
	if(genName == "SpawnActor"){
		newMapGenStep = (MapGenStep*) new MapGenStep_SpawnActor(genStepXmlElement);
	}
	if (genName == "SpawnItem"){
		newMapGenStep = (MapGenStep*) new MapGenStep_SpawnItem(genStepXmlElement);
	}
	if (genName == "SpawnDecoration"){
		newMapGenStep = (MapGenStep*) new MapGenStep_SpawnDecoration(genStepXmlElement);
	}
	if (genName == "SetSubArea"){
		newMapGenStep = (MapGenStep*) new MapGenStep_SetSubArea(genStepXmlElement);
	}
	if (genName == "EndSubArea"){
		newMapGenStep = (MapGenStep*) new MapGenStep_EndSubArea(genStepXmlElement);
	}
	if (genName == "SubMap"){
		newMapGenStep = (MapGenStep*) new MapGenStep_SubMap(genStepXmlElement);
	}



	if (newMapGenStep == nullptr){
		ERROR_AND_DIE("No map gen step called: " + genName);
	}
	return newMapGenStep;
}

MapGenStep_FillAndEdge::MapGenStep_FillAndEdge( const tinyxml2::XMLElement& generationStepElement )
	: MapGenStep( generationStepElement )
{
	m_fillTileDef = ParseXmlAttribute( generationStepElement, "fillTile", m_fillTileDef );
	m_edgeTileDef = ParseXmlAttribute( generationStepElement, "edgeTile", m_edgeTileDef );
	m_edgeThickness = ParseXmlAttribute( generationStepElement, "edgeThickness", m_edgeThickness );
	m_chanceToChangeEachTile = ParseXmlAttribute( generationStepElement, "edgeChance", m_chanceToChangeEachTile);
}

void MapGenStep_FillAndEdge::Run(Map & map)
{
	int innerBound = -1 + m_edgeThickness;
	int outerBoundX = map.GetWidth() - m_edgeThickness;
	int outerBoundY = map.GetHeight() - m_edgeThickness;
	for(int tileIndex = 0; tileIndex < (int) map.m_tiles.size(); tileIndex++){
		IntVector2 tileCoords = GetCoordinatesFromIndex(tileIndex, map.GetWidth());
		if (m_mask->CanDrawOnPoint(tileCoords)){
			if ( m_mask->GetDistanceFromEdge(tileCoords.x, tileCoords.y) < m_edgeThickness){
				if (CheckRandomChance(m_chanceToChangeEachTile)){
					map.m_tiles[tileIndex].SetType(m_edgeTileDef);
				} else {
					if (m_fillTileDef != nullptr){
						map.m_tiles[tileIndex].SetType(m_fillTileDef);
					}
				}
			} else {
				if (m_fillTileDef != nullptr){
					map.m_tiles[tileIndex].SetType(m_fillTileDef);
				}
			}
		}
	}
}

MapGenStep_FromFile::MapGenStep_FromFile(const tinyxml2::XMLElement & generationStepElement)
	: MapGenStep( generationStepElement )
{
	m_fileName = ParseXmlAttribute(generationStepElement, "fileName", m_fileName);
	m_fileName = "Data/Images/" + m_fileName;

	IntRange startX = ParseXmlAttribute(generationStepElement, "startX", IntRange(0));
	IntRange startY = ParseXmlAttribute(generationStepElement, "startY", IntRange(0));
	m_startCoordinates = IntVector2(startX.GetRandomInRange(), startY.GetRandomInRange());
}

void MapGenStep_FromFile::Run(Map & map)
{
	Image image = Image(m_fileName);
	IntVector2 dimensions = image.GetDimensions();
	for (int x = 0 ; x < dimensions.x; x++){
		for (int y = 0; y < dimensions.y; y++){
			int startX = x + m_startCoordinates.x;
			int startY = y + m_startCoordinates.y;
			if (startX < map.m_dimensions.x && startY < map.m_dimensions.y){
				RGBA texelColor = image.GetTexel(x,y);
				TileDefinition* newType = TileDefinition::GetTileDefinitionByChroma(texelColor);
				if (newType == nullptr){
					ERROR_AND_DIE("Chroma Key Not found!");
				} else {
					bool chance = CheckRandomChance(texelColor.PercentAlpha());
					if (chance){
						map.TileAt(startX,startY)->SetType(newType);
					}
				}
			}
		}
	}
}

MapGenStep_Mutate::MapGenStep_Mutate(const tinyxml2::XMLElement & generationStepElement)
	: MapGenStep( generationStepElement )
{
	m_chanceToMutate	= ParseXmlAttribute(generationStepElement, "chanceToMutate", m_chanceToMutate);
	m_ifType			= ParseXmlAttribute(generationStepElement, "ifType", m_ifType);
	m_toType			= ParseXmlAttribute(generationStepElement, "toType", m_toType);
}

void MapGenStep_Mutate::Run(Map & map)
{
	int mapWidth = map.GetWidth();
	int mapHeight = map.GetHeight();
	for (int tileX = 0; tileX < mapWidth; tileX++){
		for (int tileY = 0; tileY < mapHeight; tileY++){
			if (m_mask->CanDrawOnPoint(tileX, tileY)){
				Tile* tile = map.TileAt(tileX, tileY);
				if (m_ifType!= nullptr){
					std::string tileName = tile->m_tileDef->m_name;
					std::string ifTypeName = m_ifType->m_name;
					if (!tileName.compare(ifTypeName)){
						if (CheckRandomChance(m_chanceToMutate)){
							tile->SetType(m_toType);
						}
					}
				}
			}
		}
	}
}

MapGenStep_Conway::MapGenStep_Conway(const tinyxml2::XMLElement & generationStepElement)
	: MapGenStep( generationStepElement )
{
	m_deathLimit = ParseXmlAttribute(generationStepElement, "deathLimit", 3);
	m_birthLimit = ParseXmlAttribute(generationStepElement, "birthLimit", 4);
	m_liveTileDefinition = ParseXmlAttribute(generationStepElement, "liveTile", (TileDefinition*) nullptr);
	m_deadTileDefintion = ParseXmlAttribute(generationStepElement, "deadTile", (TileDefinition*) nullptr);

	m_iterations = ParseXmlAttribute(generationStepElement, "iterations", IntRange(2));		//reset default number of iterations for cellular automata

}

//runs a SINGLE iteration of cellular automata
void MapGenStep_Conway::Run(Map & map)
{
	std::vector<bool> cellStates = std::vector<bool>();
	cellStates.resize(map.m_numTiles);

	//set new states from map into cellStates, then update map
	for(int tileIndex = 0; tileIndex < map.m_numTiles; tileIndex++){
		IntVector2 tileCoords = GetCoordinatesFromIndex(tileIndex, map.GetWidth());
		if (m_mask->CanDrawOnPoint(tileCoords)){
			Tile neighbors[8];
			Tile currentTile = map.m_tiles[tileIndex];
			map.GetNeighbors(currentTile.m_coordinates, neighbors);
			int liveCount = 0;
			for(Tile neighborTile : neighbors){
				if (neighborTile.m_tileDef != nullptr){
					if (neighborTile.m_tileDef == m_liveTileDefinition){
						liveCount++;
					}
				}
			}

			if(currentTile.m_tileDef == m_liveTileDefinition){
				if(liveCount < m_deathLimit){
					cellStates[tileIndex] = false;
				}
				else{
					cellStates[tileIndex] = true;
				}
			} else {		//cell is currently dead
				if(liveCount > m_birthLimit){
					cellStates[tileIndex] = true;
				}
				else{
					cellStates[tileIndex] = false;
				}
			}
		}
	}

	for(int tileIndex = 0; tileIndex < map.m_numTiles; tileIndex++){
		//Tile tile = map.m_tiles[tileIndex];
		if (cellStates[tileIndex]){
			map.m_tiles[tileIndex].SetType(m_liveTileDefinition);
		} else {
			map.m_tiles[tileIndex].SetType(m_deadTileDefintion);
		}
	}

}

MapGenStep_CellularAutomata::MapGenStep_CellularAutomata(const tinyxml2::XMLElement & generationStepElement)
	: MapGenStep( generationStepElement )
{
	m_chanceToMutate					= ParseXmlAttribute(generationStepElement, "chanceToMutate", m_chanceToMutate);
	m_requiredNeighborCount	= ParseXmlAttribute(generationStepElement, "ifNeighborCount", m_requiredNeighborCount);
	m_ifTypeDef			= ParseXmlAttribute(generationStepElement, "ifType", m_ifTypeDef);
	m_changeToTypeDef	= ParseXmlAttribute(generationStepElement, "changeToType", m_changeToTypeDef);
	m_ifNeighborTypeDef	= ParseXmlAttribute(generationStepElement, "ifNeighborType", m_ifNeighborTypeDef);
}

void MapGenStep_CellularAutomata::Run(Map & map)
{
	std::vector<bool> cellsToChange = std::vector<bool>();
	cellsToChange.resize(map.m_numTiles);

	//set new states from map into cellStates, then update map
	for(int tileIndex = 0; tileIndex < map.m_numTiles; tileIndex++){
		IntVector2 tileCoords = GetCoordinatesFromIndex(tileIndex, map.GetWidth());
		if (m_mask->IsPointInside(tileCoords)){
			Tile currentTile = map.m_tiles[tileIndex];
			if(m_ifTypeDef == nullptr || currentTile.m_tileDef == m_ifTypeDef){
				Tile neighbors[8];
				map.GetNeighbors(currentTile.m_coordinates, neighbors);
				int liveCount = 0;
				//count neighboring tiles of ifNeighborTypeDef
				for(Tile neighborTile : neighbors){
					if (neighborTile.m_tileDef != nullptr){
						if (neighborTile.m_tileDef == m_ifNeighborTypeDef){
							liveCount++;
						}
					}
				}
				// if there are enough neighbors of the type to change, mark for change
				if(m_requiredNeighborCount.IsIntInRange(liveCount)){
					if (CheckRandomChance(m_chanceToMutate)){
						cellsToChange[tileIndex] = true;
					} else {
						cellsToChange[tileIndex] = false;
					}
				}
				else{
					cellsToChange[tileIndex] = false;
				}
			} 
		} else {
			cellsToChange[tileIndex] = false;
		}
	}

	for(int tileIndex = 0; tileIndex < map.m_numTiles; tileIndex++){
		//Tile tile = map.m_tiles[tileIndex];
		if (cellsToChange[tileIndex]){
			map.m_tiles[tileIndex].SetType(m_changeToTypeDef);
		}
	}
}

MapGenStep_RoomAndPath::MapGenStep_RoomAndPath(const tinyxml2::XMLElement & generationStepElement)
: MapGenStep(generationStepElement){
	const tinyxml2::XMLElement* roomsElement = generationStepElement.FirstChildElement("Rooms");
	const tinyxml2::XMLElement* pathsElement = generationStepElement.FirstChildElement("Paths");
	m_numRooms		 = ParseXmlAttribute(*roomsElement, "count"	, m_numRooms		);
	m_roomWidth		 = ParseXmlAttribute(*roomsElement, "width"	, m_roomWidth		);
	m_roomHeight	 = ParseXmlAttribute(*roomsElement, "height", m_roomHeight		);
	m_numOverlaps	 = ParseXmlAttribute(*roomsElement, "numOverlaps", m_numOverlaps);
	m_floorTileDef	 = ParseXmlAttribute(*roomsElement, "floor" , m_floorTileDef	);
	m_wallTileDef	 = ParseXmlAttribute(*roomsElement, "wall" , m_wallTileDef		);
	
	if (pathsElement != nullptr){
		m_pathTileDef		   = ParseXmlAttribute(*pathsElement, "floor"	, m_pathTileDef	);
		m_extraPaths		   = ParseXmlAttribute(*pathsElement, "extraCount" , m_extraPaths);
		m_loopPaths			   = ParseXmlAttribute(*pathsElement, "loop" , m_loopPaths);
		m_straightPathChance   = ParseXmlAttribute(*pathsElement, "straightChance" , m_straightPathChance );
		m_straightPathChance   = RangeMapFloat(m_straightPathChance, 0.f, 1.f, .5f, 1.f);
	}
}

void MapGenStep_RoomAndPath::Run(Map & map)
{
	m_rooms = std::vector<Room*>();
	GenerateRooms(map);
	
	AddRoomsToMap(map);
	GeneratePaths(map);
	
}

void MapGenStep_RoomAndPath::GenerateRooms(Map& map)
{
	int roomsToGenerate = m_numRooms.GetRandomInRange();
	int overlapsToAllow = m_numOverlaps.GetRandomInRange();
	//	2.	Pick (and remember) the first room’s random size and location (but do not modify tiles).
	Room* roomToBuild;
	int failures = 0;
	int overlapsUsed = 0;
	int roomsGenerated = 0;
	while (roomsGenerated <roomsToGenerate && failures < 1000){
		IntVector2 randomLocation = m_mask->GetRandomPointInArea(); //map.GetRandomTileCoords();
		IntVector2 randomSize = IntVector2(m_roomWidth.GetRandomInRange(), m_roomHeight.GetRandomInRange());
		roomToBuild = new Room(randomLocation, randomSize);
		bool failedToAdd = false;
		//check that the random room is on the map
		//could check that it's inside the mask, but letting buildings overrun the map feels better
		/*if (m_mask->IsPointInside(roomToBuild->m_minCoords) && m_mask->IsPointInside(roomToBuild->m_maxCoords)){*/
		if (map.IsCoordinateOnMap(roomToBuild->m_minCoords) && map.IsCoordinateOnMap(roomToBuild->m_maxCoords)){
			for(int i = 0; i < roomsGenerated; i++){
				Room* previousRoom = m_rooms[i];
				if (roomToBuild->DoesRoomOverlap(previousRoom)){
					if (overlapsUsed >= overlapsToAllow){
						failures++;
						failedToAdd = true;
						break;
					} else {
						overlapsUsed++;
					}
				} 
			}
		} else {
			failedToAdd = true;
			failures++;
		}
		if (!failedToAdd){
			roomsGenerated++;
			m_rooms.push_back(roomToBuild);
		}
	}

	//	3.	Pick the second room’s random size & location; if this overlaps the previous room(s), use one of the allowed number of overlaps.  If no [more] overlaps are allowed, reroll a new size & location and try again.
	//	4.	Proceed until the requested number of rooms have been planned, or until 1000 placement failures have occurred.
	//	5.	Set the exterior (wall) tiles for all rooms to rooms’ “wall” attribute tile type.

}

void MapGenStep_RoomAndPath::AddRoomsToMap(Map & map)
{
	for (Room* room : m_rooms){
		for (int x = room->m_minCoords.x; x < room->m_maxCoords.x; x++){
			for (int y = room->m_minCoords.y; y < room->m_maxCoords.y; y++){
				map.TileAt(x,y)->SetType(m_floorTileDef);
			}
		}
	}

	// set outer walls in a cellular automata kinda way ?
	for (Room* room : m_rooms){
		for (int x = room->m_minCoords.x; x < room->m_maxCoords.x; x++){
			for (int y = room->m_minCoords.y; y < room->m_maxCoords.y; y++){
				Tile* tileCheck = map.TileAt(x,y);
				Tile neighbors[8];
				map.GetNeighbors(tileCheck->m_coordinates, neighbors);
				for(Tile neighbor :neighbors){
					if (neighbor.m_tileDef != m_floorTileDef && neighbor.m_tileDef != m_wallTileDef){
						tileCheck->SetType(m_wallTileDef);
					}
				}
			}
		}
	}
}

void MapGenStep_RoomAndPath::GeneratePaths(Map& map)
{
	for (unsigned int roomNum = 0; roomNum < m_rooms.size(); roomNum++){
		unsigned int nextRoomNum = roomNum + 1;
		if (nextRoomNum >= m_rooms.size()){
			if (m_loopPaths){
				nextRoomNum = 0;
			} else {
				break;
			}
		}
		Room* startRoom = m_rooms[roomNum];
		Room* endRoom = m_rooms[nextRoomNum];
		
		//MakePathBetweenRooms(startRoom, endRoom, map);
		MakeDijkstraPathBetweenRooms(startRoom, endRoom, map);
	}
}

void MapGenStep_RoomAndPath::MakeDijkstraPathBetweenRooms(Room * startRoom, Room * endRoom, Map & map)
{
	Heatmap distMap = Heatmap(map.m_dimensions, 9999.f);
	distMap.SetHeat(startRoom->m_center, 0.f);
	map.FillSeededDistanceMap(distMap, map.TileAt(endRoom->m_center), 9999.f, m_straightPathChance, 1.f,1.f,5.f, 5.f);
	//map.FillSeededDistanceMap(distMap, nullptr, 9999.f, m_straightPathChance, 1.f,1.f,9999.f);
	IntVector2 currentCoords = endRoom->m_center;
	int cycles = 0;
	while (currentCoords != startRoom->m_center && cycles < 1000){
		Tile* currentTile = map.TileAt(currentCoords);
		if (currentTile->m_tileDef != m_floorTileDef){
			if (currentTile->m_tileDef == m_wallTileDef){
				currentTile->SetType(m_floorTileDef);
			} else {
				currentTile->SetType(m_pathTileDef);
			}
		}
		currentCoords = distMap.GetCoordsOfMinNeighbor(currentCoords);
		cycles++;
	}
}

void MapGenStep_RoomAndPath::MakePathBetweenRooms(Room * startRoom, Room * endRoom, Map& map)
{
	IntVector2 distBetweenRooms = endRoom->m_center - startRoom->m_center;
	int dirX = 1;
	int dirY = 1;
	if (distBetweenRooms.x < 0){
		distBetweenRooms.x *= -1;
		dirX = -1;
	}
	if (distBetweenRooms.y < 0){
		distBetweenRooms.y *= -1;
		dirY = -1;
	}
	int traveledX = 0;
	int traveledY = 0;
	bool outside = false;
	while (!outside){
		IntVector2 currentCoords = startRoom->m_center + IntVector2(traveledX * dirX, traveledY * dirY);
		Tile* currentTile = map.TileAt(currentCoords);
		if (currentTile->m_tileDef != m_floorTileDef){
			if (currentTile->m_tileDef == m_wallTileDef){
				currentTile->SetType(m_floorTileDef);
			} else {
				currentTile->SetType(m_pathTileDef);
			}
			outside = true;
		}
		if (distBetweenRooms.x > distBetweenRooms.y){
			traveledX++;
		} else {
			traveledY++;
		}
	}
	while (outside && (traveledX < distBetweenRooms.x || traveledY < distBetweenRooms.y)){
		IntVector2 currentCoords = startRoom->m_center + IntVector2(traveledX * dirX, traveledY * dirY);
		Tile* currentTile = map.TileAt(currentCoords);
		if (currentTile->m_tileDef != m_floorTileDef){
			if (currentTile->m_tileDef == m_wallTileDef){
				currentTile->SetType(m_floorTileDef);
				outside = false;
				//break;
			} else {
				currentTile->SetType(m_pathTileDef);
			}
		}
		
		if (distBetweenRooms.x >= distBetweenRooms.y){
			if (traveledX < distBetweenRooms.x && CheckRandomChance(m_straightPathChance)){
				traveledX++;
			} else {
				if (traveledY < distBetweenRooms.y){
					traveledY++;
				} else {
					traveledX++;
				}
			}
		} else {
			if (traveledY < distBetweenRooms.y && CheckRandomChance(m_straightPathChance)){
				traveledY++;
			} else {
				if (traveledX < distBetweenRooms.x){
					traveledX++;
				} else {
					traveledY++;
				}
			}
		}
		
	}
}

Room::Room(IntVector2 center, int roomWidth, int roomHeight)
{
	m_center = center;
	m_width = roomWidth;
	m_height = roomHeight;

	Vector2 floatCenter = m_center.GetVector2();
	IntVector2 halfSizeInts = IntVector2((int) (m_width * .5f), (int) (m_height * .5f));
	m_minCoords = m_center - halfSizeInts;
	m_maxCoords = m_center + halfSizeInts;
}

Room::Room(IntVector2 center, IntVector2 dimensions) 
	: Room(center, dimensions.x, dimensions.y)
{
}

bool Room::DoesRoomOverlap(Room * overlapRoom) const
{
	AABB2 myBounds = GetBoundsWithPadding();
	AABB2 overlapBounds = overlapRoom->GetBoundsWithPadding();
	if (DoAABBsOverlap(myBounds, overlapBounds)){
		return true;
	}
	return false;
}

AABB2 Room::GetBoundsAsAABB2() const
{
	Vector2 mins = m_minCoords.GetVector2();
	Vector2 maxs = m_maxCoords.GetVector2();
	return AABB2(mins, maxs);
}

AABB2 Room::GetBoundsWithPadding(int padding) const
{
	AABB2 bounds = GetBoundsAsAABB2();
	Vector2 paddingDistance = Vector2(1.f,1.f) * (float) padding;
	bounds.mins -=paddingDistance;
	bounds.maxs +=paddingDistance;
	return bounds;
}

MapGenStep_PerlinNoise::MapGenStep_PerlinNoise(const tinyxml2::XMLElement & generationStepElement) 
	: MapGenStep(generationStepElement)
{
	m_ifType					= ParseXmlAttribute(generationStepElement, "ifType", m_ifType);
	m_toType					= ParseXmlAttribute(generationStepElement, "changeToType", m_toType);
	m_noiseRange				= ParseXmlAttribute(generationStepElement, "ifNoise", m_noiseRange);
	FloatRange gridSizeRange	= ParseXmlAttribute(generationStepElement, "gridSize", FloatRange(20.f));
	m_gridSize = gridSizeRange.GetRandomInRange();
	m_seed = (unsigned int) GetRandomIntInRange(0, 2000);
}

void MapGenStep_PerlinNoise::Run(Map & map)
{
	int mapWidth = map.GetWidth();
	int mapHeight = map.GetHeight();
	for (int tileX = 0; tileX < mapWidth; tileX++){
		for (int tileY = 0; tileY < mapHeight; tileY++){
			if (m_mask->IsPointInside(tileX, tileY)){
				Tile* tile = map.TileAt(tileX, tileY);
				if (m_ifType!= nullptr){
					std::string tileName = tile->m_tileDef->m_name;
					std::string ifTypeName = m_ifType->m_name;
					if (tileName == ifTypeName){
						if (CheckPerlinNoiseAtTile(tile)){
							tile->SetType(m_toType);
						}
					}
				} else {
					if (CheckPerlinNoiseAtTile(tile)){
						tile->SetType(m_toType);
					}
				}
			}
		}
	}
}

bool MapGenStep_PerlinNoise::CheckPerlinNoiseAtTile(Tile * tile)
{
	Vector2 tilePos = tile->m_coordinates.GetVector2();
	
	float noiseVal = Compute2dPerlinNoise(tilePos.x, tilePos.y, m_gridSize, 1, .5f, 2.f, true, m_seed);
	if (m_noiseRange.IsValueInRangeInclusive(noiseVal)){
		return true;
	} else {
		return false;
	}
}

MapGenStep_SpawnActor::MapGenStep_SpawnActor(const tinyxml2::XMLElement & generationStepElement)
: MapGenStep(generationStepElement)
{
	m_actorName			= ParseXmlAttribute(generationStepElement, "type", m_actorName);
	m_spawnOnTileDef	= ParseXmlAttribute(generationStepElement, "onTile", m_spawnOnTileDef);
}

void MapGenStep_SpawnActor::Run(Map & map)
{
	if (m_spawnOnTileDef == nullptr){
		m_spawnOnTileDef = map.m_mapDef->m_defaultTile;
	}
	Tile spawnTile = map.GetSpawnTileOfType(m_spawnOnTileDef);
	map.SpawnNewActor(m_actorName, spawnTile.GetCenter());
	for (int i = 0; i < map.m_difficulty; i++){		//random chance to add a multiple of the actor
		if (CheckRandomChance(.05f)){
			map.SpawnNewActor(m_actorName, spawnTile.GetCenter());
		}
	}
	
}

MapGenStep_SpawnItem::MapGenStep_SpawnItem(const tinyxml2::XMLElement & generationStepElement)
	: MapGenStep(generationStepElement)
{
	m_itemName			= ParseXmlAttribute(generationStepElement, "type", m_itemName);
	m_spawnOnTileDef	= ParseXmlAttribute(generationStepElement, "onTile", m_spawnOnTileDef);
}

void MapGenStep_SpawnItem::Run(Map & map)
{
	if (m_spawnOnTileDef == nullptr){
		m_spawnOnTileDef = map.m_mapDef->m_defaultTile;
	}
	Tile spawnTile = map.GetSpawnTileOfType(m_spawnOnTileDef);
	map.SpawnNewItem(m_itemName, spawnTile.GetCenter());
}

MapGenStep_SetSubArea::MapGenStep_SetSubArea(const tinyxml2::XMLElement & generationStepElement)
	: MapGenStep(generationStepElement)
{
}

void MapGenStep_SetSubArea::Run(Map & map)
{
	map.m_generationMask = m_mask;
	map.m_maskType = m_maskType;
}

MapGenStep_EndSubArea::MapGenStep_EndSubArea(const tinyxml2::XMLElement & generationStepElement)
	: MapGenStep(generationStepElement)
{
}

void MapGenStep_EndSubArea::Run(Map & map)
{
	TODO("Support a parent-child relationship for sub-areas - right now can only support one layer deep");
	map.m_generationMask = map.m_fullMap;
	map.m_maskType = AREA_TYPE_RECTANGLE;
}

MapGenStep_SubMap::MapGenStep_SubMap(const tinyxml2::XMLElement & generationStepElement)
	: MapGenStep(generationStepElement)
{
	m_mapType = ParseXmlAttribute(generationStepElement, "map", "NONE");
	
}

void MapGenStep_SubMap::Run(Map & map)
{
	MapDefinition* mapDef = MapDefinition::GetMapDefinition(m_mapType);
	if (mapDef != nullptr){
		for(MapGenStep* step : mapDef->m_generationSteps){
			step->RunIterations(map);
		}
	} else {
		ERROR_AND_DIE("NO MAP DEFINITION");
	}
}

MapGenStep_SpawnDecoration::MapGenStep_SpawnDecoration(const tinyxml2::XMLElement & generationStepElement)
	: MapGenStep(generationStepElement)
{
	m_decoName			= ParseXmlAttribute(generationStepElement, "type", m_decoName);
	m_spawnOnTileDef	= ParseXmlAttribute(generationStepElement, "onTile", m_spawnOnTileDef);
	m_chanceToSpawn		= ParseXmlAttribute(generationStepElement, "chanceToSpawn", m_chanceToSpawn);
	m_spawnType			= ParseXmlAttribute(generationStepElement, "spawnType", m_spawnType);
}

void MapGenStep_SpawnDecoration::Run(Map & map)
{
	if (m_spawnOnTileDef == nullptr){
		m_spawnOnTileDef = map.m_mapDef->m_defaultTile;
	}
	if (m_spawnType == "mutate"){
		RunAsMutate(map);
	} else {
		Tile spawnTile = map.GetSpawnTileOfType(m_spawnOnTileDef);
		map.SpawnNewDecoration(m_decoName, spawnTile.GetCenter());
	}
}

void MapGenStep_SpawnDecoration::RunAsMutate(Map & map)
{
	int mapWidth = map.GetWidth();
	int mapHeight = map.GetHeight();
	for (int tileX = 0; tileX < mapWidth; tileX++){
		for (int tileY = 0; tileY < mapHeight; tileY++){
			if (m_mask->CanDrawOnPoint(tileX, tileY)){
				Tile* tile = map.TileAt(tileX, tileY);
				if (!tile->HasBeenSpawnedOn() && m_spawnOnTileDef!= nullptr){
					std::string tileName = tile->m_tileDef->m_name;
					std::string ifTypeName = m_spawnOnTileDef->m_name;
					if (tileName == ifTypeName){
						if (CheckRandomChance(m_chanceToSpawn)){
							tile->MarkAsSpawned();
							map.SpawnNewDecoration(m_decoName, tile->GetCenter());
						}
					}
				}
			}
		}
	}
}
