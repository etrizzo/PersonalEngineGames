#include "Game/GameCommon.hpp"

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
SpriteSheet* g_tileSpriteSheet = nullptr;


bool RightJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_RIGHT) || g_theInput->WasKeyJustPressed('D') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_D_RIGHT));
}

bool LeftJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_LEFT) || g_theInput->WasKeyJustPressed('A') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_D_LEFT));
}

bool UpJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_UP) || g_theInput->WasKeyJustPressed('W') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_D_UP));
}


bool DownJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_DOWN) || g_theInput->WasKeyJustPressed('S') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_D_DOWN));
}

bool IsRightDown()
{
	return (g_theInput->IsKeyDown(VK_RIGHT) || g_theInput->IsKeyDown('D') || g_theInput->GetController(0)->IsButtonDown(XBOX_D_RIGHT));
}

bool IsLeftDown()
{
	return (g_theInput->IsKeyDown(VK_LEFT) || g_theInput->IsKeyDown('A') || g_theInput->GetController(0)->IsButtonDown(XBOX_D_LEFT));
}

bool IsUpDown()
{
	return (g_theInput->IsKeyDown(VK_UP) || g_theInput->IsKeyDown('W') || g_theInput->GetController(0)->IsButtonDown(XBOX_D_UP));
}

bool IsDownDown()
{
	return (g_theInput->IsKeyDown(VK_DOWN) || g_theInput->IsKeyDown('S') || g_theInput->GetController(0)->IsButtonDown(XBOX_D_DOWN));
}

bool AcceptJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_RETURN) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_A));
}

bool BackJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_BACK) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_B));
}