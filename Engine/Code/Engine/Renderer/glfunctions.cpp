#pragma once
#include "glfunctions.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"


//------------------------------------------------------------------------
bool GLCheckError( char const *file, int line )
{
#if defined(_DEBUG)
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		DebuggerPrintf( "GL ERROR [0x%04x] at [%s(%i)]", error, file, line );
		ConsolePrintf( "GL ERROR [0x%04x] at [%s(%i)]", error, file, line );
		return true; 
	}
#endif
	return false; 
}

//------------------------------------------------------------------------
bool GLFailed()
{
	return GLCheckError(NULL, NULL); 
}

//------------------------------------------------------------------------
bool GLSucceeded()
{
	return !GLFailed();
}


PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

//clear
PFNGLCLEARPROC			glClear			= nullptr;
PFNGLCLEARCOLORPROC		glClearColor	= nullptr;

//shader creation
PFNGLCREATESHADERPROC		glCreateShader		= nullptr;
PFNGLSHADERSOURCEPROC		glShaderSource		= nullptr;
PFNGLCOMPILESHADERPROC		glCompileShader		= nullptr;
PFNGLGETSHADERIVPROC		glGetShaderiv		= nullptr;
PFNGLDELETESHADERPROC		glDeleteShader		= nullptr;
PFNGLGETSHADERINFOLOGPROC	glGetShaderInfoLog	= nullptr;

//shader program creation
PFNGLCREATEPROGRAMPROC		glCreateProgram		= nullptr;
PFNGLATTACHSHADERPROC		glAttachShader		= nullptr;
PFNGLLINKPROGRAMPROC		glLinkProgram		= nullptr;
PFNGLGETPROGRAMIVPROC		glGetProgramiv		= nullptr;
PFNGLDELETEPROGRAMPROC		glDeleteProgram		= nullptr;
PFNGLDETACHSHADERPROC		glDetachShader		= nullptr;
PFNGLGETPROGRAMINFOLOGPROC	glGetProgramInfoLog	= nullptr;

//render buffers
PFNGLGENBUFFERSPROC			glGenBuffers		= nullptr;
PFNGLBINDBUFFERPROC			glBindBuffer		= nullptr;
PFNGLBUFFERDATAPROC			glBufferData		= nullptr;
PFNGLDELETEBUFFERSPROC		glDeleteBuffers		= nullptr;
PFNGLGENVERTEXARRAYSPROC	glGenVertexArrays	= nullptr;
PFNGLBINDVERTEXARRAYPROC	glBindVertexArray	= nullptr;

//draw mesh immediate
PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation			= nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray	= nullptr;
PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer		= nullptr;
PFNGLUSEPROGRAMPROC					glUseProgram				= nullptr;
PFNGLDRAWARRAYSPROC					glDrawArrays				= nullptr;
PFNGLDRAWELEMENTSPROC				glDrawElements				= nullptr;

//gl error checking
PFNGLGETERRORPROC	glGetError	= nullptr;

//gl ortho junk
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation	= nullptr;
PFNGLUNIFORMMATRIX4FVPROC	glUniformMatrix4fv		= nullptr;
PFNGLPROGRAMUNIFORM3FVPROC	glProgramUniform3fv		= nullptr;
PFNGLPROGRAMUNIFORM4FVPROC	glProgramUniform4fv		= nullptr;
PFNGLPROGRAMUNIFORM1FPROC	glProgramUniform1f		= nullptr;


//sampler
PFNGLGENSAMPLERSPROC			glGenSamplers		= nullptr;
PFNGLSAMPLERPARAMETERIPROC		glSamplerParameteri	= nullptr;
PFNGLDELETESAMPLERSPROC			glDeleteSamplers	= nullptr;
PFNGLBINDSAMPLERPROC			glBindSampler		= nullptr;
PFNGLACTIVETEXTUREPROC			glActiveTexture		= nullptr;
PFNGLBINDTEXTUREPROC			glBindTexture		= nullptr;
PFNGLDISABLEPROC				glDisable			= nullptr;
PFNGLGENERATEMIPMAPPROC			glGenerateMipmap	= nullptr;
PFNGLSAMPLERPARAMETERFPROC		glSamplerParameterf	= nullptr;

//blending
PFNGLBLENDFUNCPROC		glBlendFunc	 = nullptr;
PFNGLENABLEPROC			glEnable	 = nullptr;

//drawing primitives
PFNGLLINEWIDTHPROC		glLineWidth =  nullptr;

//textures
PFNGLPIXELSTOREIPROC		glPixelStorei	= nullptr;
PFNGLGENTEXTURESPROC		glGenTextures	= nullptr;
PFNGLTEXPARAMETERIPROC		glTexParameteri	= nullptr;
PFNGLTEXIMAGE2DPROC			glTexImage2D	= nullptr;
PFNGLTEXSUBIMAGE2DPROC		glTexSubImage2D = nullptr;
PFNGLTEXSTORAGE2DPROC		glTexStorage2D	= nullptr;
PFNGLDELETETEXTURESPROC		glDeleteTextures= nullptr;

//frame buffers
PFNGLGENFRAMEBUFFERSPROC		glGenFramebuffers		= nullptr;
PFNGLDELETEFRAMEBUFFERSPROC		glDeleteFramebuffers	= nullptr;
PFNGLBINDFRAMEBUFFERPROC		glBindFramebuffer		= nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC		glFramebufferTexture	= nullptr;
PFNGLDRAWBUFFERSPROC			glDrawBuffers			= nullptr;
PFNGLBLITFRAMEBUFFERPROC		glBlitFramebuffer		= nullptr;
PFNGLDEPTHFUNCPROC				glDepthFunc				= nullptr;
PFNGLDEPTHMASKPROC				glDepthMask				= nullptr;
PFNGLCLEARDEPTHFPROC			glClearDepthf			= nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC	glCheckFramebufferStatus= nullptr;
PFNGLBINDBUFFERBASEPROC			glBindBufferBase		= nullptr;


PFNGLPOLYGONMODEPROC		glPolygonMode			= nullptr;
PFNGLCULLFACEPROC			glCullFace				= nullptr;



////Uniform binding
PFNGLUNIFORM1FVPROC		glUniform1fv	= nullptr;
PFNGLUNIFORM4FVPROC		glUniform4fv	= nullptr;
