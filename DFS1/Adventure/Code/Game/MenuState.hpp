#pragma once
#include "Game/GameCommon.hpp"

class GameState_Paused;
class Menu;

class MenuState{
public:
	MenuState();
	MenuState(GameState_Paused* pauseState, AABB2 bounds);
	virtual void RenderBackground() = 0;
	virtual void RenderContent() = 0;
	virtual void Update(float ds);
	virtual void HandleInput();
	AABB2 m_bounds;
	GameState_Paused* m_pauseState;
};


class MenuState_Paused : public MenuState{
public:
	MenuState_Paused(GameState_Paused* pauseState, AABB2 bounds);
	void RenderBackground() override;
	void RenderContent() override;
	void HandleInput() override;

	
};


class MenuState_Inventory : public MenuState{
public:
	MenuState_Inventory(GameState_Paused* pauseState, AABB2 bounds);
	void Update(float ds) override;
	void RenderBackground() override;
	void RenderContent() override;
	void HandleInput() override;

	Menu* m_menu;
};

class MenuState_Map : public MenuState{
public:
	MenuState_Map(GameState_Paused* pauseState, AABB2 bounds);
	void RenderBackground() override;
	void RenderContent() override;
	void HandleInput() override;
};