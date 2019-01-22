#pragma once
#include "Game/GameCommon.hpp"

class Dialogue{
public:
	Dialogue(std::string content);

	void Render(const AABB2& box);

	std::string m_content;


	
};