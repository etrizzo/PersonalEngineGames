#include "Game/GameState_Playing.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"


GameState_Playing::GameState_Playing()
{
	m_renderPath = new ForwardRenderPath();
	m_renderPath->m_renderer = g_theRenderer;
	m_scene = new RenderScene();


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

	m_scene->AddCamera(g_theGame->m_currentCamera);
}

void GameState_Playing::Update(float ds)
{
	m_timeInState+=ds;

	float deltaSeconds = ds;
	if (!g_theGame->m_isPaused){
		//the game stuff happens here
		//g_theGame->m_gameTime+=deltaSeconds;


		float degrees = 15.f * g_theGame->m_gameClock->GetCurrentSeconds(); 
		//
		Vector2 xz_pos = PolarToCartesian( 8.f, degrees ); 
		Vector3 pos = Vector3( xz_pos.x, 5.f, xz_pos.y ); 
		m_orbitLight->LookAt(pos, Vector3::ZERO);
		m_thaMiku->Rotate(Vector3(0.f,1.f,0.f));

		if (m_cameraLight != nullptr){
			m_cameraLight->SetTransform(g_theGame->m_currentCamera->m_transform	);
		}
	}

	m_couchMaterial->SetProperty("SPECULAR_AMOUNT", m_specAmount);
	m_couchMaterial->SetProperty("SPECULAR_POWER", m_specFactor);
	m_particleSystem->Update(deltaSeconds);
}

void GameState_Playing::RenderGame()
{
	g_theRenderer->ClearScreen( RGBA::BLACK ); 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 

	if (g_theGame->IsDevMode()){		//draw cube at origin
		g_theRenderer->DrawCube(Vector3::ZERO,Vector3::ONE, RGBA::RED);
	}

	m_particleSystem->m_emitters[0]->CameraPreRender(g_theGame->m_currentCamera);
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

	g_theGame->m_debugRenderSystem->UpdateAndRender();
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 
	
}

void GameState_Playing::RenderUI()
{
	AABB2 bounds = g_theGame->SetUICamera();
	std::string specFactor = std::to_string(m_specFactor);
	specFactor.resize(5);
	//specFactor = "Spec Factor (J/K): " + specFactor;

	std::string specAmount = std::to_string(m_specAmount);
	specAmount.resize(5);
	//specAmount = "\nSpec Amount(N/M): " + specAmount;
	//g_theRenderer->DrawTextInBox2D(specFactor + specAmount, bounds, Vector2(1.f, .6f), .015f);
	g_theGame->m_debugRenderSystem->MakeDebugRender2DText("Spec Amount(N/M): %s\nSpec Power(J/K): %s", specAmount.c_str(), specFactor.c_str());
	g_theGame->m_debugRenderSystem->MakeDebugRender2DText(0.f, .01f, Vector2(1.f, .95f), "Shader ([/]): %s", GetShaderName().c_str());
}

void GameState_Playing::HandleInput()
{
	
	g_theGame->m_debugRenderSystem->HandleInput();

	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}
	if (g_theInput->WasKeyJustPressed('P') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) ){
		g_theGame->TransitionToState(new GameState_Paused(this));
	}

	if (g_theInput->WasKeyJustPressed('T')){
		g_theGame->m_gameClock->SetScale(.1f);
	}
	if (g_theInput->WasKeyJustReleased('T')){
		g_theGame->m_gameClock->SetScale(1.f);
	}

	if (g_theInput->WasKeyJustPressed(VK_OEM_6)){
		UpdateShader(1);
	}
	if (g_theInput->WasKeyJustPressed(VK_OEM_4)){
		UpdateShader(-1);
	}


	if (g_theInput->WasKeyJustPressed('L')){
		AddNewPointLight( g_theGame->m_currentCamera->GetPosition() + g_theGame->m_currentCamera->GetForward(), RGBA::WHITE);
	}
	float ds = g_theGame->GetDeltaSeconds();
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



void GameState_Playing::AddNewLight(std::string type, RGBA color)
{
	Vector3 pos = g_theGame->m_currentCamera->GetPosition() + g_theGame->m_currentCamera->GetForward();
	m_scene->AddNewLight(type, pos, color);
}

void GameState_Playing::AddNewLight(std::string type, Vector3 pos, RGBA color)
{
	m_scene->AddNewLight(type, pos, color);
}

void GameState_Playing::AddNewPointLight(Vector3 pos, RGBA color)
{
	m_scene->AddNewPointLight(pos, color);
}

void GameState_Playing::AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle, float outerAngle)
{
	m_scene->AddNewSpotLight(pos, color, innerAngle, outerAngle);
}

void GameState_Playing::AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation)
{
	m_scene->AddNewDirectionalLight(pos, color, rotation);

}

void GameState_Playing::RemoveLight(int idx)
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

void GameState_Playing::SetLightPosition(Vector3 newPos, unsigned int idx)
{
	m_scene->SetLightPosition(newPos, idx);
}

void GameState_Playing::SetLightColor(RGBA newColor, unsigned int idx)
{
	m_scene->SetLightColor(newColor, idx);
}

void GameState_Playing::SetLightColor(Vector4 newColor, unsigned int idx)
{
	m_scene->SetLightColor(newColor, idx);
}

void GameState_Playing::SetLightAttenuation(int lightIndex, Vector3 att)
{
	m_scene->SetLightAttenuation(lightIndex, att);

}

unsigned int GameState_Playing::GetNumActiveLights() const
{
	return m_scene->m_lights.size();
}



void GameState_Playing::UpdateShader(int direction)
{
	//m_debugShader= eDebugShaders(ClampInt(m_debugShader + direction, 0, NUM_DEBUG_SHADERS - 1));
	int newShader = m_debugShader + direction + NUM_DEBUG_SHADERS;
	m_debugShader= eDebugShaders(newShader % NUM_DEBUG_SHADERS);

}

void GameState_Playing::SetShader()
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

std::string GameState_Playing::GetShaderName() const
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
