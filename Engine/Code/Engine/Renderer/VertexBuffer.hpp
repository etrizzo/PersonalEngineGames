#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Core/Vertex.hpp"

class VertexBuffer : public RenderBuffer{
public:
	VertexBuffer(VertexLayout layout = Vertex3D_PCU::LAYOUT){ m_handle = NULL; m_bufferSize = NULL; m_stride = layout.m_stride;};

	bool SetVertices(size_t const byte_count, void const * data);
	void SetStride(size_t newStride);

	size_t m_stride;
};