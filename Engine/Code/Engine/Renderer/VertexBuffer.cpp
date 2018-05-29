#include "VertexBuffer.hpp"

bool VertexBuffer::SetVertices(size_t const byte_count, void const * data)
{
	return RenderBuffer::CopyToGPU(byte_count, data);
}

void VertexBuffer::SetStride(size_t newStride)
{
	m_stride = newStride;
}
