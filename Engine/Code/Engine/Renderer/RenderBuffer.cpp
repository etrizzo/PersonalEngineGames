#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/glfunctions.h"

RenderBuffer::~RenderBuffer()
{
	// cleanup for a buffer; 
	if (m_handle != NULL) {
		glDeleteBuffers( 1, &m_handle ); 
		m_handle = NULL; 
	}
}



bool RenderBuffer::CopyToGPU( size_t const byte_count, void const *data ) 
{
	// handle is a GLuint member - used by OpenGL to identify this buffer
	// if we don't have one, make one when we first need it [lazy instantiation]
	if (m_handle == NULL) {
		glGenBuffers( 1, &m_handle ); 
	}

	// Bind the buffer to a slot, and copy memory
	// GL_DYNAMIC_DRAW means the memory is likely going to change a lot 
	glBindBuffer( GL_ARRAY_BUFFER, m_handle ); 
	glBufferData( GL_ARRAY_BUFFER, byte_count, data, GL_DYNAMIC_DRAW ); 

	// buffer_size is a size_t member variable I keep around for 
	// convenience
	m_bufferSize = byte_count; 
	GL_CHECK_ERROR();
	return true; 
}

GLuint RenderBuffer::GetHandle() const
{
	return m_handle;
}

size_t RenderBuffer::GetBufferSize() const
{
	return m_bufferSize;
}
