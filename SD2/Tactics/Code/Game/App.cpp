#include "Game/App.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Clock.hpp"
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
	m_nearBottomLeft = g_gameConfigBlackboard.GetValue("bottomLeft", Vector3::ZERO);
	float screenWidth = g_gameConfigBlackboard.GetValue("width", 1000.f);
	float screenHeight = screenWidth / aspect;
	std::string name = g_gameConfigBlackboard.GetValue("appName", "Win32 OpenGL Test App");
	float maxClientFraction = g_gameConfigBlackboard.GetValue("windowHeight", .9f);
	g_Window = new Window(name.c_str(), aspect, applicationInstanceHandle, maxClientFraction);

	m_appTime = 0.f;
	m_farTopRight = m_nearBottomLeft + Vector3(screenWidth,screenHeight, 100.f);
	m_backgroundColor = RGBA(0,0,0,255);

	g_theRenderer = new Renderer();
	g_theInput = new InputSystem();
	g_theAudio = new AudioSystem();
	g_theRenderer->Initialize();
	g_theGame = new Game();
	g_theGame->InitCameras(m_nearBottomLeft, m_farTopRight);
	
	g_Window->SetInputSystem(g_theInput);
	AABB2 UIBounds = g_theGame->m_uiCamera->GetBounds();
	g_devConsole = new DevConsole(UIBounds);
	g_devConsole->SetRenderer(g_theRenderer);
	RegisterCommands();
	CommandStartup();
}


void App::RunFrame()
{
	GetMasterClock()->BeginFrame();
	g_theGame->SetMainCamera();
	g_theRenderer->BeginFrame(m_nearBottomLeft, m_farTopRight, m_backgroundColor);
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

	


	float ds = m_deltaTime;
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
		g_theGame->SetMainCamera();
	}
	

}

void App::RegisterCommands()
{
	//should this be registered in Command::CommandRegister or nah? uses g_theRenderer which is kinda sorta game specific, idk
	CommandRegister("recompile_shaders", CommandRecompileShaders, "Recompiles all shaders registered to the renderer");		
	CommandRegister("spawn_actor", CommandSpawnActor, "Spawns actor of type at IntVec2(x,y) with faction number f", "spawn_actor <x> <y> <type=warrior> <f=0>");
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
		if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
			CommandRun("quit");
		}

		//have game handle input
		g_theGame->HandleInput();

	}
}


bool App::IsQuitting()
{
	return g_Window->m_isQuitting;
}

//spawn_actor <x> <y> <type=warrior> <f=0>
void CommandSpawnActor(Command & cmd)
{
	int x = cmd.GetNextInt();
	int y = cmd.GetNextInt();
	std::string type = cmd.GetNextString();
	int faction = cmd.GetNextInt();

	
	if (g_theGame->SpawnActor(IntVector2(x,y), type, faction)){
		ConsolePrintf("spawning actor at (%i, %i) of type %s and faction %i", x, y, type.c_str(), faction);
	} else {
		ConsolePrintf(RGBA::RED, "Error spawning actor at (%i, %i) of type %s and faction %i", x, y, type.c_str(), faction);
	}
}

void CommandRecompileShaders(Command & cmd)
{
	UNUSED(cmd);		//may want to add support for specifying the shader root later
	g_theRenderer->RecompileAllShaders();
	ConsolePrintf(RGBA::CYAN, "All shaders recompiled");
}
