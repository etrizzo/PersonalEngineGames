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
	/*glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
*/
// filtering; (trilinear) - really - if you want mips with nearest, siwtch the GL_LINEAR_* to GL_NEAREST_*
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );         // Default: GL_LINEAR
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );         // Default: GL_LINEAR

	// Limit where on the LOD I can actually fetch (if you want to control it through the sampler)
	// defaults to -1000.0f to 1000.0f
	glSamplerParameterf( m_samplerHandle, GL_TEXTURE_MIN_LOD, (float) -1000.f ); 
	glSamplerParameterf( m_samplerHandle, GL_TEXTURE_MAX_LOD, (float) 1000.f ); 


	// To constrain

	return true; 
}

void Sampler::Destroy()
{
	if (m_samplerHandle != NULL) {
		glDeleteSamplers( 1, &m_samplerHandle ); 
		m_samplerHandle = NULL; 
	}
} 