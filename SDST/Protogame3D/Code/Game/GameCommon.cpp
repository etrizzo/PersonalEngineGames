#include "Game/GameCommon.hpp"

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
SpriteSheet* g_tileSpriteSheet = nullptr;


bool RightJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_RIGHT) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_D_RIGHT));
}

bool LeftJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_LEFT) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_D_LEFT));
}

bool UpJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_UP) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_D_UP));
}

bool DownJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_DOWN) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_D_DOWN));
}

bool AcceptJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_RETURN) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_A));
}

bool BackJustPressed()
{
	return (g_theInput->WasKeyJustPressed(VK_BACK) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_B));
}