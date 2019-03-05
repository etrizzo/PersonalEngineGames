#include "FlyoutText.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"

FlyoutText::FlyoutText(std::string text, Vector3 startPos, float textHeight, float lengthInSeconds, RGBA tint, float speed)
{
	m_text = text;
	m_pos = startPos;
	m_tint = tint;
	m_textHeight = textHeight;
	m_lengthInSeconds = lengthInSeconds;
	m_speed = speed;
}

void FlyoutText::Update(float deltaSeconds)
{
	m_pos+= Vector3(0.f, deltaSeconds * m_speed, 0.f);
	m_ageInSeconds+=deltaSeconds;

	if (m_ageInSeconds > m_lengthInSeconds){
		m_aboutToBeDeleted = true;
	}
}

void FlyoutText::Render()
{
	g_theRenderer->BindShaderProgram("stroke");
	g_theRenderer->DrawTextAsSprite(m_text, m_pos, Vector2::HALF, m_textHeight, g_theGame->m_camera->GetRight(), UP, m_tint);
	g_theRenderer->ReleaseShader();
}
