#include "Game/App.hpp"
#include "Game/Player.hpp"
#include "Game/Adventure.hpp"
#include "Engine/Renderer/Camera.hpp"
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
	g_theGame = new Game();

	g_theGame->m_transitionToState = STATE_ATTRACT;
	g_theGame->Transition();
	g_primaryController = g_theInput->GetController(0);

	
	g_Window->SetInputSystem(g_theInput);
	AABB2 bounds = g_theGame->m_camera->GetBounds();
	g_devConsole = new DevConsole(g_theGame->m_camera->GetBounds());
	g_devConsole->SetRenderer(g_theRenderer);
	RegisterCommands();
	CommandStartup();
	g_theRenderer->SetCamera(g_theGame->m_camera);
}


void App::RunFrame()
{
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

	CheckKeys();


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
}

void App::Render()
{

	g_theGame->Render();
	if (DevConsoleIsOpen()){
		g_devConsole->Render();
	}

}

void App::RegisterCommands()
{
	//should this be registered in Command::CommandRegister or nah? uses g_theRenderer which is kinda sorta game specific, idk
	CommandRegister("recompile_shaders", CommandRecompileShaders, "Recompiles all shaders registered to the renderer");		
}
//
//App::App(HDC dispContext)
//{
//	m_appTime = 0.f;
//	m_isQuitting = false;
//	m_displayContext = dispContext;
//	m_bottomLeft = Vector2(0.f,0.f);
//	m_topRight = Vector2(40.f,40.f);
//	m_backgroundColor = RGBA(0,0,0,255);
//
//	tinyxml2::XMLDocument gameConfigDoc;
//	gameConfigDoc.LoadFile("Data/Data/GameConfig.xml");
//	tinyxml2::XMLElement* configElement = gameConfigDoc.FirstChildElement("GameConfig");
//
//	g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*configElement);
//	g_theRenderer = new Renderer();
//	g_theInput = new InputSystem();
//	g_theAudio = new AudioSystem();
//	g_theGame = new Game();
//	g_theGame->m_transitionToState = STATE_ATTRACT;
//	g_theGame->Transition();
//	//g_theGame->m_currentState = STATE_ATTRACT;
//	g_primaryController = g_theInput->GetController(0);
//	//g_theGame->StartAdventure("Balrog");
//	m_isQuitting = false;
//	m_displayContext = dispContext;
//	g_theRenderer->Initialize();
//}
//
//
//void App::RunFrame()
//{
//	g_theRenderer->BeginFrame(m_bottomLeft, m_topRight, m_backgroundColor);
//	g_theInput->BeginFrame();
//	g_theAudio->BeginFrame();
//	Update();
//	Render();
//	g_theAudio->EndFrame();
//	g_theInput->EndFrame();
//	g_theRenderer->EndFrame(m_displayContext);
//}
//
//void App::Update()
//{
//	m_deltaTime = static_cast<float>(GetCurrentTimeSeconds() - m_appTime);
//
//	CheckKeys();
//
//
//	float ds = m_deltaTime;
//	if (g_theInput->IsKeyDown('T')){
//		ds/=10.f;
//	}
//	ds  = ClampFloat(ds, 0.f, .4f);
//
//	if (g_theGame->m_isPaused){
//		ds = 0.f;
//	}
//
//	g_theGame->Update(ds);
//	m_appTime = GetCurrentTimeSeconds();
//}
//
//void App::Render()
//{
//
//	g_theGame->Render();
//
//}



void App::CheckKeys()
{

	if (g_theInput->WasKeyJustPressed(192)){		//the ` key
		if (!DevConsoleIsOpen()){
			g_devConsole->Open();
		} else {
			g_devConsole->Close();
		}
	}

	if (g_primaryController == nullptr){
		g_theInput->GetController(0);
	}

	if (g_theInput->WasKeyJustPressed(VK_SPACE)){
		SoundID testSound = g_theAudio->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
		g_theAudio->PlaySound(testSound);
	}

	switch (g_theGame->m_currentState){
	case STATE_ATTRACT:
		CheckAttractKeys();
		break;
	case STATE_PLAYING:
		CheckPlayingKeys();
		break;
	case STATE_PAUSED:
		CheckPauseKeys();
		break;
	case STATE_INVENTORY:
		CheckInventoryKeys();
		break;
	case STATE_VICTORY:
		CheckVictoryKeys();
		break;
	case STATE_DEFEAT:
		CheckDefeatKeys();
	case STATE_MAPMODE:
		CheckMapmodeKeys();
		break;
	default:
		if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
			CommandRun("quit");
		}
		break;
	}

	/*if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		if (g_theGame->m_currentState == STATE_MAPMODE){
			g_theGame->StartStateTransition(STATE_ATTRACT, 0.f);
		} else {
			m_isQuitting = true;
		}
	}

	CheckArrowKeys();

	if (g_theInput->WasKeyJustPressed('P') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START)){
		if (g_theGame->m_currentState == STATE_ATTRACT){
			g_theGame->StartStateTransition(STATE_PLAYING, 1.f);
			g_theGame->StartAdventure("Balrog");
		}
		if (g_theGame->m_currentState == STATE_PLAYING){
			g_theGame->StartStateTransition(STATE_PAUSED, .15f, RGBA(0,0,0,100));
		}
		if (g_theGame->m_currentState == STATE_PAUSED){
			g_theGame->StartStateTransition(STATE_PLAYING, .15f, RGBA(0,0,0,100));
		}
	}

	if (g_theInput->WasKeyJustPressed('I') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_BACK)){
		if (g_theGame->m_currentState == STATE_ATTRACT){
			m_isQuitting = true;
		}
		if (g_theGame->m_currentState == STATE_PLAYING){
			g_theGame->StartStateTransition(STATE_INVENTORY, .15f, RGBA(0,0,0,100));
		}
		if (g_theGame->m_currentState == STATE_PAUSED){
			g_theGame->StartStateTransition(STATE_INVENTORY, 0.f, RGBA(0,0,0,100));
		}
		if (g_theGame->m_currentState == STATE_INVENTORY){
			g_theGame->StartStateTransition(STATE_PLAYING, .15f, RGBA(0,0,0,100));
		}
	}

	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}

	

	

	if (g_theInput->WasKeyJustPressed('M')){
		if (g_theGame->m_currentState == STATE_ATTRACT){
			g_theGame->StartStateTransition(STATE_MAPMODE, 1.f);
			g_theGame->StartAdventure("Balrog");
			g_theGame->m_fullMapMode = true;
		} else if (g_theGame->m_currentState != STATE_MAPMODE){
			g_theGame->ToggleState(g_theGame->m_fullMapMode);
		}
	}

	if (g_theGame->m_currentState == STATE_MAPMODE){
		if (g_theInput->WasKeyJustPressed(VK_F5)){
			g_theGame->m_currentAdventure->RegenerateCurrentMap();
		}
	}*/

	
}

void App::CheckAttractKeys()
{
	if (WasStartJustPressed()){
		g_theGame->StartStateTransition(STATE_PLAYING, 1.f);
		g_theGame->StartAdventure("Balrog");
	}

	if (WasExitJustPressed()){
		CommandRun("quit");
	}

	if (g_theInput->WasKeyJustPressed('M') || g_primaryController->WasButtonJustPressed(XBOX_Y)){
		g_theGame->StartStateTransition(STATE_MAPMODE, 1.f);
		g_theGame->StartAdventure("Balrog");
		g_theGame->m_fullMapMode = true;
	}

	CheckArrowKeys();


}

void App::CheckPlayingKeys()
{
	if (WasPauseJustPressed() || g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		g_theGame->StartStateTransition(STATE_PAUSED, .15f, RGBA(0,0,0,200));
	}
	if (g_theInput->WasKeyJustPressed('I') || g_primaryController->WasButtonJustPressed(XBOX_BACK)){
		g_theGame->StartStateTransition(STATE_INVENTORY, .15f, RGBA(0,0,0,200));
	}

	if (g_theInput->WasKeyJustPressed('M')){
		g_theGame->ToggleState(g_theGame->m_fullMapMode);		//change later to open map screen
	}

	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}
	
}

void App::CheckPauseKeys()
{
	if (WasPauseJustPressed() || g_primaryController->WasButtonJustPressed(XBOX_B)){
		g_theGame->StartStateTransition(STATE_PLAYING, .15f, RGBA(0,0,0,200));
	}
	if (g_theInput->WasKeyJustPressed('I') || g_primaryController->WasButtonJustPressed(XBOX_BACK)){
		g_theGame->StartStateTransition(STATE_INVENTORY, .15f, RGBA(0,0,0,200));
	}

	CheckArrowKeys();

	if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		g_theGame->StartStateTransition(STATE_ATTRACT, .15f, RGBA(0,0,0,200));
	}
}

void App::CheckInventoryKeys()
{
	if (g_theInput->WasKeyJustPressed('I') || g_primaryController->WasButtonJustPressed(XBOX_BACK) || g_primaryController->WasButtonJustPressed(XBOX_B)){
		g_theGame->StartStateTransition(STATE_PLAYING, .15f, RGBA(0,0,0,200));
	}
	if (WasPauseJustPressed()){
		g_theGame->StartStateTransition(STATE_PAUSED, .15f, RGBA(0,0,0,200));
	}
	CheckArrowKeys();
	if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		g_theGame->StartStateTransition(STATE_ATTRACT, .15f, RGBA(0,0,0,200));
	}
}

void App::CheckMapmodeKeys()
{
	if (g_theInput->WasKeyJustPressed(VK_F5) || g_primaryController->WasButtonJustPressed(XBOX_Y)){
		g_theGame->m_currentAdventure->RegenerateCurrentMap();
	}

	if (WasBackJustPressed()){
		g_theGame->StartStateTransition(STATE_ATTRACT, 0.f);
	}
	
	if (WasRightJustPressed()){
		g_theGame->LookAtNextMap(1);
	}
	if (WasLeftJustPressed()){
		g_theGame->LookAtNextMap(-1);
	}
}

void App::CheckVictoryKeys()
{
	if (WasStartJustPressed()){
		g_theGame->StartStateTransition(STATE_ATTRACT, 0.f);
	}
}

void App::CheckDefeatKeys()
{
	if (g_theInput->WasKeyJustPressed('N') || g_primaryController->WasButtonJustPressed(XBOX_START)){
		g_theGame->m_player->Respawn();
	}
}

//'back' functionality - esc, b
bool App::WasBackJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_ESCAPE) || g_primaryController->WasButtonJustPressed(XBOX_B);
}

//BROAD 'start' functionality - p, enter, start, a
bool App::WasStartJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RETURN) || g_theInput->WasKeyJustPressed('P') || g_primaryController->WasButtonJustPressed(XBOX_START) || g_primaryController->WasButtonJustPressed(XBOX_A);
}

//enter or a ONLY
bool App::WasSelectJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RETURN) || g_primaryController->WasButtonJustPressed(XBOX_A);
}

//p or start ONLY
bool App::WasPauseJustPressed()
{
	return g_theInput->WasKeyJustPressed('P') || g_primaryController->WasButtonJustPressed(XBOX_START);
}

//ESC or back only
bool App::WasExitJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_ESCAPE);
}

bool App::WasUpJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_UP) || g_primaryController->WasButtonJustPressed(XBOX_D_UP);
}

bool App::WasDownJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_DOWN) || g_primaryController->WasButtonJustPressed(XBOX_D_DOWN);
}

bool App::WasRightJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RIGHT) || g_primaryController->WasButtonJustPressed(XBOX_D_RIGHT);
}

bool App::WasLeftJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_LEFT) || g_primaryController->WasButtonJustPressed(XBOX_D_LEFT);
}

void App::CheckArrowKeys()
{
	if (WasUpJustPressed()){
		g_theGame->UpdateMenuSelection(-1);
	}
	if (WasDownJustPressed()){
		g_theGame->UpdateMenuSelection(1);
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

