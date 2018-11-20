#include "Game.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/Player.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include "Game/StoryState.hpp"
#include <map>


Game::~Game()
{
	if (m_debugRenderSystem->IsActive()){
		m_debugRenderSystem->Shutdown();
	}
}

Game::Game()
{
	
	m_tileTexture = g_theRenderer->CreateOrGetTexture("Terrain_8x8.png");
	g_tileSpriteSheet = new SpriteSheet(*m_tileTexture, 8, 8);
	m_isPaused = false;
	m_devMode = false;
	//m_gameTime = 0.f;

	
	m_mainCamera = new PerspectiveCamera();

	// Setup what it will draw to
	m_mainCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_mainCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );


	m_mainCamera->SetProjectionOrtho(1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f, 100.f);
	

	m_uiCamera = new Camera();

	// Setup what it will draw to
	m_uiCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_uiCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );


	// for -1 to 1
	//m_uiCamera->SetProjectionOrtho( Vector3(-1.f, -1.f, 0.f), Vector3(1.f, 1.f, 100.f) );  
	//m_uiCamera->LookAt( Vector3( 0.0f, 0.0f, -10.0f ), Vector3::ZERO ); 
	m_uiCamera->SetProjectionOrtho(1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f, 100.f);
	Vector2 center = m_uiCamera->GetBounds().GetCenter();
	m_uiCamera->LookAt( Vector3( center.x, center.y, -1.f ), Vector3(center.x, center.y, .5f)); 
	m_mainCamera->LookAt( Vector3( center.x, center.y, -1.f ), Vector3(center.x, center.y, .5f)); 


	SetMainCamera();

	Clock* masterClock = GetMasterClock();

	m_gameClock = new Clock(masterClock);


	m_currentState = new GameState_Loading();

	
}

void Game::PostStartup()
{
	PROFILE_LOG_SCOPE_FUNCTION();
	m_soundTrackID = g_theAudio->CreateOrGetSound("Data/Audio/OrbitalColossus.mp3");

	m_currentMap = nullptr;
	m_playState = new GameState_Playing();

	m_debugRenderSystem = new DebugRenderSystem();
	DebugStartup();
	m_debugRenderSystem->ToggleInfo();
	m_debugRenderSystem->ToggleScreenGrid();
	//m_debugRenderSystem->DetachCamera();

	m_currentState = new GameState_Attract();
	//m_currentMap = new Map("Heightmap.png", AABB2(-100.f, -100.f, 100.f, 100.f), -5.f, 2.f, IntVector2(20,20), 40.f);

	//m_soundtrackPlayback = g_theAudio->PlaySound(m_soundTrackID, true, .5f);


	m_graph = StoryGraph();

	m_graph.ReadPlotNodesFromXML("Data/Data/PlotGrammars.xml");
	m_graph.ReadDetailNodesFromXML("Data/Data/DetailGrammars.xml");
	m_graph.ReadCharactersFromXML("Data/Data/Characters.xml");

	GenerateGraph();

	//GraphTests();
	//StoryGraphTests();

}

void Game::GenerateGraph()
{
	m_graph.Clear();

	m_graph.RunGeneration(NUM_PLOT_NODES_TO_GENERATE,NUM_DETAIL_NODES_TO_GENERATE + NUM_PLOT_NODES_TO_GENERATE);
	m_graph.RunNodeAdjustments();
}

void Game::ClearGraph()
{
	m_graph.Clear();
}

void Game::GeneratePlotNodes(int numToGenerate)
{
	m_graph.GenerateSkeleton(numToGenerate);
}

void Game::GenerateDetailNodes(int numToGenerate)
{
	m_graph.AddDetailNodesToDesiredSize(numToGenerate + m_graph.GetNumNodes());
}

void Game::GenerateNodePairs(int numToGenerate)
{
	for (int i = 0; i < numToGenerate; i++){
		AddPlotAndOutcomeNodeInPair();
	}
}

void Game::AddPlotAndOutcomeNodeInPair()
{
	StoryNode* newNode = m_graph.AddSinglePlotNode();
	m_graph.AddOutcomeNodesToPlotNode(newNode);
}



void Game::Update()
{
	PROFILE_PUSH_FUNCTION();
	//PROFILE_LOG_SCOPE_FUNCTION();
	float ds = GetDeltaSeconds();
	m_currentState->Update(ds);
	PROFILE_POP();

	m_graph.UpdateNodePositions();
	
}

void Game::Render()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	m_currentState->Render();
	//RenderGame();
	RenderUI();
}

void Game::HandleInput()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	m_normalizedMousePos = g_theInput->GetMouseNormalizedScreenPosition(GetUIBounds());
	//m_debugRenderSystem->HandleInput();
	m_currentState->HandleInput();
	m_graph.HandleInput();
}

void Game::RenderLoadScreen()
{
	AABB2 uiBounds = SetUICamera();
	g_theRenderer->DrawAABB2(uiBounds, RGBA(100,100,100,255));
	g_theRenderer->DrawTextInBox2D("Loading...", uiBounds, Vector2::HALF, .2f, TEXT_DRAW_SHRINK_TO_FIT);
}

void Game::TransitionToState(GameState* newState)
{
	/*if (newState == GAME_STATE_ATTRACT){
	m_encounterState->Enter("heightMap.png");
	}*/
	m_transitionToState = newState;
	m_currentState->StartTransition();
	//m_startTransitionTime = m_gameTime;
}

void Game::TriggerTransition()
{
	m_currentState = m_transitionToState;
	m_transitionToState = nullptr;
	m_timeEnteredState = m_gameClock->GetCurrentSeconds();
	m_currentState->EnterState();
	//m_currentState->ResetTime();
}

float Game::GetDeltaSeconds()
{
	float ds = m_gameClock->GetDeltaSeconds();

	if (g_theGame->m_isPaused){
		ds = 0.f;
	}
	return ds;
}

Vector2 Game::GetPlayerPositionXZ() const
{
	if (m_playState != nullptr){
		return m_playState->m_player->m_positionXZ;
	}
	return Vector2::ZERO;
}

Player * Game::GetPlayer() const
{
	if (m_playState != nullptr){
		return m_playState->m_player;
	}
	return nullptr;
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

RenderScene * Game::GetScene()
{
	return m_playState->GetScene();
}




void Game::RenderGame()
{
	m_currentState->Render();
	//m_debugRenderSystem->UpdateAndRender();
}

void Game::RenderUI()
{
	//OBB2 box = OBB2 (Vector2::HALF, Vector2(.5f, -.2f), Vector2(.4f, .2f));
	//RGBA boxColor = RGBA::DARKGRAY;
	//if (box.IsPointInside(g_theGame->m_normalizedMousePos)){
	//	boxColor = RGBA::CYAN;
	//}
	//g_theRenderer->DrawOBB2(box, boxColor);
}

void Game::GraphTests()
{
	//DirectedGraph<StoryData, StoryState> engineGraph = DirectedGraph<StoryData, StoryState>();
	//ASSERT_OR_DIE(engineGraph.GetNumNodes() == 0, "Num nodes should be 0");
	//StoryData AData = StoryData("A", 1.f);
	//Node<StoryData, StoryState>* a = engineGraph.AddNode(AData);
	////Test graph addition
	//ASSERT_OR_DIE(engineGraph.GetNumNodes() == 1, "Num nodes should be 1");

	////Test ContainsNode
	//ASSERT_OR_DIE(engineGraph.ContainsNode(a), "ContainsNode not working with node");
	//ASSERT_OR_DIE(engineGraph.ContainsNode(AData), "ContainsNode not working with data");
	//
	////Test for duplicates
	//engineGraph.AddNode(a);
	//ASSERT_OR_DIE(engineGraph.GetNumNodes() == 1, "Num nodes should be 1");
	//ASSERT_OR_DIE(engineGraph.ContainsNode(a), "ContainsNode not working with node");

	////add new nodes to graph
	//Node<StoryData, StoryState>* b = engineGraph.AddNode(StoryData("B", 2.f));
	//Node<StoryData, StoryState>* c = engineGraph.AddNode(StoryData("C", 3.f));
	//Node<StoryData, StoryState>* d = engineGraph.AddNode(StoryData("D", 4.f));
	//ASSERT_OR_DIE(engineGraph.GetNumNodes() == 4, "Num nodes should be 4");
	//
	////Test graph edges
	//ASSERT_OR_DIE(engineGraph.GetNumEdges() == 0, "Num edges should be 0");
	//DirectedEdge<StoryData, StoryState>* testEdge = engineGraph.AddEdge(a,b, StoryState(0.f));
	//ASSERT_OR_DIE(engineGraph.GetNumEdges() == 1, "Num edges should be 1");
	//
	////test ContainsEdge
	//ASSERT_OR_DIE(engineGraph.ContainsEdge(a, b), "ContainsEdge not working with edges");
	//ASSERT_OR_DIE(engineGraph.ContainsEdge(testEdge), "ContainsEdge not working with edges");

	//engineGraph.AddEdge(b,c, StoryState(1.f, ));
	//engineGraph.AddEdge(b,d, StoryState(3.f, ));
	//engineGraph.AddEdge(a,d, StoryState(2.f, ));
	//engineGraph.AddEdge(c,d, StoryState(5.f, ));

	//ASSERT_OR_DIE(engineGraph.GetNumEdges() == 5, "Num edges should be 5");

	////Test Removals
	//engineGraph.RemoveEdge(b,c);
	//ASSERT_OR_DIE(engineGraph.GetNumEdges() == 4, "Num edges should be 4");
	//ASSERT_OR_DIE(!engineGraph.ContainsEdge(b, c), "Edge was not removed");
	//ASSERT_OR_DIE(engineGraph.ContainsNode(b), "ContainsNode not working with node");
	//ASSERT_OR_DIE(engineGraph.ContainsNode(c), "ContainsNode not working with node");

	////return graph to previous state
	//engineGraph.AddEdge(b,c);
	//ASSERT_OR_DIE(engineGraph.GetNumEdges() == 5, "Num edges should be 5");

	////test RemoveNode with node
	//engineGraph.RemoveNode(b);
	//ASSERT_OR_DIE(engineGraph.GetNumNodes() == 3, "Num nodes should be 3");
	//ASSERT_OR_DIE(engineGraph.GetNumEdges() == 2, "Edges attached to b should have been destroyed");
	//ASSERT_OR_DIE(!engineGraph.ContainsNode(b), "Graph should not contian b");
	//ASSERT_OR_DIE(!engineGraph.ContainsEdge(b, c), "b->c should not exist in graph");

	////test RemoveNode with data
	//engineGraph.RemoveNode(AData);
	//ASSERT_OR_DIE(engineGraph.GetNumNodes() == 2, "Num nodes should be 2");
	//ASSERT_OR_DIE(engineGraph.GetNumEdges() == 1, "Edges attached to a should have been destroyed");
	//ASSERT_OR_DIE(!engineGraph.ContainsNode(a), "Graph should not contian b");
	//ASSERT_OR_DIE(!engineGraph.ContainsEdge(a, d), "a->d should not exist in graph");

	////test graph clearing
	//engineGraph.Clear();
	//ASSERT_OR_DIE(engineGraph.GetNumNodes() == 0, "Num nodes should be 0");
	//ASSERT_OR_DIE(engineGraph.GetNumEdges() == 0, "Num nodes should be 0");

	//a = engineGraph.AddNode(StoryData("A", 1.f));
	//b = engineGraph.AddNode(StoryData("B", 2.f));
	//c = engineGraph.AddNode(StoryData("C", 3.f));
	//d = engineGraph.AddNode(StoryData("D", 4.f));

	//engineGraph.AddEdge(a,b, 10.f);
	//engineGraph.AddEdge(b,c, 1.f);
	//engineGraph.AddEdge(b,d, 3.f);
	//engineGraph.AddEdge(a,d, 2.f);
	//engineGraph.AddEdge(c,d, 5.f);

}

void Game::StoryGraphTests()
{
	unsigned int numChars = m_graph.GetNumCharacters();

	//ASSERT_OR_DIE(m_graph.GetNumNodes() == 0, "Num nodes should be 0");
	//StoryData* AData = new StoryData("A", 1.f);
	//StoryNode* a = m_graph.AddNode(AData);
	////Test graph addition
	//ASSERT_OR_DIE(m_graph.GetNumNodes() == 1, "Num nodes should be 1");

	////Test ContainsNode
	//ASSERT_OR_DIE(m_graph.ContainsNode(a), "ContainsNode not working with node");
	//ASSERT_OR_DIE(m_graph.ContainsNode(AData), "ContainsNode not working with data");

	////Test for duplicates
	//m_graph.AddNode(a);
	//ASSERT_OR_DIE(m_graph.GetNumNodes() == 1, "Num nodes should be 1");
	//ASSERT_OR_DIE(m_graph.ContainsNode(a), "ContainsNode not working with node");

	////add new nodes to graph
	//StoryNode* b = m_graph.AddNode(new StoryData("B", 2.f));
	//StoryNode* c = m_graph.AddNode(new StoryData("C", 3.f));
	//StoryNode* d = m_graph.AddNode(new StoryData("D", 4.f));
	//ASSERT_OR_DIE(m_graph.GetNumNodes() == 4, "Num nodes should be 4");

	//ASSERT_OR_DIE(m_graph.GetNumEdges() == 0, "Num edges should be 0");
	//StoryEdge* testEdge = m_graph.AddEdge(a,b, new StoryState(0.f, numChars));
	//ASSERT_OR_DIE(m_graph.GetNumEdges() == 1, "Num edges should be 1");

	////test ContainsEdge
	//ASSERT_OR_DIE(m_graph.ContainsEdge(a, b), "ContainsEdge not working with edges");
	//ASSERT_OR_DIE(m_graph.ContainsEdge(testEdge), "ContainsEdge not working with edges");

	//m_graph.AddEdge(b,c, new StoryState(1.f, numChars));
	//m_graph.AddEdge(b,d, new StoryState(3.f, numChars));
	//m_graph.AddEdge(a,d, new StoryState(2.f, numChars));
	//m_graph.AddEdge(c,d, new StoryState(5.f, numChars));

	//ASSERT_OR_DIE(m_graph.GetNumEdges() == 5, "Num edges should be 5");

	////Test Removals
	//m_graph.RemoveAndDeleteEdge(b,c);
	//ASSERT_OR_DIE(m_graph.GetNumEdges() == 4, "Num edges should be 4");
	//ASSERT_OR_DIE(!m_graph.ContainsEdge(b, c), "Edge was not removed");
	//ASSERT_OR_DIE(m_graph.ContainsNode(b), "ContainsNode not working with node");
	//ASSERT_OR_DIE(m_graph.ContainsNode(c), "ContainsNode not working with node");

	////return graph to previous state
	//m_graph.AddEdge(b,c);
	//ASSERT_OR_DIE(m_graph.GetNumEdges() == 5, "Num edges should be 5");

	////test DeleteNode with node
	//m_graph.RemoveAndDeleteNode(b);
	//ASSERT_OR_DIE(m_graph.GetNumNodes() == 3, "Num nodes should be 3");
	//ASSERT_OR_DIE(m_graph.GetNumEdges() == 2, "Edges attached to b should have been destroyed");
	//ASSERT_OR_DIE(!m_graph.ContainsNode(b), "Graph should not contian b");
	//ASSERT_OR_DIE(!m_graph.ContainsEdge(b, c), "b->c should not exist in graph");

	////test DeleteNode with data
	//m_graph.RemoveAndDeleteNode(AData);
	//ASSERT_OR_DIE(m_graph.GetNumNodes() == 2, "Num nodes should be 2");
	//ASSERT_OR_DIE(m_graph.GetNumEdges() == 1, "Edges attached to a should have been destroyed");
	//ASSERT_OR_DIE(!m_graph.ContainsNode(a), "Graph should not contian b");
	//ASSERT_OR_DIE(!m_graph.ContainsEdge(a, d), "a->d should not exist in graph");

	//m_graph.Clear();
	//ASSERT_OR_DIE(m_graph.GetNumNodes() == 0, "Num nodes should be 0");
	//ASSERT_OR_DIE(m_graph.GetNumEdges() == 0, "Num nodes should be 0");

	//a = m_graph.AddNode(new StoryData("A", 1.f));
	//b = m_graph.AddNode(new StoryData("B", 2.f));
	//c = m_graph.AddNode(new StoryData("C", 3.f));
	//d = m_graph.AddNode(new StoryData("D", 4.f));
	//StoryNode* ee = m_graph.AddNode(new StoryData("E", 5.f));
	//StoryNode* f = m_graph.AddNode(new StoryData("F", 1.f));

	//m_graph.AddEdge(a,b,   new StoryState(10.f, numChars));
	//m_graph.AddEdge(b,c,   new StoryState(1.f , numChars));
	//m_graph.AddEdge(b,d,   new StoryState(3.f , numChars));
	//m_graph.AddEdge(a,d,   new StoryState(2.f , numChars));
	//m_graph.AddEdge(c,d,   new StoryState(5.f , numChars));
	//m_graph.AddEdge(b, ee, new StoryState(3.f , numChars));
	//m_graph.AddEdge(ee, d);
	//m_graph.AddEdge(a, f, new StoryState(1.f, numChars));
	//m_graph.AddEdge(f, c, new StoryState(6.f, numChars));
	////m_graph.AddEdge(f, d, 2.f);

	//m_graph.AddStart(a);
	//m_graph.AddEnd(d);

	//GetRandomFloatInRange(.3f, .7f);
	//b->m_data->SetPosition( Vector2(GetRandomFloatInRange(.4f, .6f), GetRandomFloatInRange(.4f, .6f)));
	//c->m_data->SetPosition( Vector2(GetRandomFloatInRange(.4f, .6f), GetRandomFloatInRange(.4f, .6f)));
	//ee->m_data->SetPosition(Vector2(GetRandomFloatInRange(.4f, .6f), GetRandomFloatInRange(.4f, .6f)));
	//f->m_data->SetPosition( Vector2(GetRandomFloatInRange(.4f, .6f), GetRandomFloatInRange(.4f, .6f)));



	//m_graph.RunNodeAdjustments();
}









Game* g_theGame = nullptr;