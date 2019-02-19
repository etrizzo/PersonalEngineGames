#include "Game/App.hpp"
#include "Game/Player.hpp"
#include "Game/Adventure.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Map.hpp"
#include "Game/Party.hpp"
#include "Engine/Networking/RemoteCommandService.hpp"
using namespace std;

App::~App()
{
	CommandShutdown();
	delete g_theGame;
	delete g_theAudio;
	delete g_theInput;
	delete g_theRenderer;
	delete g_Window;
	delete g_devConsole;
}

App::App(HINSTANCE applicationInstanceHandle)
{
	//srand(88);
	tinyxml2::XMLDocument gameConfigDoc;
	gameConfigDoc.LoadFile("Data/Data/GameConfig.xml");
	tinyxml2::XMLElement* configElement = gameConfigDoc.FirstChildElement("GameConfig");

	g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*configElement);

	float aspect = g_gameConfigBlackboard.GetValue("windowAspect", 1.f);
	float screenWidth = g_gameConfigBlackboard.GetValue("width", 1000.f);
	float screenHeight = screenWidth / aspect;
	std::string name = g_gameConfigBlackboard.GetValue("appName", "Win32 OpenGL Test App");
	float maxClientFraction = g_gameConfigBlackboard.GetValue("windowHeight", .9f);
	g_Window = new Window(name.c_str(), aspect, applicationInstanceHandle, maxClientFraction);


	m_appTime = 0.f;
	m_bottomLeft = Vector2(0.f,0.f);
	m_topRight = Vector2(screenWidth,screenHeight);
	m_backgroundColor = RGBA(0,0,0,255);

	g_theRenderer = new Renderer();
	g_theInput = new InputSystem();
	g_theAudio = new AudioSystem();
	g_theRenderer->Initialize();
	g_theRenderer->LoadShadersFromFile("shaders.xml");
	g_theRenderer->LoadMaterialsFromFile("materials.xml");
	g_theGame = new Game();
	g_theGame->m_currentState = new GameState_Attract();

	//g_theGame->TransitionToState(new GameState_Attract());
	//g_theGame->Transition();
	g_primaryController = g_theInput->GetController(0);

	
	g_Window->SetInputSystem(g_theInput);
	AABB2 bounds = g_theGame->m_camera->GetBounds();
	g_devConsole = new DevConsole(g_theGame->GetUIBounds());
	g_devConsole->SetRenderer(g_theRenderer);

	ProfilerVisualizer::GetInstance(g_theRenderer, g_theInput, g_theGame->GetUIBounds()); // = new ProfilerVisualizer(g_theRenderer, g_theInput, g_theGame->GetUIBounds());
	RegisterCommands();
	CommandStartup();
	g_theRenderer->SetCamera(g_theGame->m_camera);
	g_theRenderer->SetWindowSize(g_Window);

	RemoteCommandService::GetInstance()->Startup();
	g_theGame->PostStartup();
}


void App::RunFrame()
{
	Profiler::GetInstance()->MarkFrame();
	ClockSystemBeginFrame();
	g_theRenderer->BeginFrame(m_bottomLeft, m_topRight, m_backgroundColor);
	g_theInput->BeginFrame();
	g_theAudio->BeginFrame();
	Update();
	Render();
	g_theAudio->EndFrame();
	g_theInput->EndFrame();
	g_theRenderer->EndFrame(g_Window->m_displayContext);
}

void App::Update()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	m_deltaTime = static_cast<float>(GetCurrentTimeSeconds() - m_appTime);

	//CheckKeys();


	float ds = m_deltaTime;
	ds = GetMasterClock()->GetDeltaSeconds();
	if (g_theInput->IsKeyDown('T')){
		ds/=10.f;
	}
	//ds  = ClampFloat(ds, .005f, .4f);

	if (g_theGame->m_isPaused){
		ds = 0.f;
	}

	g_theGame->Update(ds);
	if (DevConsoleIsOpen()){
		g_devConsole->Update(ds);
	}
	ProfilerVisualizer::GetInstance()->Update();
	m_appTime = GetCurrentTimeSeconds();
	HandleInput();
	g_theRenderer->UpdateClock(ds, m_deltaTime);
}

void App::Render()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	g_theGame->Render();

	PROFILE_PUSH("RenderProfiler");
	if (ProfilerVisualizer::GetInstance()->IsOpen()){
		g_theGame->SetUICamera();
		ProfilerVisualizer::GetInstance()->Render();
	}
	PROFILE_POP();

	if (DevConsoleIsOpen()){
		g_theGame->SetUICamera();
		g_devConsole->Render();
		g_theGame->SetGameCamera();
	}

}

void App::RegisterCommands()
{
	//should this be registered in Command::CommandRegister or nah? uses g_theRenderer which is kinda sorta game specific, idk
	CommandRegister("recompile_shaders", CommandRecompileShaders, "Recompiles all shaders registered to the renderer");		
	CommandRegister("show_stats", CommandShowStats, "Shows the stats for all actors on the current map");
	CommandRegister("toggle_god_mode", CommandToggleGodMode, "Toggles god mode for player");
	CommandRegister("tgm", CommandToggleGodMode, "Toggles god mode for player");
	CommandRegister("set_difficulty", CommandSetDifficulty, "Sets difficulty for current adventure and reloads", "set_difficulty <int>");
	CommandRegister("go_to_map", CommandGoToMap, "Goes to map with specified name", "go_to_map \"MapName\"" );
	CommandRegister("print_map_names", CommandPrintMapNames, "Prints map names for current adventure");
	CommandRegister("win_adventure", CommandWinAdventure, "Automatically wins current adventure");
	CommandRegister("complete_quest", CommandCompleteQuest, "Automatically completes the quest at specified index", "complete_quest <index>");
	CommandRegister("set_speed", CommandSetSpeed, "Sets player walking speed", "set_speed <int>");
	CommandRegister("spawn_actor", CommandSpawnActor, "Spawns an actor of the specified type near the player", "spawn_actor \"Actor Name\"");
	CommandRegister("spawn_item", CommandSpawnItem, "Spawns an item of the specified type near the player", "spawn_item \"Item Name\"");
	
	CommandRegister("toggle_edges", CommandToggleEdges, "Sets whether or not the map should render edge tiles", "toggle_edges <bool>");
	CommandRegister("edge_remove_invalid", CommandRemoveInvalidTiles, "Removes un-edgable tiles from the map");
	CommandRegister("edge_tufts", CommandAddTufts, "Adds \"tufts\" on high level tiles surrounded by lower-level tiles");
	CommandRegister("edge_shore", CommandEdgeShore, "Edges the shoreline");
	CommandRegister("edge_high", CommandEdgeHighPriority, "Edges high-to-low ground levels");
	CommandRegister("edge_low", CommandEdgeLowPriority, "Edges different tiles of the same ground level (dirt -> sand, eg.)");
	CommandRegister("edge_map", CommandEdgeTiles, "Goes through the entire tile-edging process.");
	
	CommandRegister("reroll_appearance", CommandRerollPlayerAppearance, "Rerolls the sprite/portrait appearance for the current player");
	

	//thesis
	CommandRegister("set_seed", CommandSetSeed, "Sets the random seed for the program", "set_seed <int>");
	CommandRegister("output_graph_to_console", CommandPrintGraph, "Prints current graph as text");
	CommandRegister("generate_graph", CommandGenerateGraph, "Re-generates story graph");
	CommandRegister("generate_skeleton", CommandGenerateSkeleton, "Generates n plot nodes for a graph", "generate_skeleton, <int>");
	CommandRegister("generate_details", CommandAddDetails, "Generates n detail nodes", "generate_details <int>");
	CommandRegister("find_path", CommandFindPath, "Finds the shortest path in the graph");
	CommandRegister("print_story", CommandPrintStory, "Prints the current story to the screen");
	CommandRegister("add_branches", CommandFindBranches, "Adds branches to the graph");
	CommandRegister("set_branch_chance", CommandSetBranchChance, "Sets the chance to branch nodes on failure to place node.", "set_branch_chance <float 0-1>");

	CommandRegister("generate_pairs", CommandGeneratePairs, "Generates a plot node and outcome node as a pair", "generate_pairs <numPairs>");
	CommandRegister("reset_graph", CommandResetGraph, "resets the graph to start->end");

	CommandRegister("read_default_data", CommandReadDefaultData, "reads the default data set into the graph and resets it.");
	CommandRegister("read_murder_data", CommandReadMurderData, "reads the murder mystery data set into the graph and resets it.");
	CommandRegister("read_dialogue_data", CommandReadDialogueData, "reads the dialogue data set into the graph and resets it.");

	CommandRegister("progress_story", CommandProgressStory, "Progresses the story graph for all villages");
}
void App::HandleInput()
{
	if (g_theInput->WasKeyJustPressed(192)){		//the ` key
		if (!DevConsoleIsOpen()){
			g_devConsole->Open();
		} else {
			g_devConsole->Close();
		}
	}
	if (g_theInput->WasKeyJustPressed(VK_F2)){
		ProfilerVisualizer::GetInstance()->ToggleOpen();
	}
	if (!DevConsoleIsOpen()){
		//universal keys
		//if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		//	CommandRun("quit");
		//}

		//have game handle input
		if (!ProfilerVisualizer::GetInstance()->IsControllingInput()){
			g_theGame->HandleInput();
		}
		if (ProfilerVisualizer::GetInstance()->IsOpen()){
			ProfilerVisualizer::GetInstance()->HandleInput(g_theInput);
		}

	}
}


bool App::IsQuitting()
{
	return g_Window->m_isQuitting;
}


void CommandRecompileShaders(Command & cmd)
{
	UNUSED(cmd);		//may want to add support for specifying the shader root later
	g_theRenderer->RecompileAllShaders();
	ConsolePrintf(RGBA::CYAN, "All shaders recompiled");
}

void CommandShowStats(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->ShowActorStats();
}

void CommandToggleGodMode(Command & cmd)
{
	UNUSED(cmd);
	if (!g_theGame->m_party->IsEmpty()){
		g_theGame->m_party->GetPlayerCharacter()->m_godMode = !g_theGame->m_party->GetPlayerCharacter()->m_godMode;
		ConsolePrintf(("God mode is: " + std::to_string(g_theGame->m_party->GetPlayerCharacter()->m_godMode)).c_str());
	} else {
		ConsolePrintf(RGBA::RED, "No current adventure - start the game");
	}
}

void CommandSetDifficulty(Command & cmd)
{
	g_theGame->DebugSetDifficulty(cmd.GetNextInt());
	g_devConsole->Close();
}

void CommandGoToMap(Command & cmd)
{
	g_theGame->GoToMap(cmd.GetNextString());
	g_devConsole->Close();
}

void CommandPrintMapNames(Command & cmd)
{
	UNUSED(cmd);
	if (g_theGame->m_currentState->m_currentAdventure != nullptr){
		for(Map* m : g_theGame->m_currentState->m_currentAdventure->m_mapsByIndex){
			ConsolePrintf(("  " + m->GetName() ).c_str());
		}
	} else {
		ConsolePrintf(RGBA::RED, "No current adventure - start the game");
	}
}

void CommandWinAdventure(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->DebugWinAdventure();
	g_devConsole->Close();
}

void CommandCompleteQuest(Command & cmd)
{
	g_theGame->DebugCompleteQuest(cmd.GetNextInt());
	g_devConsole->Close();
}

void CommandSetSpeed(Command & cmd)
{
	int speed = cmd.GetNextInt();
	g_theGame->DebugSetPlayerSpeed(speed);
}

void CommandSpawnActor(Command & cmd)
{
	std::string actorName = cmd.GetNextString();
	g_theGame->DebugSpawnActor(actorName);
}

void CommandSpawnItem(Command & cmd)
{
	std::string actorName = cmd.GetNextString();
	g_theGame->DebugSpawnItem(actorName);
}

void CommandToggleEdges(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_renderingEdges = (!g_theGame->m_renderingEdges);
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->ReCreateRenderable(g_theGame->m_renderingEdges);

}

void CommandRemoveInvalidTiles(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->RemoveInvalidTiles();
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->RemoveInvalidTiles();
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->ReCreateRenderable(g_theGame->m_renderingEdges);
}

void CommandAddTufts(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->AddTufts();
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->ReCreateRenderable(g_theGame->m_renderingEdges);
}

void CommandEdgeShore(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->EdgeShoreline();
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->ReCreateRenderable(g_theGame->m_renderingEdges);
}

void CommandEdgeHighPriority(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->EdgeGrassToDirt();
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->ReCreateRenderable(g_theGame->m_renderingEdges);
}

void CommandEdgeLowPriority(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->EdgeLowPriority();
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->ReCreateRenderable(g_theGame->m_renderingEdges);
}

void CommandEdgeTiles(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->EdgeTilesThreeSteps();
	g_theGame->m_currentState->m_currentAdventure->m_currentMap->ReCreateRenderable(g_theGame->m_renderingEdges);
}

void CommandRerollPlayerAppearance(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->DebugRerollPlayerAppearance();
}

void CommandSetSeed(Command & cmd)
{
	int seed = cmd.GetNextInt();
	if (seed == 0){
		seed = (int) time(0);
	}
	srand((unsigned int) seed);
	ConsolePrintf("Setting seed to %i", seed);
}

void CommandPrintGraph(Command & cmd)
{
	UNUSED(cmd);
	std::string graphString = g_theGame->m_graph->ToString();
	ConsolePrintf(graphString.c_str());
}

void CommandGenerateGraph(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->GenerateGraph();
}

void CommandFindPath(Command & cmd)
{
	int seed = cmd.GetNextInt();
	if (seed != 0){
		srand((unsigned int) seed);
	} else {
		unsigned int randoSeed = (unsigned int) time(0);
		srand(randoSeed);
		ConsolePrintf("Calculating path with seed %i", randoSeed);
	}
	//g_theGame->m_graph->FindPath(RandomPathHeuristic);
	g_theGame->m_graph->FindPath(CalculateChanceHeuristic);
}


void CommandPrintStory(Command& cmd)
{
	UNUSED(cmd);
	g_theGame->m_graph->PrintPath();
}

void CommandFindBranches(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_graph->IdentifyBranchesAndAdd();
}

void CommandSetBranchChance(Command & cmd)
{
	UNUSED(cmd);
	float chance = cmd.GetNextFloat();
	g_theGame->m_graph->SetBranchChance(chance);
	ConsolePrintf(RGBA::YELLOW, "Changed branch chance on generation to %.2f", chance);
}

void CommandResetGraph(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->ClearGraph();
	g_theGame->m_graph->GenerateStartAndEnd();
}

void CommandGenerateSkeleton(Command & cmd)
{
	g_theGame->ClearGraph();
	int numToGenerate = cmd.GetNextInt();
	if (numToGenerate == 0){
		numToGenerate = NUM_PLOT_NODES_TO_GENERATE;
	}
	numToGenerate+=2;	//for start and end nodes
	g_theGame->GeneratePlotNodes(numToGenerate);
}

void CommandAddDetails(Command & cmd)
{
	int numToGenerate = cmd.GetNextInt();
	if (numToGenerate == 0){
		numToGenerate = NUM_DETAIL_NODES_TO_GENERATE;
	}
	g_theGame->GenerateDetailNodes(numToGenerate);
}

void CommandGeneratePairs(Command & cmd)
{
	int numToGenerate = cmd.GetNextInt();
	if (numToGenerate == 0){
		numToGenerate = NUM_NODE_PAIRS_TO_GENERATE;
	}
	int seed = cmd.GetNextInt();
	if (seed > 0){
		srand((unsigned int) seed);
	} else if (seed == -1){
		unsigned int randoSeed = (unsigned int) time(0);
		srand(randoSeed);
		ConsolePrintf("Generating pairs with seed %i", randoSeed);
	}
	g_theGame->GenerateNodePairs(numToGenerate);
}

void CommandReadDefaultData(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->InitGraphDefault();
	/*g_theGame->ClearGraph();

	g_theGame->ResetGraphData();
	g_theGame->ReadPlotNodes("Data/Data/PlotGrammars.xml");
	g_theGame->ReadOutcomeNodes("Data/Data/DetailGrammars.xml");
	g_theGame->ReadCharacters("Data/Data/Characters.xml");
	g_theGame->InitCharacterArray();

	g_theGame->m_graph->GenerateStartAndEnd();*/
	ConsolePrintf("Default data loaded.");
}

void CommandReadMurderData(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->InitGraphMurder();
	/*g_theGame->ClearGraph();

	g_theGame->ResetGraphData();
	g_theGame->ReadPlotNodes("Data/Data/MurderMystery_PlotGrammars.xml");
	g_theGame->ReadOutcomeNodes("Data/Data/MurderMystery_OutcomeGrammars.xml");
	g_theGame->ReadCharacters("Data/Data/MurderMystery_Characters.xml");
	g_theGame->InitCharacterArray();

	g_theGame->m_graph->GenerateStartAndEnd();*/
	ConsolePrintf("Murder mystery data loaded.");
}

void CommandReadDialogueData(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->InitGraphDialogue();
	ConsolePrintf("Dialogue data loaded.");
}

void CommandProgressStory(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->ProgressStories();
}
