#pragma once
#include "Engine/Math/Renderer.hpp"

class FrameBuffer
{
public:
	FrameBuffer(); 
	~FrameBuffer();

	// should just update members
	// finalize does the actual binding
	void SetColorTarget( Texture* color_target ); 
	void SetDepthStencilTarget( Texture* depth_target ); 

	// setups the the GL framebuffer - called before us. 
	// TODO: Make sure this only does work if the targets
	// have changed.
	bool Finalize(); 

	GLuint GetHandle() { return m_handle; };
	unsigned int GetWidth() { return m_colorTarget->GetWidth(); };
	unsigned int GetHeight() { return m_colorTarget->GetHeight(); };

public:
	GLuint m_handle; 
	Texture* m_colorTarget; 
	Texture* m_depthStencilTarget; 
};