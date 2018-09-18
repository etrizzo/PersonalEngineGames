#include "Game/App.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Networking/RemoteCommandService.hpp"
#include "Engine/Networking/Net.hpp"
using namespace std;


App::App(){
	
	m_appTime = GetCurrentTimeSeconds();
	m_isQuitting = false;
	//m_theGame = new Game();
	m_appTime = 0.f;
	
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
	m_bottomLeft = Vector2(SCREEN_MIN,SCREEN_MIN);
	m_topRight = Vector2(SCREEN_MAX,SCREEN_MAX);
	m_backgroundColor = RGBA(0,0,0,255);

	g_theRenderer = new Renderer();
	g_theInput = new InputSystem();
	g_theAudio = new AudioSystem();
	g_theRenderer->Initialize();
	m_theGame = new Game();

	
	g_Window->SetInputSystem(g_theInput);
	AABB2 UIBounds = m_theGame->m_camera->GetBounds();
	Net::Startup();
	g_devConsole = new DevConsole(UIBounds);
	g_devConsole->PostStartup();
	g_devConsole->SetRenderer(g_theRenderer);
	CommandStartup();
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
	Sleep(0);
}

void App::Update()
{
	m_deltaTime = static_cast<float>(GetCurrentTimeSeconds() - m_appTime);

	//check keys
	CheckKeys();
	if (g_theInput->IsKeyDown('T')){
		m_deltaTime/=10.f;
	}
	m_theGame->Update(m_deltaTime);
	if (DevConsoleIsOpen()){
		g_devConsole->Update(m_deltaTime);
	}
	m_appTime = GetCurrentTimeSeconds();
}

void App::Render()
{
	m_theGame->Render();

	if (DevConsoleIsOpen()){
		g_devConsole->Render();
	}

}


void App::CheckKeys()
{
	if (g_theInput->WasKeyJustPressed(192)){		//the ` key
		if (!DevConsoleIsOpen()){
			g_devConsole->Open();
		} else {
			g_devConsole->Close();
		}
	}

	if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		m_isQuitting = true;
	}

	//I spawns asteroid
	if (g_theInput->WasKeyJustPressed('I')){
		m_theGame->AddAsteroid();
	}
	if (g_theInput->WasKeyJustPressed('O')){
		m_theGame->DeleteAsteroid();
	}

	if (g_theInput->WasKeyJustPressed('P')){
		m_theGame->TogglePause();
	}

	if (g_theInput->WasKeyJustPressed('N')){
		m_theGame->SpawnShip();
	}
	
	if (g_theInput->WasKeyJustPressed(VK_F1)){
		m_theGame->ToggleDevMode();
	}

	XboxController* c = g_theInput->GetController(m_theGame->m_ship->m_controllerID);
	bool aButton = c->WasButtonJustPressed(XBOX_A);

	if (g_theInput->WasKeyJustPressed(VK_SPACE) || aButton || TestXboxVirtualButtons()){
		m_theGame->FireBullet();
	}

	if (g_theInput->WasKeyJustPressed('X') || g_theInput->GetController(m_theGame->m_ship->m_controllerID)->WasButtonJustPressed(XBOX_X)){
		m_theGame->BeginStarburst();
	}

	if(g_theInput->WasKeyJustReleased('X') || g_theInput->GetController(m_theGame->m_ship->m_controllerID)->WasButtonJustReleased(XBOX_X)){
		m_theGame->FireStarburst();
	}


}

bool App::IsQuitting()
{
	return m_isQuitting;
}

bool TestXboxVirtualButtons()
{
	XboxController* controller = g_theInput->GetController(0);
	if (controller->WasButtonJustPressed(XBOX_TRIGGER_PRESS_LEFT) || controller->WasButtonJustPressed(XBOX_TRIGGER_PRESS_RIGHT)){
		return true;
	}
	if (controller->WasButtonJustPressed(XBOX_THUMBSTICK_LEFT_PRESS_DOWN)){
		return true;
	}
	if (controller->WasButtonJustPressed(XBOX_THUMBSTICK_LEFT_PRESS_LEFT)){
		return true;
	}
	if (controller->WasButtonJustPressed(XBOX_THUMBSTICK_LEFT_PRESS_RIGHT)){
		return true;
	}
	if (controller->WasButtonJustPressed(XBOX_THUMBSTICK_LEFT_PRESS_UP)){
		return true;
	}
	if (controller->WasButtonJustPressed(XBOX_THUMBSTICK_RIGHT_PRESS_DOWN)){
		return true;
	}
	if (controller->WasButtonJustPressed(XBOX_THUMBSTICK_RIGHT_PRESS_LEFT)){
		return true;
	}
	if (controller->WasButtonJustPressed(XBOX_THUMBSTICK_RIGHT_PRESS_RIGHT)){
		return true;
	}
	if (controller->WasButtonJustPressed(XBOX_THUMBSTICK_RIGHT_PRESS_UP)){
		return true;
	}
	return false;
}
