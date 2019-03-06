#include "Village.hpp"
#include "Game/VillageDefinition.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/StoryGraph.hpp"

Village::Village(VillageDefinition * definition, Map * map, int numActorsToSpawn)
{
	m_villageName = definition->GetRandomVillageNameAndCrossOff();
	m_definition = definition;
	m_faction = definition->GetRandomFaction();
	m_map = map;

	m_center = m_map->m_generationMask->GetCenter();
	//copy the area in case we wanna do stuff with it later (village owns this mask)
	m_area = m_map->m_generationMask->Clone();
	m_villageGraph = new StoryGraph();

	SpawnResidents(numActorsToSpawn);		//creates the actors
	ReadGraphData();						//load the graph data
	ConnectResidentsToGraphCharacters();	//sync graph characters with the residents of the villages
	GenerateGraph();						//Actually generate the nodes of the graph
	SetResidentDialogues();					//Set the residents' dialogs based on the graph state
}

Village::~Village()
{
	delete m_area;
	delete m_villageGraph;
	//doesn't OWN it's residents, they still belong to the map. Just references.
}

void Village::ProgressVillageStoryTime()
{
	StoryNode* start = m_currentEdge->GetStart();
	if (start->m_data->GetProgressionType() == PROGRESSION_TIME) {
		//progress to the next node
		StoryNode* node = m_currentEdge->GetEnd();

		if (node != m_villageGraph->GetEnd() && node->m_data->GetProgressionType() == PROGRESSION_TIME) {
			int edgeIndex = GetRandomIntLessThan(node->m_outboundEdges.size());
			m_currentEdge = node->m_outboundEdges[edgeIndex];

			//do whatever to update the state of the story according to quests & shit
			SetResidentDialogues();
		}
	}
}

void Village::SpawnResidents(int numToSpawn)
{
	TileDefinition* spawnTileDefinition = m_definition->m_baseTileDefinition ;
	for (int i = 0; i < numToSpawn; i++)
	{
		if (spawnTileDefinition == nullptr){
			spawnTileDefinition = m_map->m_mapDef->m_defaultTile;
		}
		Tile* spawnTile = nullptr;
		
		spawnTile = GetSpawnTileOfType(spawnTileDefinition);
		
		if (spawnTile != nullptr){
			Actor* newActor = m_map->SpawnNewActor(m_definition->GetRandomResidentDefinition(), spawnTile->GetCenter());
			newActor->SetVillage(this);
			m_residents.push_back(newActor);
		}
	}
}

void Village::ReadGraphData()
{
	std::string dataSet = m_definition->GetRandomDataSetName();
	m_villageGraph->LoadDataSet(dataSet);
}

void Village::ConnectResidentsToGraphCharacters()
{
	m_villageGraph->SelectCharactersForGraph(m_residents.size());
	//tier 1 - eventually, pick the best villager for each role
	for (int i = 0; i < (int) m_villageGraph->m_characters.size(); i++)
	{
		if (i < (int) m_residents.size())
		{
			m_residents[i]->AssignStoryCharacter(m_villageGraph->m_characters[i]);
		}
	}
}

void Village::GenerateGraph()
{
	bool generated = false;
	while (!generated){
		m_villageGraph->Clear();
		m_villageGraph->RunGenerationPairs(NUM_NODE_PAIRS_TO_GENERATE);
		generated = m_villageGraph->AddEndingsToGraph();
	}

	m_currentEdge = m_villageGraph->GetStart()->m_outboundEdges[0];
}

void Village::SetResidentDialogues()
{
	for (Actor* resident : m_residents)
	{
		if (resident->m_storyCharacter != nullptr){
			CharacterState* resState = m_currentEdge->GetCost()->GetCharacterStateForCharacter(resident->m_storyCharacter);
			//copy the state of the character
			resident->m_tags = resState->m_tags;
			
			//check the story tags to see if this character currently has a role
			TagSet storyTags = m_currentEdge->GetCost()->m_storyTags;
			std::string residentRole = "";
			for (int i = 0; i < (int) storyTags.m_tags.size(); i++)
			{
				if (storyTags.m_tags[i].GetType() == "character" )
				{
					if (storyTags.m_tags[i].GetValue() == resident->m_name){
						residentRole = storyTags.m_tags[i].GetName();
					}
				}
			}
			resident->m_roleInStory = residentRole;

			resident->SetDialogFromState();
		}
	}
}

Tile * Village::GetSpawnTileOfType(TileDefinition * def)
{
	IntVector2 pos = m_area->GetRandomPointInArea();
	Tile* tile = m_map->TileAt(pos);
	int tries = 0;
	while (tries < 1000 && ((tile == nullptr) || ((tile->m_tileDef != def) || tile->HasBeenSpawnedOn()))){
		pos = m_area->GetRandomPointInArea();
		tile = m_map->TileAt(pos);
		tries++;
	}

	if (tile != nullptr){
		if ((tile->m_tileDef != def) || tile->HasBeenSpawnedOn()) {
			return nullptr;
		} else {
			m_map->MarkTileForSpawn(pos);
		}
	}
	return tile;
}
