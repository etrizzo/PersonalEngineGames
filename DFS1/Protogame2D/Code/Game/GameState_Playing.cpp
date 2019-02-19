#include "Game/GameState_Playing.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/SpriteRenderPath.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Flower.hpp"


GameState_Playing::GameState_Playing()
{
	m_renderPath = new SpriteRenderPath();
	m_renderPath->m_renderer = g_theRenderer;
	m_scene = new RenderScene2D();


	m_couchMaterial = Material::GetMaterial("couch");

	m_player = new Player(Vector2(1.f,1.f));

	//for (int i = 0; i < 28; i++)
	//{
	//	Entity* ent = new Entity(Vector2(1.75f + (i * .5f), 7.f), Vector2::HALF * .5f, "white", RGBA::GetRandomEarthTone());
	//	m_scene->AddRenderable(ent->m_renderable);
	//}

	////Random HSV
	//for (int i = 0; i < 28; i++)
	//{
	//	Entity* ent = new Entity(Vector2(1.75f + (i * .5f), 6.f), Vector2::HALF * .5f, "white", RGBA::GetRGBAFromHSV(GetRandomFloatZeroToOne(), .5f, .75f));
	//	m_scene->AddRenderable(ent->m_renderable);
	//}

	////sunflower
	//RGBA goodYellow = RGBA(240, 200, 60);
	//RGBA goodRed = RGBA(133, 24, 0);
	//for (int i = 0; i < 28; i++)
	//{
	//	RGBA randomYellow = RGBA::GetRandomMixedColor(goodYellow, .8f);
	//	RGBA randomRed = RGBA::GetRandomMixedColor(goodRed, .8f);
	//	float perc = (float) i / 28.f;
	//	float intValue = SmoothStep3(perc );
	//	RGBA entColor = Interpolate(randomYellow, randomRed, intValue);
	//	
	//	Entity* ent = new Entity(Vector2(1.75f + (i * .5f), 5.f), Vector2::HALF * .5f, "white", entColor);
	//	m_scene->AddRenderable(ent->m_renderable);
	//}

	////golden ratio
	//for (int i = 0; i < 28; i++)
	//{
	//	Entity* ent = new Entity(Vector2(1.75f + (i * .5f), 4.f), Vector2::HALF * .5f, "white", RGBA::GetGoldenRatioColorSequence(.5f, .75f));
	//	m_scene->AddRenderable(ent->m_renderable);
	//}

	//pastels
	/*for (int i = 0; i < 28; i++)
	{
		Entity* ent = new Entity(Vector2(1.75f + (i * .5f), 3.f), Vector2::HALF * .5f, "white", RGBA::GetRandomMixedColor(RGBA(149,200,45), .8f));
		m_scene->AddRenderable(ent->m_renderable);
	}*/

	//for (int i = 0; i < 28; i++)
	//{
	//	float perc = (float) i / 28.f;
	//	Entity* ent = new Entity(Vector2(1.75f + (i * .5f), 2.f), Vector2::HALF * .5f, "white", RGBA::GetRGBAFromHSV(perc, 1.f, 1.f));
	//	m_scene->AddRenderable(ent->m_renderable);
	//}
	

	m_flower = new Flower(Vector2::ONE * 3.f, 4.f);
	m_scene->AddRenderable(m_flower->m_renderable);

	m_scene->AddRenderable(m_player->m_renderable);

	m_scene->AddCamera(g_theGame->m_currentCamera);
}

void GameState_Playing::Update(float ds)
{
	m_timeInState+=ds;

	float deltaSeconds = ds;

	m_couchMaterial->SetProperty("SPECULAR_AMOUNT", m_specAmount);
	m_couchMaterial->SetProperty("SPECULAR_POWER", m_specFactor);
	m_player->Update();
}

void GameState_Playing::RenderGame()
{
	g_theRenderer->ClearScreen( RGBA::BLACK ); 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 

	//SetShader();

	g_theRenderer->BindModel(Matrix44::IDENTITY);




	m_renderPath->Render(m_scene);

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
	//g_theGame->m_debugRenderSystem->MakeDebugRender2DText("Spec Amount(N/M): %s\nSpec Power(J/K): %s", specAmount.c_str(), specFactor.c_str());
	//g_theGame->m_debugRenderSystem->MakeDebugRender2DText(0.f, .01f, Vector2(1.f, .95f), "Shader ([/]): %s", GetShaderName().c_str());
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

	if (g_theInput->WasKeyJustPressed('O')){
		m_flower->GenerateFlower();
	}

	if (!g_theGame->m_debugRenderSystem->m_isDetached){
		m_player->HandleInput();
	}
}



//void GameState_Playing::AddNewLight(std::string type, RGBA color)
//{
//	Vector3 pos = g_theGame->m_currentCamera->GetPosition() + g_theGame->m_currentCamera->GetForward();
//	m_scene->AddNewLight(type, pos, color);
//}
//
//void GameState_Playing::AddNewLight(std::string type, Vector3 pos, RGBA color)
//{
//	m_scene->AddNewLight(type, pos, color);
//}
//
//void GameState_Playing::AddNewPointLight(Vector3 pos, RGBA color)
//{
//	m_scene->AddNewPointLight(pos, color);
//}
//
//void GameState_Playing::AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle, float outerAngle)
//{
//	m_scene->AddNewSpotLight(pos, color, innerAngle, outerAngle);
//}
//
//void GameState_Playing::AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation)
//{
//	m_scene->AddNewDirectionalLight(pos, color, rotation);
//
//}
//
//void GameState_Playing::RemoveLight(int idx)
//{
//	m_scene->RemoveLight(idx);
//}
//
//void GameState_Playing::SetLightPosition(Vector3 newPos, unsigned int idx)
//{
//	m_scene->SetLightPosition(newPos, idx);
//}
//
//void GameState_Playing::SetLightColor(RGBA newColor, unsigned int idx)
//{
//	m_scene->SetLightColor(newColor, idx);
//}
//
//void GameState_Playing::SetLightColor(Vector4 newColor, unsigned int idx)
//{
//	m_scene->SetLightColor(newColor, idx);
//}
//
//void GameState_Playing::SetLightAttenuation(int lightIndex, Vector3 att)
//{
//	m_scene->SetLightAttenuation(lightIndex, att);
//
//}
//
//unsigned int GameState_Playing::GetNumActiveLights() const
//{
//	return m_scene->m_lights.size();
//}



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
	
	g_theGame->m_debugRenderSystem->MakeDebugRender2DText(shaderName.c_str());

	g_theRenderer->UseShader(shaderName);
	m_player->m_renderable->SetShader(shaderName);
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
