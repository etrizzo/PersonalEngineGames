#pragma once
#include "Game/GameState.hpp"
#include "Game/GameCommon.hpp"


class Player;
class ForwardRenderPath;
class RenderScene;

class GameState_Playing: public GameState{
public:
	GameState_Playing();

	Player* m_player;
	//Light* m_cameraLight;
	//std::vector<Light*> m_lights = std::vector<Light*>();

	ForwardRenderPath* m_renderPath;
	RenderScene* m_scene;


	void EnterState() override;
	void Update(float ds);
	void RenderUI();
	void HandleInput();
	

	bool m_gameWon = false;
	bool m_gameLost = false;
	bool m_playing = false;
	
protected:
	void RenderGraph() const;
	void UpdateGraph();
};