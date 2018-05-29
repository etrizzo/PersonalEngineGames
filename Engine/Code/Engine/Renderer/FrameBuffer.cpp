#include "FrameBuffer.hpp"

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers( 1, &m_handle ); 
}

FrameBuffer::~FrameBuffer()
{
	if (m_handle != NULL) {
		glDeleteFramebuffers( 1, &m_handle ); 
		m_handle = NULL; 
	}
}

void FrameBuffer::SetColorTarget(Texture * color_target)
{
	m_colorTarget = color_target;
}

void FrameBuffer::SetDepthStencilTarget(Texture * depth_target)
{
	m_depthStencilTarget = depth_target;
}

bool FrameBuffer::Finalize()
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_handle );		//set up the frame buffer we're actually going to draw to - this one

	// keep track of which outputs go to which attachments; 
	GLenum targets[1]; 

	// Bind a color target to an attachment point
	// and keep track of which locations to to which attachments. 
	glFramebufferTexture( GL_FRAMEBUFFER, 
		GL_COLOR_ATTACHMENT0 + 0,		//determines which color target
		m_colorTarget->GetHandle(),
		0); 
	// 0 to to attachment 0
	targets[0] = GL_COLOR_ATTACHMENT0 + 0;		//telling where the bind point goes to

	// Update target bindings - syncs up the bindings you define in the vertex shader and the things you define for the frame buffers
	glDrawBuffers( 1, targets ); 

	// binds the depth target to the currently bound frame buffer if available;
	if (m_depthStencilTarget == nullptr) {
		glFramebufferTexture( GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			NULL ,
			0); 
	} else {
		glFramebufferTexture( GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			m_depthStencilTarget->GetHandle(),
			0); 
	}

	// Error Check - recommend only doing in debug
	#if defined(_DEBUG)
		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			DebuggerPrintf( "Failed to create framebuffer:  %u", status );
			return false;
		}
	#endif

	return true;
}
