#pragma once
#include "Game/GameState.hpp"
#include "Game/GameCommon.hpp"

class Camera;
class DebugRenderSystem;
class Entity;
class Map;

class GameState_Playing: public GameState{
public:
	GameState_Playing();
	Map* m_currentMap = nullptr;

	void Update(float ds);
	void RenderGame();
	void RenderUI();
	void HandleInput();

	RenderScene2D* GetScene() override { return m_renderScene;  }
protected:

	RenderScene2D* m_renderScene = nullptr;		//debug purposes only

};