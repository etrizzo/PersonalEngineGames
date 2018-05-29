#include "Engine/Renderer/Sampler.hpp"

Sampler::Sampler() 
	: m_samplerHandle(NULL)
{
	Create();
}

Sampler::~Sampler()
{
	Destroy();
}

bool Sampler::Create()
{
	// create the sampler handle if needed; 
	if (m_samplerHandle == NULL) {
		glGenSamplers( 1, &m_samplerHandle ); 
		if (m_samplerHandle == NULL) {
			return false; 
		}
	}

	// setup wrapping
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_S, GL_REPEAT );  
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_T, GL_REPEAT );  
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_R, GL_REPEAT );  

	// filtering; 
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	return true; 
}

void Sampler::Destroy()
{
	if (m_samplerHandle != NULL) {
		glDeleteSamplers( 1, &m_samplerHandle ); 
		m_samplerHandle = NULL; 
	}
} 