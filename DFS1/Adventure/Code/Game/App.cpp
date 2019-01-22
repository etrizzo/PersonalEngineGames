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

	RemoteCommandService::GetInstance()->Startup();
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
	//CommandRegister("profiler", CommandToggleProfiler, "Toggles profiler view");
	//CommandRegister("profiler_report", CommandPrintProfilerReport, "Prints a frame of the profiler to the console", "profiler_report <tree|flat>");
	//CommandRegister("profiler_pause", CommandProfilePause, "Pauses profiling");
	//CommandRegister("profiler_resume", CommandProfileResume, "Resumes profiling");

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
	bool shouldEdge = cmd.GetNextBool();
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



//void CommandToggleProfiler(Command & cmd)
//{
//	UNUSED(cmd);
//	ProfilerVisualizer::GetInstance()->ToggleOpen();
//}
//
//void CommandPrintProfilerReport(Command & cmd)
//{
//	std::string type = cmd.GetNextString();
//	if (type == "flat"){
//		AddProfilerFrameAsFlatToConsole();
//	} else {
//		AddProfilerFrameAsTreeToConsole();
//	}
//}
//
//void CommandProfilePause(Command & cmd)
//{
//	Profiler::GetInstance()->Pause();
//}
//
//void CommandProfileResume(Command & cmd)
//{
//	Profiler::GetInstance()->Resume();
//}
//
//void AddProfilerFrameAsTreeToConsole()
//{
//	profileMeasurement_t* tree = Profiler::GetInstance()->ProfileGetPreviousFrame();
//
//	if (tree != nullptr){
//		ProfilerReport* report = new ProfilerReport();
//		report->GenerateReportTreeFromFrame(tree);
//		if (tree != nullptr){
//			PrintTree(report->m_root);
//		}
//	} else {
//		ConsolePrintf(RGBA::RED, "No profiler frame found - profiling may be disabled in EngineBuildPreferences.hpp");
//	}
//}
//
//void AddProfilerFrameAsFlatToConsole()
//{
//	profileMeasurement_t* tree = Profiler::GetInstance()->ProfileGetPreviousFrame();
//
//	if (tree != nullptr){
//		ProfilerReport* report = new ProfilerReport();
//		report->GenerateReportFlatFromFrame(tree);
//		if (tree != nullptr){
//			PrintTree(report->m_root);
//		}
//	} else {
//		ConsolePrintf(RGBA::RED, "No profiler frame found- profiling may be disabled in EngineBuildPreferences.hpp");
//	}
//}
//
//
//void PrintTree(ProfilerReportEntry * tree, int depth)
//{
//	//	ConsolePrintf("%.64s : %.8fms", tree->m_id, tree->GetTotalElapsedTime());
//	std::string text = FormatProfilerReport(tree, depth);
//	ConsolePrint(text.c_str());
//	for ( ProfilerReportEntry* child : tree->m_children){
//		PrintTree(child, depth + 1);
//	}
//}

