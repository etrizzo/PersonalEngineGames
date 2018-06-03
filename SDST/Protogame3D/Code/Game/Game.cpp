#include "Game.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/Entity.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include <map>


Game::~Game()
{
	if (m_debugRenderSystem->IsActive()){
		m_debugRenderSystem->Shutdown();
	}
}

Game::Game()
{
	m_renderPath = new ForwardRenderPath();
	m_renderPath->m_renderer = g_theRenderer;
	m_scene = new RenderScene();
	m_tileTexture = g_theRenderer->CreateOrGetTexture("Terrain_8x8.png");
	g_tileSpriteSheet = new SpriteSheet(*m_tileTexture, 8, 8);
	m_isPaused = false;
	m_devMode = false;
	m_gameTime = 0.f;
	m_currentMap = nullptr;

	LoadMapDefinitions();
	
	m_mainCamera = new PerspectiveCamera();

	// Setup what it will draw to
	m_mainCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_mainCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );


	m_mainCamera->SetPerspectiveOrtho(45.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.1f, 100.f);
	m_mainCamera->LookAt(Vector3(0.f, 4.f, -5.f), Vector3(0.f, 1.f, 0.f));

	m_uiCamera = new Camera();

	// Setup what it will draw to
	m_uiCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_uiCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );


	// for -1 to 1
	//m_uiCamera->SetProjectionOrtho( Vector3(-1.f, -1.f, 0.f), Vector3(1.f, 1.f, 100.f) );  
	//m_uiCamera->LookAt( Vector3( 0.0f, 0.0f, -10.0f ), Vector3::ZERO ); 
	m_uiCamera->SetProjectionOrtho(1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);
	Vector2 center = m_uiCamera->GetBounds().GetCenter();
	m_uiCamera->LookAt( Vector3( center.x, center.y, -1.f ), Vector3(center.x, center.y, .5f)); 

	SetMainCamera();
	
}

void Game::PostStartup()
{
	m_debugRenderSystem = new DebugRenderSystem();
	DebugStartup();
	m_debugRenderSystem->DetachCamera();
	m_couchMaterial = Material::GetMaterial("couch");

	m_thaShip = new Entity(Vector3::ZERO, "scifi_fighter_mk6.obj");
	m_thaShip->SetDiffuseTexture("SciFi_Fighter-MK6-diffuse.png");

	m_thaMiku = new Entity(Vector3(0.f, 3.f, 10.f), "miku.obj", "miku.mtl");
	m_thaMiku->Rotate(Vector3(0.f,180.f,0.f));
	m_thaMiku->m_renderable->SetShader("lit_alpha", 0);
	m_thaMiku->m_renderable->SetShader("lit_alpha", 1);
	m_thaMiku->m_renderable->SetShader("default_lit", 2);

	m_particleSystem = new ParticleSystem();
	m_particleSystem->CreateEmitter(Vector3(0.f, 4.f, 0.f));
	m_particleSystem->m_emitters[0]->SetSpawnRate(200.f);

	m_scene->AddRenderable(m_particleSystem->m_emitters[0]->m_renderable);
	m_scene->AddRenderable(m_thaShip->m_renderable);
	m_scene->AddRenderable(m_thaMiku->m_renderable);


	//m_scene->AddNewPointLight(Vector3::ZERO, RGBA::WHITE);
	m_scene->AddNewSpotLight(Vector3(0.f, 4.f, -5.f), RGBA::WHITE, 20.f, 23.f);		//camera light
	m_scene->AddNewSpotLight(Vector3(0.f, 5.f, 5.f), RGBA(255,255,128,255));			//orbiting light
	m_scene->AddNewPointLight(Vector3(0.f, 5.f, 15.f), RGBA(255, 128, 70,255));		//reddish point light
	m_scene->AddNewDirectionalLight(Vector3(-10.f, 0.f, -10.f), RGBA::WHITE, Vector3(0.f, -90.f, -10.f));		//bluish directional light

	m_cameraLight = m_scene->m_lights[0];
	m_orbitLight = (SpotLight*) m_scene->m_lights[1];
	
}

void Game::Update()
{
	float deltaSeconds = GetDeltaSeconds();
	if (!m_isPaused){
		//the game stuff happens here
		m_gameTime+=deltaSeconds;


		float degrees = 15.f * m_gameTime; 
		//
		Vector2 xz_pos = PolarToCartesian( 8.f, degrees ); 
		Vector3 pos = Vector3( xz_pos.x, 5.f, xz_pos.y ); 
		m_orbitLight->LookAt(pos, Vector3::ZERO);
		m_thaMiku->Rotate(Vector3(0.f,1.f,0.f));

		if (m_cameraLight != nullptr){
			m_cameraLight->SetTransform(m_currentCamera->m_transform	);
		}
	}
	
	m_couchMaterial->SetProperty("SPECULAR_AMOUNT", m_specAmount);
	m_couchMaterial->SetProperty("SPECULAR_POWER", m_specFactor);
	m_particleSystem->Update(deltaSeconds);
	
}

void Game::Render()
{

	float quadZ = 12.f;
	Vector2 quadSize = Vector2(3.f, .2f);
	RGBA xrayColor = RGBA(255,200,0,255);
	m_debugRenderSystem->MakeDebugRender3DText("Use Depth!", 0.f, Vector3(-4.f, 1.f, quadZ), quadSize.y, Vector3::UP, Vector3::RIGHT, RGBA::RED, RGBA::RED, DEBUG_RENDER_USE_DEPTH);
	m_debugRenderSystem->MakeDebugRenderQuad(0.f, Vector3(0.f, 1.f, quadZ), quadSize, Vector3::RIGHT, Vector3::UP, RGBA::RED, RGBA::RED, DEBUG_RENDER_USE_DEPTH);

	m_debugRenderSystem->MakeDebugRender3DText("Ignore Depth!", 0.f, Vector3(-4.f, .25f, quadZ), quadSize.y, Vector3::UP, Vector3::RIGHT, RGBA::CYAN, RGBA::CYAN, DEBUG_RENDER_IGNORE_DEPTH);
	m_debugRenderSystem->MakeDebugRenderQuad(0.f, Vector3(0.f, .25f, quadZ), quadSize, Vector3::RIGHT, Vector3::UP, RGBA::CYAN, RGBA::CYAN, DEBUG_RENDER_IGNORE_DEPTH);
	
	m_debugRenderSystem->MakeDebugRender3DText("Hidden!", 0.f, Vector3(-4.f, -.5f, quadZ), quadSize.y, Vector3::UP, Vector3::RIGHT, RGBA::YELLOW, RGBA::YELLOW, DEBUG_RENDER_HIDDEN);
	m_debugRenderSystem->MakeDebugRenderQuad(0.f, Vector3(0.f, -.5f, quadZ), quadSize, Vector3::RIGHT, Vector3::UP, RGBA::YELLOW, RGBA::YELLOW, DEBUG_RENDER_HIDDEN);
	
	m_debugRenderSystem->MakeDebugRender3DText("XRAY!", 0.f, Vector3(-4.f, -1.25f, quadZ), quadSize.y, Vector3::UP, Vector3::RIGHT,xrayColor, xrayColor, DEBUG_RENDER_XRAY);
	m_debugRenderSystem->MakeDebugRenderQuad(0.f, Vector3(0.f, -1.25f, quadZ), quadSize, Vector3::RIGHT, Vector3::UP, xrayColor, xrayColor, DEBUG_RENDER_XRAY);
	m_debugRenderSystem->MakeDebugRender3DText("(0,0)", 0.f, Vector3::ZERO, .5f, Vector3::UP, Vector3::RIGHT, RGBA::RED, RGBA::RED, DEBUG_RENDER_IGNORE_DEPTH);
	RenderGame();
	RenderUI();
}

void Game::HandleInput()
{
	m_debugRenderSystem->HandleInput();

	if (g_theInput->WasKeyJustPressed(VK_OEM_6)){
		UpdateShader(1);
	}
	if (g_theInput->WasKeyJustPressed(VK_OEM_4)){
		UpdateShader(-1);
	}


	if (g_theInput->WasKeyJustPressed('L')){
		AddNewPointLight( m_currentCamera->GetPosition() + m_currentCamera->GetForward(), RGBA::WHITE);
	}
	float ds = GetDeltaSeconds();
	float factorScale = 10.f;
	if (g_theInput->IsKeyDown('K')){
		m_specFactor+=(ds * factorScale);
		m_specFactor = ClampFloat(m_specFactor, 1.f, 99.999f);
	}
	if (g_theInput->IsKeyDown('J')){
		m_specFactor-=(ds * factorScale);
		m_specFactor = ClampFloat(m_specFactor, 1.f, 99.999f);
	}

	if (g_theInput->IsKeyDown('M')){
		m_specAmount+=ds;
		m_specAmount = ClampFloat(m_specAmount, 0.f, 1.f);
	}
	if (g_theInput->IsKeyDown('N')){
		m_specAmount-=ds;
		m_specAmount = ClampFloat(m_specAmount, 0.f, 1.f);
	}
}

float Game::GetDeltaSeconds()
{
	float ds = GetMasterClock()->GetDeltaSeconds();
	if (g_theInput->IsKeyDown('T')){
		ds/=10.f;
	}
	ds  = ClampFloat(ds, .001f, .4f);

	if (g_theGame->m_isPaused){
		ds = 0.f;
	}
	return ds;
}


AABB2 Game::SetUICamera()
{
	g_theRenderer->SetCamera( m_uiCamera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	//g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );
	g_theRenderer->DisableDepth();
	return m_uiCamera->GetBounds();
}

AABB2 Game::SetMainCamera()
{
	m_currentCamera = m_mainCamera;
	SetGameCamera();
	return m_mainCamera->GetBounds();		//not really gonna be necessary, probably
}

AABB2 Game::GetUIBounds()
{
	return m_uiCamera->GetBounds();
}

Vector3 Game::GetCurrentCameraUp()
{
	return m_currentCamera->GetUp();
}

Vector3 Game::GetCurrentCameraRight()
{
	return m_currentCamera->GetRight();
}

void Game::SetGameCamera(PerspectiveCamera* newCam)
{
	if (newCam != nullptr){
		m_currentCamera = newCam;
	}
	g_theRenderer->SetCamera( m_currentCamera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	
}



void Game::TogglePause()
{
	if ( !m_isPaused){
		m_isPaused = true;
	} else {
		m_isPaused = false;
	}
}

void Game::ToggleDevMode()
{
	if ( !m_devMode){
		m_devMode = true;
	} else {
		m_devMode = false;
	}
}

void Game::DebugStartup()
{
	m_debugRenderSystem->Startup(m_mainCamera);
}

void Game::DebugShutdown()
{
	m_debugRenderSystem->Shutdown();
	SetMainCamera();
}

void Game::DebugClear()
{
	m_debugRenderSystem->Clear();
}

void Game::DebugToggleRendering()
{
	m_debugRenderSystem->ToggleRendering();
}

void Game::AddNewLight(std::string type, RGBA color)
{
	Vector3 pos = m_currentCamera->GetPosition() + m_currentCamera->GetForward();
	m_scene->AddNewLight(type, pos, color);
}

void Game::AddNewLight(std::string type, Vector3 pos, RGBA color)
{
	m_scene->AddNewLight(type, pos, color);
}

void Game::AddNewPointLight(Vector3 pos, RGBA color)
{
	m_scene->AddNewPointLight(pos, color);
}

void Game::AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle, float outerAngle)
{
	m_scene->AddNewSpotLight(pos, color, innerAngle, outerAngle);
}

void Game::AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation)
{
	m_scene->AddNewDirectionalLight(pos, color, rotation);

}

void Game::RemoveLight(int idx)
{
	if (m_lights.size() > idx){
		Light* del = m_lights[idx];
		if (del == m_cameraLight){
			m_cameraLight = nullptr;
		}
		if (del == m_orbitLight){
			m_orbitLight = nullptr;
		}
	}
	m_scene->RemoveLight(idx);
}

void Game::SetLightPosition(Vector3 newPos, unsigned int idx)
{
	m_scene->SetLightPosition(newPos, idx);
}

void Game::SetLightColor(RGBA newColor, unsigned int idx)
{
	m_scene->SetLightColor(newColor, idx);
}

void Game::SetLightColor(Vector4 newColor, unsigned int idx)
{
	m_scene->SetLightColor(newColor, idx);
}

void Game::SetLightAttenuation(int lightIndex, Vector3 att)
{
	m_scene->SetLightAttenuation(lightIndex, att);
	
}

void Game::LoadTileDefinitions()
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile("Data/Data/Tiles.xml");


	tinyxml2::XMLElement* root = tileDefDoc.FirstChildElement("TileDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("TileDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("TileDefinition")){
		TileDefinition newDefinition = TileDefinition(tileDefElement);
		TileDefinition::s_definitions.insert(std::pair<std::string, TileDefinition>(newDefinition.m_name, newDefinition));
	}


}

void Game::LoadMapDefinitions()
{
	tinyxml2::XMLDocument mapDefDoc;
	mapDefDoc.LoadFile("Data/Data/Maps.xml");


	tinyxml2::XMLElement* root = mapDefDoc.FirstChildElement("MapDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("MapDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("MapDefinition")){
		//had to make make MapDefinition take in pointer because was getting inaccessible error when trying to derference
		MapDefinition* newDefinition = new MapDefinition(tileDefElement);

		MapDefinition::s_definitions.insert(std::pair<std::string, MapDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::RenderGame()
{
	m_particleSystem->m_emitters[0]->CameraPreRender(m_currentCamera);
	SetShader();

	//m_thaShip->m_renderable->GetEditableMaterial()->SetProperty("TINT", RGBA::RED);
	
	g_theRenderer->BindModel(Matrix44::IDENTITY);


	//for a grass block
	//g_theRenderer->BindTexture("Terrain_8x8.png");
	//AABB2 sideUVS = g_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(4, 2));
	//AABB2 topUVs = g_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1, 1));

	g_theRenderer->BindMaterial(m_couchMaterial);
	g_theRenderer->DrawCube( Vector3(0.f,0.f,10.f), Vector3::ONE, RGBA::WHITE); 

	
	g_theRenderer->DrawSphere( Vector3( 3.0f, 0.0f, 10.f ), 1.f, 15, 15, RGBA::WHITE ); 
	g_theRenderer->DrawSphere( Vector3( 6.0f, 0.0f, 10.f ), 1.f, 15, 15, RGBA::WHITE ); 




	Plane p = Plane(Vector3(-3.f, 0.f, 10.f), Vector3::UP, Vector3::RIGHT, Vector2(1.f,1.f));
	TODO("Clean up the whole plane class and go back and fix up sprites/tactics");
	g_theRenderer->DrawPlane(p, RGBA::WHITE);



	//m_thaShip->Render();


	

	m_renderPath->Render(m_scene);

	g_theRenderer->ReleaseTexture(0);
	g_theRenderer->ReleaseTexture(1);

	g_theRenderer->ReleaseShader();
	m_debugRenderSystem->UpdateAndRender();


}

void Game::RenderUI()
{
	AABB2 bounds = SetUICamera();
	std::string specFactor = std::to_string(m_specFactor);
	specFactor.resize(5);
	//specFactor = "Spec Factor (J/K): " + specFactor;
	
	std::string specAmount = std::to_string(m_specAmount);
	specAmount.resize(5);
	//specAmount = "\nSpec Amount(N/M): " + specAmount;
	//g_theRenderer->DrawTextInBox2D(specFactor + specAmount, bounds, Vector2(1.f, .6f), .015f);
	m_debugRenderSystem->MakeDebugRender2DText("Spec Amount(N/M): %s\nSpec Power(J/K): %s", specAmount.c_str(), specFactor.c_str());
	m_debugRenderSystem->MakeDebugRender2DText(0.f, .01f, Vector2(1.f, .95f), "Shader ([/]): %s", GetShaderName().c_str());
}



void Game::UpdateShader(int direction)
{
	//m_debugShader= eDebugShaders(ClampInt(m_debugShader + direction, 0, NUM_DEBUG_SHADERS - 1));
	int newShader = m_debugShader + direction + NUM_DEBUG_SHADERS;
	m_debugShader= eDebugShaders(newShader % NUM_DEBUG_SHADERS);
	
}

void Game::SetShader()
{
	std::string shaderName = "invalid";
	switch (m_debugShader){
	case SHADER_LIT:
		shaderName = "default_lit";
		break;
	case SHADER_NORMAL		:
		shaderName = "vertex_normal";
		break;
	case SHADER_TANGENT		:
		shaderName = "tangent";
		break;
	case SHADER_BITANGENT	:
		shaderName = "bitangent";
		break;
	case SHADER_NORMALMAP	:
		shaderName = "normal_map";
		break;
	case SHADER_WORLDNORMAL	:
		shaderName = "world_normal";
		break;
	case SHADER_DIFFUSE		:
		shaderName = "diffuse";
		break;
	case SHADER_SPECULAR	:
		shaderName = "specular";
		break;
	}

	g_theRenderer->UseShader(shaderName);
	m_thaShip->m_renderable->SetShader(shaderName);
	m_thaMiku->m_renderable->SetShader(shaderName, 2);
	m_couchMaterial->SetShader(shaderName);
}

std::string Game::GetShaderName() const
{
	switch (m_debugShader){
	case SHADER_LIT:
		return "default_lit";
		break;
	case SHADER_NORMAL		:
		return "vertex_normal";
		break;
	case SHADER_TANGENT		:
		return "tangent";
		break;
	case SHADER_BITANGENT	:
		return "bitangent";
		break;
	case SHADER_NORMALMAP	:
		return "normal_map";
		break;
	case SHADER_WORLDNORMAL	:
		return "world_normal";
		break;
	case SHADER_DIFFUSE :
		return "diffuse";
		break;
	case SHADER_SPECULAR :
		return "specular";
		break;

	}
	return "NO_SHADER";
}






Game* g_theGame = nullptr;