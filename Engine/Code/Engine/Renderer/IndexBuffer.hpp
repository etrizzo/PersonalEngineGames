#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

class IndexBuffer : public RenderBuffer{
public:
	IndexBuffer(){ m_handle = NULL; m_bufferSize = NULL; m_stride = 1;};

	bool SetIndices(size_t const byte_count, void const * data);

	size_t m_stride;
};