#pragma once
#include "Engine/Renderer/external/gl/glcorearb.h"
#include "Engine/Renderer/external/gl/glext.h"
#include "Engine/Renderer/external/gl/wglext.h"


#define GL_CHECK_ERROR()  GLCheckError( __FILE__, __LINE__ )



//------------------------------------------------------------------------
bool GLCheckError( char const *file, int line );

//------------------------------------------------------------------------
bool GLFailed();

//------------------------------------------------------------------------
bool GLSucceeded();



/*
----------------------------------------------------
	Macro/Template used for binding glfunctions
----------------------------------------------------
*/


// Use this to deduce type of the pointer so we can cast; 
template <typename T>
bool wglGetTypedProcAddress( T *out, char const *name ) 
{
	// Grab the function from the currently bound render contect
	// most opengl 2.0+ features will be found here
	*out = (T) wglGetProcAddress(name); 

	if ((*out) == nullptr) {
		// if it is not part of wgl (the device), then attempt to get it from the GLL library
		// (most OpenGL functions come from here)
		*out = (T) GetProcAddress( gGLLibrary, name); 
	}

	return (*out != nullptr); 
}

#define GL_BIND_FUNCTION(f)      wglGetTypedProcAddress( &f, #f )

/*
----------------------------------------------------
					GLFunctions
----------------------------------------------------
*/



extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

//clear
extern PFNGLCLEARPROC		glClear;
extern PFNGLCLEARCOLORPROC	glClearColor;

//shader creation
extern PFNGLCREATESHADERPROC		glCreateShader		;
extern PFNGLSHADERSOURCEPROC		glShaderSource		;
extern PFNGLCOMPILESHADERPROC		glCompileShader		;
extern PFNGLGETSHADERIVPROC			glGetShaderiv		;
extern PFNGLDELETESHADERPROC		glDeleteShader		;
extern PFNGLGETSHADERINFOLOGPROC	glGetShaderInfoLog	;

//shader program creation
extern PFNGLCREATEPROGRAMPROC		glCreateProgram		;
extern PFNGLATTACHSHADERPROC		glAttachShader		;
extern PFNGLLINKPROGRAMPROC			glLinkProgram		;
extern PFNGLGETPROGRAMIVPROC		glGetProgramiv		;
extern PFNGLDELETEPROGRAMPROC		glDeleteProgram		;
extern PFNGLDETACHSHADERPROC		glDetachShader		;
extern PFNGLGETPROGRAMINFOLOGPROC	glGetProgramInfoLog	;

//render buffer
extern PFNGLGENBUFFERSPROC		glGenBuffers	;
extern PFNGLBINDBUFFERPROC		glBindBuffer	;
extern PFNGLBUFFERDATAPROC		glBufferData	;
extern PFNGLDELETEBUFFERSPROC	glDeleteBuffers	;
extern PFNGLGENVERTEXARRAYSPROC	glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC	glBindVertexArray;

//drawing mesh
extern PFNGLGETATTRIBLOCATIONPROC		glGetAttribLocation			;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray	;
extern PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer		;
extern PFNGLUSEPROGRAMPROC				glUseProgram				;
extern PFNGLDRAWARRAYSPROC				glDrawArrays				;
extern PFNGLDRAWELEMENTSPROC			glDrawElements				;

//gl error checking
extern PFNGLGETERRORPROC	glGetError;

//binding uniform shit
extern PFNGLGETUNIFORMLOCATIONPROC	glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC	glUniformMatrix4fv	;
extern PFNGLPROGRAMUNIFORM3FVPROC	glProgramUniform3fv	;
extern PFNGLPROGRAMUNIFORM4FVPROC	glProgramUniform4fv	;
extern PFNGLPROGRAMUNIFORM1FPROC	glProgramUniform1f	;

//Sampler
extern PFNGLGENSAMPLERSPROC			glGenSamplers		;
extern PFNGLSAMPLERPARAMETERIPROC	glSamplerParameteri	;
extern PFNGLDELETESAMPLERSPROC		glDeleteSamplers	;
extern PFNGLBINDSAMPLERPROC			glBindSampler		;
extern PFNGLACTIVETEXTUREPROC		glActiveTexture		;
extern PFNGLBINDTEXTUREPROC			glBindTexture		;
extern PFNGLDISABLEPROC				glDisable			;

//blending
extern PFNGLBLENDFUNCPROC		glBlendFunc	;
extern PFNGLENABLEPROC			glEnable	;

//drawing Primitives
extern PFNGLLINEWIDTHPROC		glLineWidth;

//textures
extern PFNGLPIXELSTOREIPROC		glPixelStorei	;
extern PFNGLGENTEXTURESPROC		glGenTextures	;
extern PFNGLTEXPARAMETERIPROC	glTexParameteri	;
extern PFNGLTEXIMAGE2DPROC		glTexImage2D	;
extern PFNGLTEXSUBIMAGE2DPROC	glTexSubImage2D ;
extern PFNGLTEXSTORAGE2DPROC	glTexStorage2D	;
extern PFNGLDELETETEXTURESPROC	glDeleteTextures;

//frame buffers
extern PFNGLGENFRAMEBUFFERSPROC		glGenFramebuffers		;
extern PFNGLDELETEFRAMEBUFFERSPROC	glDeleteFramebuffers	;
extern PFNGLBINDFRAMEBUFFERPROC		glBindFramebuffer		;
extern PFNGLFRAMEBUFFERTEXTUREPROC	glFramebufferTexture	;
extern PFNGLDRAWBUFFERSPROC			glDrawBuffers			;
extern PFNGLBLITFRAMEBUFFERPROC		glBlitFramebuffer		;
extern PFNGLDEPTHFUNCPROC			glDepthFunc				;
extern PFNGLDEPTHMASKPROC			glDepthMask				;
extern PFNGLCLEARDEPTHFPROC			glClearDepthf			;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC	glCheckFramebufferStatus	;

extern PFNGLBINDBUFFERBASEPROC		glBindBufferBase	;

extern PFNGLPOLYGONMODEPROC			glPolygonMode	;
extern PFNGLCULLFACEPROC			glCullFace		;



////setting uniforms
extern PFNGLUNIFORM1FVPROC		glUniform1fv	;
extern PFNGLUNIFORM4FVPROC		glUniform4fv	;
