#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/glfunctions.h"

class RenderBuffer{
public:
	RenderBuffer(){ m_handle = NULL; m_bufferSize = NULL;};
	~RenderBuffer();

	//bool UpdateFromCPU( size_t const byte_count, void const* data);
	bool CopyToGPU(size_t const byte_count, void const * data);

	GLuint GetHandle() const;
	size_t GetBufferSize() const;

protected:
	GLuint m_handle;
	size_t m_bufferSize;

};