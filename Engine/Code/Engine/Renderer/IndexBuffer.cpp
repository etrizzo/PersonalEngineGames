#include "IndexBuffer.hpp"

//should bind to the element array buffer instead of the array buffer according to the internet?
//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
bool IndexBuffer::SetIndices(size_t const byte_count, void const * data)
{
	// handle is a GLuint member - used by OpenGL to identify this buffer
	// if we don't have one, make one when we first need it [lazy instantiation]
	if (m_handle == NULL) {
		glGenBuffers( 1, &m_handle ); 
	}

	// Bind the buffer to a slot, and copy memory
	// GL_DYNAMIC_DRAW means the memory is likely going to change a lot (we'll get
	// during the second project)
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_handle ); 
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, byte_count * sizeof(unsigned int), data, GL_STATIC_DRAW ); 

	// buffer_size is a size_t member variable I keep around for 
	// convenience
	m_bufferSize = byte_count; 
	return true; 




	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
}
