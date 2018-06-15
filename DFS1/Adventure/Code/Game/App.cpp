#include "Game/App.hpp"
#include "Game/Player.hpp"
#include "Game/Adventure.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Map.hpp"
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
	tinyxml2::XMLDocument gameConfigDoc;
	gameConfigDoc.LoadFile("Data/Data/GameConfig.xml");
	tinyxml2::XMLElement* configElement = gameConfigDoc.FirstChildElement("GameConfig");

	g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*configElement);

	float aspect = g_gameConfigBlackboard.GetValue("windowAspect", 1.f);
	float screenWidth = g_gameConfigBlackboard.GetValue("width", 1000.f);
	float screenHeight = screenWidth / aspect;
	std::string name = g_gameConfigBlackboard.GetValue("appName", "Win32 OpenGL Test App");
	g_Window = new Window(name.c_str(), aspect, applicationInstanceHandle);

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
	RegisterCommands();
	CommandStartup();
	g_theRenderer->SetCamera(g_theGame->m_camera);
}


void App::RunFrame()
{
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
	m_deltaTime = static_cast<float>(GetCurrentTimeSeconds() - m_appTime);

	//CheckKeys();


	float ds = m_deltaTime;
	ds = GetMasterClock()->GetDeltaSeconds();
	if (g_theInput->IsKeyDown('T')){
		ds/=10.f;
	}
	ds  = ClampFloat(ds, .005f, .4f);

	if (g_theGame->m_isPaused){
		ds = 0.f;
	}

	g_theGame->Update(ds);
	if (DevConsoleIsOpen()){
		g_devConsole->Update(ds);
	}
	m_appTime = GetCurrentTimeSeconds();
	HandleInput();
	g_theRenderer->UpdateClock(ds, m_deltaTime);
}

void App::Render()
{

	g_theGame->Render();
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

	if (!DevConsoleIsOpen()){
		//universal keys
		//if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		//	CommandRun("quit");
		//}

		//have game handle input
		g_theGame->HandleInput();

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
	if (g_theGame->m_player != nullptr){
		g_theGame->m_player->m_godMode = !g_theGame->m_player->m_godMode;
		ConsolePrintf(("God mode is: " + std::to_string(g_theGame->m_player->m_godMode)).c_str());
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

