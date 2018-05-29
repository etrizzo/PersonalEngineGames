#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Renderer.hpp"


class Sampler{
public:
	Sampler();
	~Sampler();

	void Destroy();
	bool Create();
	GLuint GetHandle() const { return m_samplerHandle; };

	GLuint m_samplerHandle;


};