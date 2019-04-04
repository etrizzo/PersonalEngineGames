#include "Engine/Math/Renderer.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/ShaderLiterals.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Renderer/Sprite.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/VertexLayout.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Material.hpp"

static HMODULE gGLLibrary  = NULL; 
static HWND gGLwnd         = NULL;    // window our context is attached to; 
static HDC gHDC            = NULL;    // our device context
static HGLRC gGLContext    = NULL;    // our rendering context; 



int g_openGlPrimitiveTypes[ NUM_PRIMITIVE_TYPES ] =
{
	//GL_POINTS,			// called PRIMITIVE_POINTS		in our engine
	GL_LINES,			// called PRIMITIVE_LINES		in our engine
	GL_TRIANGLES,		// called PRIMITIVE_TRIANGES	in our engine
	GL_QUADS			// called PRIMITIVE_QUADS		in our engine
};

Renderer::~Renderer()
{
	for (int i = 0; i < (int) m_loadedTextures.size(); i++){
		delete m_loadedTextures[i];
	}
	for (std::pair<std::string, BitmapFont*> fontElement : m_loadedFonts) {
		delete fontElement.second;
	}
	for (std::pair<std::string, ShaderProgram*> shaderElement : m_loadedShaderPrograms) {
		delete shaderElement.second;
	}
}

Renderer::Renderer()
{
	m_loadedTextures = std::vector<Texture*>();
	m_loadedTexturePaths = std::vector<std::string>();
	m_loadedFonts = std::map<std::string, BitmapFont*>();
	m_loadedShaderPrograms = std::map<std::string, ShaderProgram*>();

	//m_immediateBuffer = new RenderBuffer();
	m_immediateMesh = new SubMesh(Vertex3D_PCU::LAYOUT);

	m_modelMatrix = MatrixStack();
	

}

bool Renderer::Initialize()
{
	// load and get a handle to the opengl dll (dynamic link library)
	gGLLibrary = ::LoadLibraryA( "opengl32.dll" ); 

	// Get the Device Context (DC) - how windows handles the interface to rendering devices
	// This "acquires" the resource - to cleanup, you must have a ReleaseDC(hwnd, hdc) call. 
	HWND hwnd = ::GetActiveWindow();
	HDC hdc = ::GetDC( hwnd );       

	// use the DC to create a rendering context (handle for all OpenGL state - like a pointer)
	// This should be very similar to SD1
	HGLRC temp_context = CreateOldRenderContext( hdc ); 

	::wglMakeCurrent( hdc, temp_context ); 
	BindNewWGLFunctions();  // find the functions we'll need to create the real context; 

							// create the real context, using opengl version 4.2
	HGLRC real_context = CreateRealRenderContext( hdc, 4, 2 ); 

	// Set and cleanup
	::wglMakeCurrent( hdc, real_context ); 
	::wglDeleteContext( temp_context ); 

	// Bind all our OpenGL functions we'll be using.
	BindGLFunctions(); 

	// set the globals
	gGLwnd = hwnd;
	gHDC = hdc; 
	gGLContext = real_context; 


	PostStartup();

	return true; 

}

static HGLRC CreateOldRenderContext( HDC hdc ) 
{
	// Setup the output to be able to render how we want
	// (in our case, an RGBA (4 bytes per channel) output that supports OpenGL
	// and is double buffered
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof(pfd) ); 
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 0; // 24; Depth/Stencil handled by FBO
	pfd.cStencilBits = 0; // 8; DepthStencil handled by FBO
	pfd.iLayerType = PFD_MAIN_PLANE; // ignored now according to MSDN

									 // Find a pixel format that matches our search criteria above. 
	int pixel_format = ::ChoosePixelFormat( hdc, &pfd );
	if ( pixel_format == NULL ) {
		return NULL; 
	}

	// Set our HDC to have this output. 
	if (!::SetPixelFormat( hdc, pixel_format, &pfd )) {
		return NULL; 
	}

	// Create the context for the HDC
	HGLRC context = wglCreateContext( hdc );
	if (context == NULL) {
		return NULL; 
	}

	// return the context; 
	return context; 
}

static HGLRC CreateRealRenderContext( HDC hdc, int major, int minor ) 
{
	// So similar to creating the temp one - we want to define 
	// the style of surface we want to draw to.  But now, to support
	// extensions, it takes key_value pairs
	int const format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,    // The rc will be used to draw to a window
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,    // ...can be drawn to by GL
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,     // ...is double buffered
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // ...uses a RGBA texture
		WGL_COLOR_BITS_ARB, 24,             // 24 bits for color (8 bits per channel)
											// WGL_DEPTH_BITS_ARB, 24,          // if you wanted depth a default depth buffer...
											// WGL_STENCIL_BITS_ARB, 8,         // ...you could set these to get a 24/8 Depth/Stencil.
											NULL, NULL,                         // Tell it we're done.
	};

	// Given the above criteria, we're going to search for formats
	// our device supports that give us it.  I'm allowing 128 max returns (which is overkill)
	size_t const MAX_PIXEL_FORMATS = 128;
	int formats[MAX_PIXEL_FORMATS];
	int pixel_format = 0;
	UINT format_count = 0;

	BOOL succeeded = wglChoosePixelFormatARB( hdc, 
		format_attribs, 
		nullptr, 
		MAX_PIXEL_FORMATS, 
		formats, 
		(UINT*)&format_count );

	if (!succeeded) {
		return NULL; 
	}

	// Loop through returned formats, till we find one that works
	for (unsigned int i = 0; i < format_count; ++i) {
		pixel_format = formats[i];
		succeeded = SetPixelFormat( hdc, pixel_format, NULL ); // same as the temp context; 
		if (succeeded) {
			break;
		} else {
			DWORD error = GetLastError();
			DebuggerPrintf( "Failed to set the format: %u", error ); 
		}
	}

	if (!succeeded) {
		return NULL; 
	}

	// Okay, HDC is setup to the right format, now create our GL context

	// First, options for creating a debug context (potentially slower, but 
	// driver may report more useful errors). 
	int context_flags = 0; 
#if defined(_DEBUG)
	context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB; 
#endif

	// describe the context
	int const attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,                             // Major GL Version
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,                             // Minor GL Version
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,   // Restrict to core (no compatibility)
		WGL_CONTEXT_FLAGS_ARB, context_flags,                             // Misc flags (used for debug above)
		0, 0
	};

	// Try to create context
	HGLRC context = wglCreateContextAttribsARB( hdc, NULL, attribs );
	if (context == NULL) {
		return NULL; 
	}

	return context;
}

GLenum ToGLCompare(eCompare compare)
{
	switch(compare){
	case(COMPARE_NEVER):
		return GL_NEVER;
		break;
	case (COMPARE_LESS):
		return GL_LESS;
		break;
	case (COMPARE_LEQUAL):
		return GL_LEQUAL;
		break;
	case (COMPARE_GREATER):
		return GL_GREATER;
		break;
	case (COMPARE_GEQUAL):
		return GL_GEQUAL;
		break;
	case (COMPARE_EQUAL):
		return GL_EQUAL;
		break;
	case (COMPARE_NOT_EQUAL):
		return GL_NOTEQUAL;
		break;
	case (COMPARE_ALWAYS):
		return GL_ALWAYS;
		break;
	default:
		return 0;
	}
}

GLenum ToGLType(eRenderDataType type)
{
	switch(type){
	case (RDT_FLOAT):
		return GL_FLOAT;
		break;
	case (RDT_UNSIGNED_BYTE):
		return GL_UNSIGNED_BYTE;
		break;
	}
	return 0;
}




void Renderer::BindNewWGLFunctions()
{
	GL_BIND_FUNCTION( wglGetExtensionsStringARB ); 
	GL_BIND_FUNCTION( wglChoosePixelFormatARB ); 
	GL_BIND_FUNCTION( wglCreateContextAttribsARB );
}



void Renderer::BindGLFunctions()
{
	GL_BIND_FUNCTION( glClear ); 
	GL_BIND_FUNCTION( glClearColor );

	//shader creation
	GL_BIND_FUNCTION( glCreateShader	);	
	GL_BIND_FUNCTION( glShaderSource	);	
	GL_BIND_FUNCTION( glCompileShader	);	
	GL_BIND_FUNCTION( glGetShaderiv		);
	GL_BIND_FUNCTION( glDeleteShader	);	
	GL_BIND_FUNCTION( glGetShaderInfoLog);	
	
	//shader program creation
	GL_BIND_FUNCTION( glCreateProgram		);
	GL_BIND_FUNCTION( glAttachShader		);
	GL_BIND_FUNCTION( glLinkProgram			);
	GL_BIND_FUNCTION( glGetProgramiv		);
	GL_BIND_FUNCTION( glDeleteProgram		);
	GL_BIND_FUNCTION( glDetachShader		);
	GL_BIND_FUNCTION( glGetProgramInfoLog	);

	//render buffer
	GL_BIND_FUNCTION( glGenBuffers		);
	GL_BIND_FUNCTION( glBindBuffer		);
	GL_BIND_FUNCTION( glBufferData		);
	GL_BIND_FUNCTION( glDeleteBuffers	);
	GL_BIND_FUNCTION( glGenVertexArrays	);
	GL_BIND_FUNCTION( glBindVertexArray	);
	GL_BIND_FUNCTION( glViewport)		;

	//draw mesh immediate
	GL_BIND_FUNCTION( glGetAttribLocation		);	
	GL_BIND_FUNCTION( glEnableVertexAttribArray	);
	GL_BIND_FUNCTION( glVertexAttribPointer		);
	GL_BIND_FUNCTION( glUseProgram				);
	GL_BIND_FUNCTION( glDrawArrays				);
	GL_BIND_FUNCTION( glDrawElements			);

	//gl error checking
	GL_BIND_FUNCTION( glGetError );

	//gl ortho shit
	GL_BIND_FUNCTION( glGetUniformLocation	);
	GL_BIND_FUNCTION( glUniformMatrix4fv	);
	GL_BIND_FUNCTION( glProgramUniform3fv	);
	GL_BIND_FUNCTION( glProgramUniform4fv	);
	GL_BIND_FUNCTION( glProgramUniform1f	);

	//samplers
	GL_BIND_FUNCTION( glGenSamplers			);
	GL_BIND_FUNCTION( glSamplerParameteri	);
	GL_BIND_FUNCTION( glDeleteSamplers		);
	GL_BIND_FUNCTION( glBindSampler			);
	GL_BIND_FUNCTION( glActiveTexture		);
	GL_BIND_FUNCTION( glBindTexture			);
	GL_BIND_FUNCTION( glDisable				);
	GL_BIND_FUNCTION( glGenerateMipmap		);
	GL_BIND_FUNCTION( glSamplerParameterf	);

	//blending
	GL_BIND_FUNCTION( glBlendFunc );	
	GL_BIND_FUNCTION( glEnable	  );

	//drawing primitives
	GL_BIND_FUNCTION( glLineWidth );

	//textures;
	GL_BIND_FUNCTION( glPixelStorei		);
	GL_BIND_FUNCTION( glGenTextures		);
	GL_BIND_FUNCTION( glTexParameteri	);
	GL_BIND_FUNCTION( glTexImage2D		);
	GL_BIND_FUNCTION( glTexSubImage2D	);
	GL_BIND_FUNCTION( glTexStorage2D    );
	GL_BIND_FUNCTION( glDeleteTextures  );

	//frame buffers
	GL_BIND_FUNCTION( glGenFramebuffers	   );
	GL_BIND_FUNCTION( glDeleteFramebuffers );
	GL_BIND_FUNCTION( glBindFramebuffer	   );
	GL_BIND_FUNCTION( glFramebufferTexture );
	GL_BIND_FUNCTION( glDrawBuffers		   );
	GL_BIND_FUNCTION( glBlitFramebuffer	   );
	GL_BIND_FUNCTION( glDepthFunc		   );
	GL_BIND_FUNCTION( glDepthMask		   );
	GL_BIND_FUNCTION( glClearDepthf		   );
	GL_BIND_FUNCTION( glCheckFramebufferStatus );
	GL_BIND_FUNCTION( glBindBufferBase	   );

	GL_BIND_FUNCTION( glPolygonMode );
	GL_BIND_FUNCTION( glCullFace );
	GL_BIND_FUNCTION (glFrontFace);

	GL_BIND_FUNCTION( glUniform1fv );
	GL_BIND_FUNCTION( glUniform2fv );
	GL_BIND_FUNCTION( glUniform4fv );


	//vsync
	GL_BIND_FUNCTION( wglSwapIntervalEXT );
}

RECT Renderer::CalculateClientWindow(float clientAspect)
{
	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)( desktopRect.right - desktopRect.left );
	float desktopHeight = (float)( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;
	return clientRect;
}

void Renderer::SetWindowSize(Window * theWindow)
{

	//g_theRenderer->BindShaderProgram("watercolor");
	m_windowSizeTexels  = Vector2((float) theWindow->GetWidth(),  (float) theWindow->GetHeight());
	m_texelSize = Vector2( 1.f / m_windowSizeTexels.x, 1.f / m_windowSizeTexels.y);

	m_sizeData.m_screenSizeX = m_windowSizeTexels.x;
	m_sizeData.m_screenSizeY = m_windowSizeTexels.y;
	m_sizeData.m_texelSizeX = m_texelSize.x;
	m_sizeData.m_texelSizeY = m_texelSize.y;
}

GLenum Renderer::GetGLPrimitiveType(eDrawPrimitiveType primType)
{
	switch (primType){
	case PRIMITIVE_LINES:
		return GL_LINES;
		break;
	case PRIMITIVE_QUADS:
		return GL_QUADS;
		break;
	case PRIMITIVE_TRIANGLES:
		return GL_TRIANGLES;
		break;
	default:
		return 0;
		break;
	}
}

void Renderer::DrawMeshImmediate( Vertex3D_PCU * verts, int numVerts, eDrawPrimitiveType drawPrimitive, const int* indices, int numIndices)
{


	//m_immediateMesh = new Mesh();
	// first, copy the memory to the buffer
	m_immediateMesh->SetLayout(Vertex3D_PCU::LAYOUT);
	m_immediateMesh->SetVertices(sizeof(Vertex3D_PCU) * numVerts, verts);
	m_immediateMesh->SetIndices(numIndices, indices);
	
	//DrawInstruction draw;
	m_immediateMesh->m_instruction.m_primitive = drawPrimitive;
	m_immediateMesh->m_instruction.m_start_index = 0;
	m_immediateMesh->m_instruction.m_useIndices = (numIndices != 0);
	m_immediateMesh->m_instruction.m_elem_count = m_immediateMesh->m_instruction.m_useIndices ? numIndices : numVerts;
	//m_immediateMesh->m_instruction = draw;
	
	//BindModel(Matrix44::IDENTITY);
	BindModel(m_modelMatrix.m_top);
	
	BindStateAndDrawMesh(m_immediateMesh);
	//glDrawArrays( GetGLPrimitiveType(drawPrimitive), 0, numVerts );
	//delete[] verts;
	//delete m_immediateMesh;
}

void Renderer::BindStateAndDrawMesh(SubMesh * mesh)
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	// Describe the buffer - first, figure out where the shader is expecting
	// position to be.
	BindRendererUniforms();
	GL_CHECK_ERROR();
	BindRenderState(m_currentShader->m_state);
	GL_CHECK_ERROR();
	//GL_CHECK_ERROR();

	glBindFramebuffer( GL_FRAMEBUFFER, m_currentCamera->GetFramebufferHandle() );
	GL_CHECK_ERROR();

	//--------------------------------------------------------------------------------------------------------------
	DrawMesh(mesh);

}

void Renderer::DrawMesh(SubMesh * mesh)
{
	BindMeshToProgram(m_currentShader->m_program, mesh);
	GL_CHECK_ERROR();

	GLenum primitive = GetGLPrimitiveType(mesh->GetPrimitiveType());
	GL_CHECK_ERROR();
	if (mesh->UsesIndices()){
		//glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->m_ibo.GetHandle() ); 
		glDrawElements(primitive, mesh->GetElementCount(), GL_UNSIGNED_INT, 0);		//is this the correct call?? @GL_ARRAY_BUFFER
	} else {
		GL_CHECK_ERROR();
		glDrawArrays( primitive, mesh->GetStartIndex(), mesh->GetElementCount());
	}
}

void Renderer::BindUniform(std::string uniformName, const float & value)
{
	GLuint program_handle = m_currentShader->GetProgramHandle(); 

	GLint bind  = glGetUniformLocation(program_handle, uniformName.c_str() );

	if (bind >= 0) {
		glProgramUniform1f(program_handle, bind, (GLfloat) value);
		//glUniform1fv( bind, 1, GL_FALSE, (GLfloat*)&m_currentCamera->m_projMatrix.m_top );
	}
}



void Renderer::BindRendererUniforms()
{
	GLuint program_handle = m_currentShader->GetProgramHandle(); 

	/*GLint model_bind = glGetUniformLocation(program_handle, "MODEL" );*/
	GLint proj_bind  = glGetUniformLocation(program_handle, "PROJECTION" );
	GLint view_bind  = glGetUniformLocation(program_handle, "VIEW" );
	GLint pos_bind	 = glGetUniformLocation(program_handle, "EYE_POSITION");

	// next, bind where position is in our buffer to that location; 

// 	if (model_bind >= 0) {
// 		glUniformMatrix4fv( model_bind, 1, GL_FALSE, (GLfloat*)&m_currentCamera->m_modelMatrix );
// 	}

	if (proj_bind >= 0) {
		glUniformMatrix4fv( proj_bind, 1, GL_FALSE, (GLfloat*)&m_currentCamera->m_projMatrix.m_top );
	}

	if (view_bind >= 0) {
		Matrix44 viewMat = m_currentCamera->GetViewMatrix();
		glUniformMatrix4fv( view_bind, 1, GL_FALSE, (GLfloat*)&viewMat );
	}

	if (pos_bind >= 0) {
		Vector3 pos = m_currentCamera->GetPosition();
		glProgramUniform3fv(program_handle, pos_bind, 1, (GLfloat*)&pos);
	}

	BindAmbientLight();
}

void Renderer::BindRenderState(RenderState state)
{
	SetCullMode(state.m_cullMode);
	SetFillMode(state.m_fillMode);
	SetBlendMode(state.m_blendMode);
	SetWindOrder(state.m_frontFace);
	SetDepth(state.m_depthCompare, state.m_depthWrite);
}

void Renderer::BindMeshToProgram(ShaderProgram * program, SubMesh * mesh)
{
	

	// first, bind the mesh - same as before
	glBindBuffer( GL_ARRAY_BUFFER, mesh->m_vbo.GetHandle() ); 
	if (mesh->m_instruction.m_useIndices){
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->m_ibo.GetHandle() ); 
	}

	// we'll need stride later...
	unsigned int vertex_stride = mesh->GetVertexStride(); 

	GLuint ph = program->GetHandle(); 

	

	unsigned int attribCount = mesh->GetAttributeCount(); 

	for (unsigned int attribIdx = 0; attribIdx < attribCount; ++attribIdx) {
		VertexAttribute const &attribute = mesh->m_layout.GetAttribute(attribIdx); 

		// a program needs a name;
		int bind = glGetAttribLocation( ph, attribute.m_name.c_str() ); 
		GL_CHECK_ERROR();
		// this attribute exists in this shader, cool, bind it
		if (bind >= 0) {
			glEnableVertexAttribArray( bind );

			// be sure mesh and program are bound at this point
			// as this links them together
			glVertexAttribPointer( bind,
				attribute.m_elementCount,      // how many? 
				ToGLType(attribute.m_type),  // what are they 
				attribute.m_normalized,      // are they normalized 
				vertex_stride,          // vertex size?
				(GLvoid*) attribute.m_memberOffset // data offset from start
				);                         // of vertex 
		}
	}
}

void Renderer::BindLightUniforms(Light * light)
{
	m_lightData.m_lights[0] = light->GetBuffer();

	m_lightBuffer.CopyToGPU( sizeof(m_lightData), &m_lightData); 
	glBindBufferBase( GL_UNIFORM_BUFFER, 
		LIGHT_BUFFER_BINDING, 
		m_lightBuffer.GetHandle() ); 
}

void Renderer::BindLightUniforms(std::vector<Light*> lights)
{
	for (int i = 0; i < MAX_LIGHTS; i++){
		if (i < (int) lights.size()){
			m_lightData.m_lights[i] = lights[i]->GetBuffer();
		}
	}

	m_lightBuffer.CopyToGPU( sizeof(m_lightData), &m_lightData); 
	glBindBufferBase( GL_UNIFORM_BUFFER, 
		LIGHT_BUFFER_BINDING, 
		m_lightBuffer.GetHandle() ); 
}

void Renderer::BindLightUniforms(Light * lights[MAX_LIGHTS])
{
	for (int i = 0; i < MAX_LIGHTS; i++){
		if (lights[i] != nullptr){
			m_lightData.m_lights[i] = lights[i]->GetBuffer();
		} else {
			m_lightData.m_lights[i].color = Vector4(0.f,0.f,0.f,0.f);		//disable the light
		}
	}

	m_lightBuffer.CopyToGPU( sizeof(m_lightData), &m_lightData); 
	glBindBufferBase( GL_UNIFORM_BUFFER, 
		LIGHT_BUFFER_BINDING, 
		m_lightBuffer.GetHandle() ); 
}

void Renderer::BindSurfaceUniforms(float specAmount, float specFactor)
{
	GLuint program_handle = m_currentShader->GetProgramHandle(); 

	GLint specAmount_loc = glGetUniformLocation(program_handle, "SPECULAR_AMOUNT" );
	GLint specPower_loc  = glGetUniformLocation(program_handle, "SPECULAR_POWER" );

	if (specAmount_loc >= 0) {
		glProgramUniform1f(program_handle, specAmount_loc, (GLfloat) specAmount);
	}

	if (specPower_loc >= 0) {
		glProgramUniform1f(program_handle, specPower_loc, (GLfloat) specFactor );
	}

}

void Renderer::BindAmbientLight()
{
	Vector4 color = m_ambientLight.GetNormalized();
	GLuint program_handle = m_currentShader->GetProgramHandle(); 

	GLint amb_loc  = glGetUniformLocation(program_handle, "AMBIENT" );

	// next, bind where position is in our buffer to that location; 
	if (amb_loc >= 0) {
		glProgramUniform4fv(program_handle, amb_loc, 1, (GLfloat*)&color);
	}
}

void Renderer::BindFog(FogData_t& fogData)
{
	m_fogBuffer.CopyToGPU( sizeof(fogData), &fogData);
	glBindBufferBase( GL_UNIFORM_BUFFER, 
		FOG_BUFFER_BINDING, 
		m_fogBuffer.GetHandle() ); 
}

void Renderer::BindMaterial(Material * mat)
{
	
	BindShader(mat->GetShader());
	GLuint handle = mat->m_shader->GetProgramHandle();

	for (unsigned int texIdx = 0; texIdx < mat->m_textures.size(); texIdx++){
		if (mat->m_textures[texIdx] != nullptr){
			BindTexture(*mat->m_textures[texIdx], texIdx);
			BindSampler(*mat->m_samplers[texIdx], texIdx);
		}
	}

	//BindSurfaceUniforms(m_specAmount, m_specFactor);

	//surface uniforms are now properties
	for(MaterialProperty* prop : mat->m_properties){
		prop->Bind(handle);
	}
}

void Renderer::DrawSkybox(Skybox * skybox)
{
	BindMaterial(Material::GetMaterial("skybox"));
	BindModel(skybox->m_transform.GetWorldMatrix());
	BindTextureCube(skybox->m_texCube, 8);
	GL_CHECK_ERROR();
	BindStateAndDrawMesh(skybox->m_mesh->m_subMeshes[0]);
}

void Renderer::BindProjection()
{
	GLuint program_handle = m_currentShader->GetProgramHandle(); 
	GLint proj_bind  = glGetUniformLocation(program_handle, "MODEL" );
	if (proj_bind >= 0) {
		glUniformMatrix4fv( proj_bind, 1, GL_FALSE, (GLfloat*)&m_currentCamera->m_projMatrix.m_top );
	}
}

void Renderer::BindCamera(Camera * cam)
{
	IntVector2 dimensions = cam->GetDepthTarget()->GetDimensions();
	glViewport(0, 0, dimensions.x, dimensions.y);
	m_currentCamera = cam;		//???
	GLuint program_handle = m_currentShader->GetProgramHandle(); 
	GLint proj_bind  = glGetUniformLocation(program_handle, "PROJECTION" );
	if (proj_bind >= 0) {
		glUniformMatrix4fv( proj_bind, 1, GL_FALSE, (GLfloat*)&m_currentCamera->m_projMatrix.m_top );
	}
	GL_CHECK_ERROR();
}

void Renderer::BindModel(Matrix44 mat, Shader* shader)
{
	GLuint ph;
	 if (shader == nullptr){
		ph = m_currentShader->GetProgramHandle(); 
	 } else {
		 ph = shader->GetProgramHandle();
	 }
	GLint model_bind = glGetUniformLocation(ph, "MODEL" );
	if (model_bind >= 0) {
		glUniformMatrix4fv( model_bind, 1, GL_FALSE, (GLfloat*)&mat );
	}
}

void Renderer::SetAmbientLight(RGBA color)
{
	m_ambientLight = color;
	BindAmbientLight();
}

void Renderer::DrawLine2D(const Vector2 start, const Vector2 end, const RGBA startColor, const RGBA endColor, float lineThickness)
{
	//m_currentShader->SetCullMode(CULLMODE_NONE);
	glLineWidth( lineThickness );
	//Vertex3D_PCU *verts = (Vertex3D_PCU*) malloc( vsize ); 
	Vertex3D_PCU startVert = Vertex3D_PCU(start, startColor, Vector2(0.f,0.f));
	Vertex3D_PCU endVert = Vertex3D_PCU(end, endColor, Vector2(1.f,1.f));
	Vertex3D_PCU verts[] = {startVert, endVert};
	DrawMeshImmediate(verts, 2, PRIMITIVE_LINES);
	//free(verts);
}

void Renderer::DrawDottedLine2D(const Vector2 start, const Vector2 end, const RGBA color, float dotLength, float lineThickness)
{
	Vector2 direction = end - start;
	float length = (end-start).GetLength();
	int numLines = (int)(length/ dotLength);
	Vector2 step = direction / (float) numLines;
	Vector2 startPoint = start;
	for (int stepNum = 0; stepNum < numLines; stepNum+=2){
		startPoint = start + (step * (float) stepNum);
		DrawLine2D(startPoint, startPoint + step, color, color, lineThickness);
	}
}


void Renderer::DrawShapeFromVertices2(Vector2 vertices[], int numVertices, RGBA color, int numInArray)
{
	
	//vertsVector.clear();
	if (numInArray == -1){
		numInArray = numVertices;
	}
	const int numToDraw = numInArray;
	if ((numVertices >0)){
		std::vector<Vertex3D_PCU> vertsVector = std::vector<Vertex3D_PCU>();
		for (int i = 0; i < numInArray; i++){
			Vertex3D_PCU startPoint = Vertex3D_PCU(vertices[i], color, Vector2(0.f,0.f));
			Vertex3D_PCU endPoint = Vertex3D_PCU(vertices[((i+1)%numVertices)], color, Vector2(0.f,0.f));
			vertsVector.push_back(startPoint);
			vertsVector.push_back(endPoint);
			//DrawLine(vertices[i], vertices[((i+1)%numVertices)], color, color);
		}

		DrawMeshImmediate(vertsVector.data(), (int) vertsVector.size(), PRIMITIVE_LINES);
	}
	
}


void Renderer::BeginFrame(const Vector2 & bottomLeft, const Vector2 & topRight, RGBA color)
{
	UNUSED(bottomLeft);
	UNUSED(topRight);
	ReleaseTexture();
	SetAlphaBlending();
	UseShaderProgram(m_defaultShader->m_program);
	//SetOrtho(bottomLeft, topRight);
	ClearScreen(color);
	
	BindRendererUniforms();
	BindRenderState(m_currentShader->m_state);
	BindFrameUniforms();
}

void Renderer::BeginFrame(const Vector3 & nearBottomLeft, const Vector3 & farTopRight, RGBA color)
{
	UNUSED(farTopRight);
	UNUSED(nearBottomLeft);
	ReleaseTexture();
	SetAlphaBlending();
	UseShaderProgram(m_defaultShader->m_program);
	//SetOrtho(nearBottomLeft, farTopRight);
	ClearScreen(color);

	BindRendererUniforms();
	BindRenderState(m_currentShader->m_state);
	BindFrameUniforms();

	
}

void Renderer::BindFrameUniforms()
{
	m_timeBuffer.CopyToGPU( sizeof(m_timeData), &m_timeData); 
	glBindBufferBase( GL_UNIFORM_BUFFER, 
		TIME_BUFFER_BINDING, 
		m_timeBuffer.GetHandle() ); 

	m_sizeBuffer.CopyToGPU(sizeof(m_sizeData), &m_sizeData);
	glBindBufferBase( GL_UNIFORM_BUFFER, 
		SIZE_BUFFER_BINDING, 
		m_sizeBuffer.GetHandle() ); 
	
}

void Renderer::UpdateClock(float gameDS, float systemDS)
{
	m_timeData.game_delta_time = gameDS;
	m_timeData.game_time += gameDS; 
	m_timeData.system_delta_time = systemDS;
	m_timeData.system_time += systemDS; 
}

void Renderer::ClearScreen(const RGBA & clearColor)
{
	Vector4 normalizedColor = clearColor.GetNormalized();
	glClearColor(normalizedColor.x, normalizedColor.y, normalizedColor.z, normalizedColor.w);
	glClear( GL_COLOR_BUFFER_BIT );
}

void Renderer::SetOrtho(const Vector3 & nearBottomLeft, const Vector3 & farTopRight)
{
	SetProjectionMatrix(Matrix44::MakeOrtho3D(nearBottomLeft, farTopRight));
}

void Renderer::SetOrtho(const Vector2 & bottomLeft, const Vector2 & topRight)
{
	SetProjectionMatrix(Matrix44::MakeOrtho2D(bottomLeft, topRight));
}

void Renderer::SetProjectionMatrix(Matrix44 const & projection)
{
	m_currentCamera->SetProjection(projection);
}

//void Renderer::SetViewMatrix(Matrix44 const & viewMatrix)
//{
//	m_currentCamera->SetViewMatrix(viewMatrix);
//}

void Renderer::UseShader(std::string shaderID)
{
	Shader* newShader = Shader::GetShader(shaderID);
	if (newShader != nullptr){
		m_currentShader = newShader;
		if (m_currentShader->m_program == nullptr){		//program didn't compile, so use invalid shader
			m_currentShader = m_invalidShader;
		}
		GLuint program_handle = m_currentShader->GetProgramHandle();
		// Now that it is described and bound, draw using our program
		glUseProgram( program_handle ); 
	} else {
		m_currentShader = m_invalidShader;
		GLuint program_handle = m_currentShader->GetProgramHandle();
		// Now that it is described and bound, draw using our program
		glUseProgram( program_handle ); 
	}

}

void Renderer::BindShader(Shader * shader)
{
	
	if (shader != nullptr){		//invalid shader
		m_currentShader = shader;
		if (m_currentShader->m_program == nullptr){		//program didn't compile, so use invalid shader
			m_currentShader = m_invalidShader;
		}
		GLuint program_handle = m_currentShader->GetProgramHandle();
		// Now that it is described and bound, draw using our program
		glUseProgram( program_handle ); 
	} else {
		m_currentShader = m_invalidShader;
		GLuint program_handle = m_currentShader->GetProgramHandle();
		// Now that it is described and bound, draw using our program
		glUseProgram( program_handle ); 
	}
	BindRenderState(m_currentShader->m_state);
}

void Renderer::ReleaseShader()
{
	m_currentShader = m_defaultShader;
	GLuint program_handle = m_currentShader->GetProgramHandle();
	// Now that it is described and bound, draw using our program
	glUseProgram( program_handle ); 
}

void Renderer::SetAdditiveBlending()
{
	//UNIMPLEMENTED();
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
}

void Renderer::SetAlphaBlending()
{
	//UNIMPLEMENTED();
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void Renderer::SetMultiplyBlending()
{
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	//glBlendFunc(GL_ZERO, GL_SRC_COLOR);
}

void Renderer::DisableBlending()
{
	glBlendFunc(GL_ZERO, GL_ONE);
}

void Renderer::BindShaderProgram(std::string shaderRoot)
{
	ShaderProgram* program = CreateOrGetShaderProgram(shaderRoot);
	if (program == nullptr){
		m_currentShader = m_invalidShader;
		GLuint program_handle = m_currentShader->GetProgramHandle();
		// Now that it is described and bound, draw using our program
		glUseProgram( program_handle ); 
	} else {
		UseShaderProgram(program);
	}
}

void Renderer::UseShaderProgram(ShaderProgram * shader)
{
	if (shader == nullptr){
		m_defaultShader->SetProgram(CreateOrGetShaderProgram("default"));
		
	} else {
		m_defaultShader->SetProgram(shader);
	}

	ASSERT_OR_DIE(m_defaultShader->m_program != nullptr, "No default shader available");
	GLuint program_handle = m_defaultShader->GetProgramHandle();
	// Now that it is described and bound, draw using our program
	glUseProgram( program_handle ); 
	//glLinkProgram(m_currentShaderProgram->GetHandle());
}

void Renderer::ReleaseShaderProgram()
{
	BindShaderProgram("default");
}

void Renderer::ApplyEffect(std::string shaderRoot, const AABB2& uvs)
{
	UNUSED(uvs);
	Material* mat = Material::GetMaterial(shaderRoot);
	if (mat != nullptr)
	{
		ApplyEffect(mat);
	} else {
		ShaderProgram* prog = CreateOrGetShaderProgram(shaderRoot);
		ApplyEffect(prog);
	}
}

void Renderer::ApplyEffect(ShaderProgram * program, const AABB2& uvs)
{
	UNUSED(uvs);
	//make sure you've got both targets
	if (m_effectTarget == nullptr){
		m_effectTarget = m_defaultColorTarget;
		//m_effectTarget = m_currentCamera->m_output.m_colorTarget;		//lazy instantiation of effect target(s)
		if (m_effectScratch == nullptr){
			m_effectScratch = Texture::CreateCompatible(m_effectTarget);
		}
	}

	//Draw full screen quad using the program
	m_effectCamera->SetColorTarget(m_effectScratch);
	SetCamera(m_effectCamera);
	UseShaderProgram(program);

	BindTexture(*m_effectTarget, 0);
	//BindTexture(*m_defaultDepthTarget, 1);
	DrawAABB2(AABB2(-1.f * Vector2::ONE, Vector2::ONE), RGBA::WHITE);


	//the current output should be the input for this step
	//scratch will BECOME the new output - flipping output and scratch
	//BLIT memcopy's the image, pretty much

	std::swap(m_effectTarget, m_effectScratch);
	//(m_effectTarget = m_effectScratch, and m_effectScratch = m_effectTarget)
	GL_CHECK_ERROR();

}

void Renderer::ApplyEffect(Material * material, const AABB2& uvs)
{
	//make sure you've got both targets
	if (m_effectTarget == nullptr){
		m_effectTarget = m_defaultColorTarget;
		//m_effectTarget = m_currentCamera->m_output.m_colorTarget;		//lazy instantiation of effect target(s)
		if (m_effectScratch == nullptr){
			m_effectScratch = Texture::CreateCompatible(m_effectTarget);
		}
	}

	material->SetProperty("WORLD_UV_MINS", uvs.mins);
	material->SetProperty("WORLD_UV_MAXS", uvs.maxs);

	//Draw full screen quad using the program
	m_effectCamera->SetColorTarget(m_effectScratch);
	SetCamera(m_effectCamera);
	BindMaterial(material);

	BindTexture(*m_effectTarget, 0);
	//BindTexture(*m_defaultDepthTarget, 1);
	DrawAABB2(AABB2(-1.f * Vector2::ONE, Vector2::ONE), RGBA::WHITE);


	//the current output should be the input for this step
	//scratch will BECOME the new output - flipping output and scratch
	//BLIT memcopy's the image, pretty much

	std::swap(m_effectTarget, m_effectScratch);
	//(m_effectTarget = m_effectScratch, and m_effectScratch = m_effectTarget)
	GL_CHECK_ERROR();
}

void Renderer::FinishEffects()
{
	//check if you've applied any effects
	if (m_effectTarget == nullptr){ 
		return; 
	}

	if (m_effectTarget != m_defaultColorTarget) {
		//I ended on an odd apply effect, copy effect target to default color target
		CopyTexture( m_defaultColorTarget, m_effectTarget);		//just don't copy to CPU then copy to GPU again. Might be a GL thing for this.
																//DON't Do read pixel to buffer and set pixels from buffer
		m_effectScratch = m_effectTarget;		//put scratch back where it belongs
	}
	m_effectTarget = nullptr;
	ReleaseShader();
	//ReleaseShaderProgram();
	ReleaseTexture();
}

void Renderer::EnableDepth(eCompare compare, bool should_write)
{
	m_defaultShader->m_state.m_depthCompare = compare;
	m_defaultShader->m_state.m_depthWrite = should_write;
	
}

void Renderer::DisableDepth()
{
	// You can glDisable( GL_DEPTH_TEST ) as well, 
	// but that prevents depth clearing from working, 
	// so I prefer to just use these settings which is 
	// effectively not using the depth buffer.
	//EnableDepth( COMPARE_ALWAYS, false ); 
	m_defaultShader->m_state.m_depthCompare = COMPARE_ALWAYS;
	m_defaultShader->m_state.m_depthWrite = true;
}

void Renderer::ClearDepth(float depth)
{
	glClearDepthf( depth );
	glClear( GL_DEPTH_BUFFER_BIT );
}

void Renderer::EnableFillMode(eFillMode newMode)
{
	m_defaultShader->m_state.m_fillMode = newMode;
}

void Renderer::ResetFillMode()
{
	m_defaultShader->m_state.m_fillMode = FILL_MODE_FILL;
}

void Renderer::EndFrame(HDC displayContext)
{
	// copies the default camera's framebuffer to the "null" framebuffer, 
	// also known as the back buffer.
	PROFILE_PUSH("EndFrame::CopyFrameBuffer");
	CopyFrameBuffer( nullptr, &m_defaultCamera->m_output );			//m_current camera?
	PROFILE_POP();
	PROFILE_PUSH("EndFrame::SwapBuffers");
	SwapBuffers( displayContext ); // Note: call this once at the end of each frame
	PROFILE_POP();
}

bool Renderer::CopyFrameBuffer(FrameBuffer * dst, FrameBuffer * src)
{
	// we need at least the src.
	if (src == nullptr) {
		return false; 
	}

	// Get the handles - NULL refers to the "default" or back buffer FBO
	GLuint src_fbo = src->GetHandle();
	GLuint dst_fbo = NULL; 
	if (dst != nullptr) {
		dst_fbo = dst->GetHandle(); 
	}

	// can't copy onto ourselves
	if (dst_fbo == src_fbo) {
		return false; 
	}

	// the GL_READ_FRAMEBUFFER is where we copy from
	glBindFramebuffer( GL_READ_FRAMEBUFFER, src_fbo ); 

	// what are we copying to?
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dst_fbo ); 

	// blit it over - get the size
	// (we'll assume dst matches for now - but to be safe,
	// you should get dst_width and dst_height using either
	// dst or the window depending if dst was nullptr or not
	unsigned int  width = src->GetWidth();     
	unsigned int height = src->GetHeight(); 

	// Copy it over
	glBlitFramebuffer( 0, 0, // src start pixel
		width, height,        // src size
		0, 0,                 // dst start pixel
		width, height,        // dst size
		GL_COLOR_BUFFER_BIT,  // what are we copying (just colour)
		GL_NEAREST );         // resize filtering rule (in case src/dst don't match)

							  // Make sure it succeeded
	GL_CHECK_ERROR(); 

	// cleanup after ourselves
	glBindFramebuffer( GL_READ_FRAMEBUFFER, NULL ); 
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL ); 

	return GLSucceeded();
}

void Renderer::Pop()
{
	m_modelMatrix.Pop();
	//BindProjection();
	BindModel(m_modelMatrix.m_top);
	/*glPopMatrix();*/
}

void Renderer::Push()
{
	m_modelMatrix.Push();
	BindModel(m_modelMatrix.m_top);
	/*glPushMatrix();*/
}


//backwards compatibility for now (go through old projects and updated Push2 and Pop2
void Renderer::Pop2()
{
	Pop();
}

void Renderer::Push2()
{
	Push();
}

void Renderer::Transform(Vector3 position, Vector3 rotation, float scale)
{
	Translate(position);
	Rotate(rotation);
	Scale(scale);

}

void Renderer::Transform2(Vector2 position, float rotation, float scale)
{
	Translate2(position);
	Rotate2(rotation);
	Scale2(scale);
}

void Renderer::Transform(Matrix44 mat)
{
	//m_currentCamera->m_projMatrix.Transform(mat);
	m_modelMatrix.Transform(mat);
}


void Renderer::Translate2(Vector2 position)
{
	//m_currentCamera->m_projMatrix.m_top.Translate2D(position);
	m_modelMatrix.m_top.Translate2D(position);
}

//void Renderer::Translate(Vector3 position)
//{
//	UNIMPLEMENTED();
//	/*glTranslatef(position.x, position.y, 0.f);*/
//}

void Renderer::Translate(Vector3 position)
{
	//m_currentCamera->m_projMatrix.m_top.Translate3D(position);
	m_modelMatrix.m_top.Translate3D(position);
}

void Renderer::Rotate2(float rotation)
{
	m_modelMatrix.m_top.RotateDegrees2D(rotation);
	//m_currentCamera->m_projMatrix.m_top.RotateDegrees2D(rotation);
	/*glRotatef(rotation, 0.f, 0.f, 1.f);*/
}

void Renderer::Rotate(Vector3 rotation)
{
	m_modelMatrix.m_top.RotateDegrees3D(rotation);
}

void Renderer::Scale(float scale)
{
	UNUSED(scale);
	UNIMPLEMENTED();
	//m_currentCamera->m_projMatrix.m_top.ScaleUniform3D(scale);
	/*glScalef(scale, scale, 1.f);*/
}

void Renderer::Scale2(float scale)
{
	//m_currentCamera->m_projMatrix.m_top.MakeScaleUniform2D(scale);
	m_modelMatrix.m_top.Scale2D(scale,scale);
}

void Renderer::DrawRegularPolygon2(int numSides, Vector2 pos, float radius, float rotation, RGBA color, bool dottedLine)
{
	float theta = (360.f /  numSides) + rotation;		//internal angle of a "circle" polygon
	Vector2 start;
	Vector2 end;
	int increase = 1;
	if (dottedLine){
		increase = 2;
	}
	
	std::vector<Vertex3D_PCU> vertsVector = std::vector<Vertex3D_PCU>();
	
		
	for (int i = 0; i < numSides; i+=increase){
		float degreesStart = (theta * i) + rotation;
		float degreesEnd = (theta * (i+1)) + rotation;
		start = (Vector2(CosDegreesf(degreesStart) * radius, SinDegreesf(degreesStart)*radius)) + pos;
		end = (Vector2(CosDegreesf(degreesEnd) * radius, SinDegreesf(degreesEnd)*radius)) + pos;

		Vertex3D_PCU startPoint = Vertex3D_PCU(start, color, Vector2(0.f,0.f));
		Vertex3D_PCU endPoint = Vertex3D_PCU(end, color, Vector2(0.f,0.f));
		vertsVector.push_back(startPoint);
		vertsVector.push_back(endPoint);
		//DrawLine(start,end, color, color);
	}
	 Vertex3D_PCU* vertArray = &vertsVector[0];
	DrawMeshImmediate(vertArray, (int) vertsVector.size(), PRIMITIVE_LINES);
}

void Renderer::DrawCircleArc(float percentage, int numSides, Vector2 pos, float radius, float rotation, RGBA color)
{
	int numSidesToDraw = static_cast<int> (percentage * numSides);
	float theta = (360.f /  numSides) + rotation;		//internal angle of a "circle" polygon
	Vector2 start;
	Vector2 end;
	for (int i = 0; i < numSidesToDraw; i++){
		float degreesStart = (theta * i) + rotation;
		float degreesEnd = (theta * (i+1)) + rotation;
		start = (Vector2(CosDegreesf(degreesStart) * radius, SinDegreesf(degreesStart)*radius));
		end = (Vector2(CosDegreesf(degreesEnd) * radius, SinDegreesf(degreesEnd)*radius));
		DrawLine2D(start,end, color, color);
	}
}

void Renderer::DrawSquigglyLine(const Vector2 start, const Vector2 end, const float xVal, RGBA color)
{
	float xChange = 1.f/100.f;
	Vector2 vertices[100];
	for (int i =0;i < 100;i++){
		float toSin = RangeMapFloat(xChange*i,0.f,.4f,0.f,10.f);
		float sinVal = sinf(toSin+(xVal))*.05f;
		vertices[i] = Vector2(xChange*i, sinVal);

	}

	Vector2 direction = end - start;

	PushAndTransform2(start, direction.GetOrientationDegrees(), direction.GetLength());
	DrawShapeFromVertices2(vertices,100,color,99);
	Pop();
}

void Renderer::DrawTriangle(const Vector2 vertices[], const RGBA & color)
{
	size_t vsize = Vertex3D_PCU::LAYOUT.m_stride * 3; 
	Vertex3D_PCU *verts = (Vertex3D_PCU*) malloc( vsize ); 
	verts[0] = Vertex3D_PCU(Vector2(vertices[0].x, vertices[0].y), color, Vector2(0.f,1.f));
	verts[1] = Vertex3D_PCU(Vector2(vertices[1].x, vertices[1].y), color, Vector2(1.f,1.f));
	verts[2] = Vertex3D_PCU(Vector2(vertices[2].x, vertices[2].y), color, Vector2(1.f,0.f));
	DrawMeshImmediate(verts, 3, PRIMITIVE_TRIANGLES);
	free(verts);
}

void Renderer::DrawAABB2(const AABB2 & bounds, const RGBA & color, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs)
{
	Vector2 topLeft = Vector2(bounds.mins.x, bounds.maxs.y);		//texCoords 0,1
	Vector2 topRight = Vector2(bounds.maxs.x, bounds.maxs.y);		//texCoords 1,1
	Vector2 bottomLeft = Vector2(bounds.mins.x, bounds.mins.y);		//texCoords 0,0
	Vector2 bottomRight = Vector2(bounds.maxs.x, bounds.mins.y);	//texCoords 1,0

	size_t vsize = Vertex3D_PCU::LAYOUT.m_stride * 6; 
	Vertex3D_PCU *verts = (Vertex3D_PCU*) malloc( vsize ); 
	verts[0] = Vertex3D_PCU(bottomLeft,  color, Vector2(texCoordsAtMins.x,texCoordsAtMins.y));
	verts[1] = Vertex3D_PCU(bottomRight, color, Vector2(texCoordsAtMaxs.x,texCoordsAtMins.y));
	verts[2] = Vertex3D_PCU(topRight,	 color, Vector2(texCoordsAtMaxs.x,texCoordsAtMaxs.y));

	verts[3] = Vertex3D_PCU(bottomLeft,  color, Vector2(texCoordsAtMins.x,texCoordsAtMins.y));
	verts[4] = Vertex3D_PCU(topRight,	 color, Vector2(texCoordsAtMaxs.x,texCoordsAtMaxs.y));
	verts[5] = Vertex3D_PCU(topLeft,	 color, Vector2(texCoordsAtMins.x,texCoordsAtMaxs.y));
	DrawMeshImmediate(verts, 6, PRIMITIVE_TRIANGLES);
	free(verts);
}

void Renderer::DrawOBB2(const OBB2 & bounds, const RGBA & color, const Vector2 & uvMins, const Vector2 & uvMaxs)
{
	Vector2 topLeft = bounds.GetWorldTopLeft();			//texCoords 0,1
	Vector2 topRight = bounds.GetWorldMaxs();			//texCoords 1,1
	Vector2 bottomLeft = bounds.GetWorldMins();			//texCoords 0,0
	Vector2 bottomRight = bounds.GetWorldBottomRight();	//texCoords 1,0

	size_t vsize = Vertex3D_PCU::LAYOUT.m_stride * 6; 
	Vertex3D_PCU *verts = (Vertex3D_PCU*) malloc( vsize ); 
	verts[0] = Vertex3D_PCU(bottomLeft,  color, Vector2(uvMins.x, uvMins.y));
	verts[1] = Vertex3D_PCU(bottomRight, color, Vector2(uvMaxs.x, uvMins.y));
	verts[2] = Vertex3D_PCU(topRight,	 color, Vector2(uvMaxs.x, uvMaxs.y));

	verts[3] = Vertex3D_PCU(bottomLeft,  color, Vector2(uvMins.x, uvMins.y));
	verts[4] = Vertex3D_PCU(topRight,	 color, Vector2(uvMaxs.x, uvMaxs.y));
	verts[5] = Vertex3D_PCU(topLeft,	 color, Vector2(uvMins.x, uvMaxs.y));
	DrawMeshImmediate(verts, 6, PRIMITIVE_TRIANGLES);
	free(verts);
}

void Renderer::DrawDisc2(const Vector2 & center, const float & radius, const RGBA & color, int segments)
{
	if (segments < 3){
		ConsolePrintf("Can't draw a circle with < 3 segments :(");
		return;
	}
	float currentAngle = 0.f;
	float theta = 360.f / (float) segments;
	std::vector<Vertex3D_PCU> verts = std::vector<Vertex3D_PCU>();

	Vector2 currentPos;
	for (int i = 0; i < segments + 1; i++){
		currentPos = PolarToCartesian(radius, currentAngle) + center;
		Vector2 nextPos = PolarToCartesian(radius, currentAngle + theta) + center;
		//add a triangle :)
		verts.push_back(Vertex3D_PCU(center, color, Vector2::ZERO));				//center
		verts.push_back(Vertex3D_PCU(currentPos, color, Vector2(0.f, 1.f)));		//first radius point
		verts.push_back(Vertex3D_PCU(nextPos, color, Vector2(1.f, 0.f)));			//second radius point

		currentAngle+=theta;
	}
	DrawMeshImmediate(verts.data(), (int) verts.size(), PRIMITIVE_TRIANGLES);
	
}

void Renderer::DrawBlendedAABB2(const AABB2 & bounds, const RGBA & topLeftColor, const RGBA & bottomLeftColor, const RGBA & topRightColor, const RGBA & bottomRightColor)
{
	Vector2 topLeft = Vector2(bounds.mins.x, bounds.maxs.y);		//texCoords 0,1
	Vector2 topRight = Vector2(bounds.maxs.x, bounds.maxs.y);		//texCoords 1,1
	Vector2 bottomLeft = Vector2(bounds.mins.x, bounds.mins.y);		//texCoords 0,0
	Vector2 bottomRight = Vector2(bounds.maxs.x, bounds.mins.y);	//texCoords 1,0

	size_t vsize = Vertex3D_PCU::LAYOUT.m_stride * 6; 
	Vertex3D_PCU *verts = (Vertex3D_PCU*) malloc( vsize ); 
	verts[0] = Vertex3D_PCU(bottomLeft,  bottomLeftColor, Vector2(0.f,0.f));
	verts[1] = Vertex3D_PCU(bottomRight, bottomRightColor, Vector2(1.f,0.f));
	verts[2] = Vertex3D_PCU(topRight,	 topRightColor, Vector2(1.f,1.f));

	verts[3] = Vertex3D_PCU(bottomLeft,  bottomLeftColor, Vector2(0.f,0.f));
	verts[4] = Vertex3D_PCU(topRight,	 topRightColor, Vector2(1.f,1.f));
	verts[5] = Vertex3D_PCU(topLeft,	 topLeftColor, Vector2(0.f,1.f));


// 	verts[0] = Vertex3D_PCU(topLeft,	 topLeftColor,		Vector2(0.f,1.f));
// 	verts[1] = Vertex3D_PCU(topRight,	 topRightColor,		Vector2(1.f,1.f));
// 	verts[2] = Vertex3D_PCU(bottomLeft,  bottomLeftColor,	Vector2(0.f,0.f));
// 	verts[3] = Vertex3D_PCU(bottomLeft,  bottomLeftColor,	Vector2(0.f,0.f));
// 	verts[4] = Vertex3D_PCU(topRight,	 topRightColor,		Vector2(1.f,1.f));
// 	verts[5] = Vertex3D_PCU(bottomRight, bottomRightColor,	Vector2(1.f,0.f));
	DrawMeshImmediate(verts, 6, PRIMITIVE_TRIANGLES);
	free(verts);
}

void Renderer::DrawAABB2Outline(const AABB2 & bounds, const RGBA & color)
{
	Vector2 topLeft = Vector2(bounds.mins.x, bounds.maxs.y);
	Vector2 bottomRight = Vector2(bounds.maxs.x, bounds.mins.y);

	DrawLine2D(topLeft, bounds.mins, color, color);
	DrawLine2D(bounds.mins, bottomRight, color, color);
	DrawLine2D(bottomRight, bounds.maxs, color, color);
	DrawLine2D(bounds.maxs, topLeft, color, color);

}

void Renderer::DrawTexturedAABB2(const AABB2 & bounds, const Texture & texture, const Vector2 & texCoordsAtMins, const Vector2 & texCoordsAtMaxs, const RGBA & tint)
{

	BindTexture(texture);
	//m_currentShader->SetCullMode(CULLMODE_NONE);


	Vector2 topLeft = Vector2(bounds.mins.x, bounds.maxs.y);		//texCoords 0,1
	Vector2 topRight = Vector2(bounds.maxs.x, bounds.maxs.y);		//texCoords 1,1
	Vector2 bottomLeft = Vector2(bounds.mins.x, bounds.mins.y);		//texCoords 0,0
	Vector2 bottomRight = Vector2(bounds.maxs.x, bounds.mins.y);	//texCoords 1,0

	size_t vsize = Vertex3D_PCU::LAYOUT.m_stride * 6; 
	Vertex3D_PCU *verts = (Vertex3D_PCU*) malloc( vsize ); 

	verts[0] = Vertex3D_PCU(bottomLeft,  tint, Vector2(texCoordsAtMins.x,texCoordsAtMins.y));
	verts[1] = Vertex3D_PCU(bottomRight, tint, Vector2(texCoordsAtMaxs.x,texCoordsAtMins.y));
	verts[2] = Vertex3D_PCU(topRight,	 tint, Vector2(texCoordsAtMaxs.x,texCoordsAtMaxs.y));
	
	verts[3] = Vertex3D_PCU(bottomLeft,  tint, Vector2(texCoordsAtMins.x,texCoordsAtMins.y));
	verts[4] = Vertex3D_PCU(topRight,	 tint, Vector2(texCoordsAtMaxs.x,texCoordsAtMaxs.y));
	verts[5] = Vertex3D_PCU(topLeft,	 tint, Vector2(texCoordsAtMins.x,texCoordsAtMaxs.y));
	
	DrawMeshImmediate(verts, 6, PRIMITIVE_TRIANGLES);

	ReleaseTexture();
	free( verts ); 

	
}


void Renderer::DrawCube(const Vector3 & center, const Vector3 & dimensions, const RGBA & color, const Vector3& right, const Vector3& up, const Vector3& forward, AABB2 uvs)
{
	DrawCube(center, dimensions, color, right, up, forward, uvs, uvs, uvs);
}

void Renderer::DrawCube(const Vector3 & center, const Vector3 & dimensions, const RGBA & color, const Vector3& right, const Vector3& up, const Vector3& forward, AABB2 UV_TOP, AABB2 UV_SIDE, AABB2 UV_BOTTOM)
{
	BindModel(Matrix44::IDENTITY);
	SubMesh* cubeMesh = CreateCube(center, dimensions, color, right, up, forward, UV_TOP, UV_SIDE, UV_BOTTOM);
	BindStateAndDrawMesh(cubeMesh);
	delete(cubeMesh);
}

void Renderer::DrawSphere(const Vector3 & center, const float & radius, const int & wedges, const int & slices, const RGBA & color, const AABB2 & uvs)
{
	BindModel(Matrix44::IDENTITY);
	SubMesh* sphereMesh = CreateSphere(center, radius, wedges, slices, color, uvs);
	BindStateAndDrawMesh(sphereMesh);
	delete(sphereMesh);
}

void Renderer::DrawSphere(const Sphere & sphere, const RGBA & color, const AABB2 & uvs)
{
	DrawSphere(sphere.m_center, sphere.m_radius, sphere.m_wedges, sphere.m_slices, color, uvs);
}

void Renderer::DrawPlane(const Vector3& nearBottomLeft, const Vector3& nearBottomRight, const Vector3& farTopRight, const Vector3& farTopLeft, const RGBA & color, Vector2 uvMins, Vector2 uvMaxs)
{

	size_t vsize = Vertex3D_PCU::LAYOUT.m_stride * 6; 
	Vertex3D_PCU *verts = (Vertex3D_PCU*) malloc( vsize ); 
	//old wind order
	//verts[0] = Vertex3D_PCU(farTopLeft,		 color,	Vector2(uvMins.x,uvMaxs.y));
	//verts[1] = Vertex3D_PCU(farTopRight,	 color, Vector2(uvMaxs.x,uvMaxs.y));
	//verts[2] = Vertex3D_PCU(nearBottomLeft,  color, Vector2(uvMins.x,uvMins.y));
	//verts[3] = Vertex3D_PCU(nearBottomLeft,  color, Vector2(uvMins.x,uvMins.y));
	//verts[4] = Vertex3D_PCU(farTopRight,	 color, Vector2(uvMaxs.x,uvMaxs.y));
	//verts[5] = Vertex3D_PCU(nearBottomRight, color, Vector2(uvMaxs.x,uvMins.y));
	
	verts[0] = Vertex3D_PCU(nearBottomLeft,  color, Vector2(uvMins.x,uvMins.y));
	verts[1] = Vertex3D_PCU(nearBottomRight, color, Vector2(uvMaxs.x,uvMins.y));
	verts[2] = Vertex3D_PCU(farTopRight,	 color, Vector2(uvMaxs.x,uvMaxs.y));
	
	verts[3] = Vertex3D_PCU(nearBottomLeft,  color, Vector2(uvMins.x,uvMins.y));
	verts[4] = Vertex3D_PCU(farTopRight,	 color, Vector2(uvMaxs.x,uvMaxs.y));
	verts[5] = Vertex3D_PCU(farTopLeft,		 color,Vector2(uvMins.x,uvMaxs.y));
	DrawMeshImmediate(verts, 6, PRIMITIVE_TRIANGLES);
	free(verts);
}

void Renderer::DrawPlane(const Plane & plane, const RGBA & color, const Vector2 & uvMins, const Vector2 & uvMaxs)
{
	SubMesh* planeMesh = CreatePlane(plane.m_center, plane.m_up, plane.m_right, plane.m_size, color, uvMins, uvMaxs);
	BindModel(m_modelMatrix.m_top);
	BindStateAndDrawMesh(planeMesh);
	delete(planeMesh);
}

void Renderer::DrawLine3D(const Vector3 & start, const Vector3 & end, const RGBA & startColor, const RGBA & endColor, float width)
{
	UNUSED(width);
	Vertex3D_PCU startVert = Vertex3D_PCU(start, startColor, Vector2(0.f,0.f));
	Vertex3D_PCU endVert = Vertex3D_PCU(end, endColor, Vector2(1.f,1.f));
	Vertex3D_PCU verts[2] {startVert, endVert};
	DrawMeshImmediate(verts, 2, PRIMITIVE_LINES);

}

void Renderer::DrawPoint(const Vector3 & pos, float size, const Vector3 & right, const Vector3 & up, const Vector3 & forward, RGBA rightColor, RGBA upColor, RGBA forwardColor)
{
	ReleaseTexture();

	Vector3 halfRight = right * .5f * size;
	Vector3 halfUp = up * .5f * size;
	Vector3 halfForward = forward * .5f * size;
	//make a cube and draw from corners
	Vector3 nearbotleft = pos - halfRight - halfUp - halfForward;
	Vector3 fartopright = pos + halfRight + halfUp + halfForward;

	Vector3 nearbotright = pos + halfRight - halfUp - halfForward;
	Vector3 fartopleft = pos - halfRight +halfUp + halfForward;

	Vector3 neartopleft = pos - halfRight + halfUp - halfForward;
	Vector3 farbotright = pos + halfRight - halfUp + halfForward;

	Vector3 neartopright = pos + halfRight + halfUp - halfForward;
	Vector3 farbotleft = pos - halfRight - halfUp + halfForward;
	
	DrawLine3D(fartopright, nearbotleft, rightColor, rightColor);
	DrawLine3D(nearbotright, fartopleft, rightColor, rightColor);
	DrawLine3D(neartopleft, farbotright, rightColor, rightColor);
	DrawLine3D(neartopright, farbotleft, rightColor, rightColor);
/*
	Vector3 point_left = pos - (right * size);
	Vector3 point_right = pos + (right * size);
	Vector3 point_down = pos - (up * size);
	Vector3 point_up = pos + (up * size);
	Vector3 point_near = pos - (forward * size);
	Vector3 point_far = pos + (forward * size);
	DrawLine3D(point_left,point_right, rightColor, rightColor);
	DrawLine3D(point_down,point_up, upColor, upColor);
	DrawLine3D(point_near,point_far, forwardColor, forwardColor);*/
}


void Renderer::DrawPoint(const Vector3 & pos, float size, const Vector3 & right, const Vector3& up, const Vector3& forward, const RGBA & color)
{
	DrawPoint(pos, size, right, up, forward, color, color, color);
}

void Renderer::DrawTextAsSprite(const std::string& asciiText,  const Vector3 & pos, const Vector2& pivot, float cellHeight, const Vector3 & right, const Vector3 & up, RGBA tint, const BitmapFont* font, float aspect)
{
	//break text into separate lines
	std::vector<std::string> lines;
	Split(asciiText, '\n', lines);

	//determine max height and starting y pos
	int maxLines = (int) lines.size();
	float maxHeight = maxLines * cellHeight;
	float yStart = -1.f * maxHeight * pivot.y;

	//Draw each line
	for(int lineNum = 0; lineNum < maxLines; lineNum++){
		std::string line = lines[lineNum];
		float lineWidth = line.size() * (cellHeight * aspect);
		float lineOffsetY = cellHeight * (maxLines - lineNum);
		//determine draw pos based on this lines width and which line currently being drawn
		float offsetX = -1.f * lineWidth * pivot.x;
		float offsetY = yStart + lineOffsetY;
		Vector3 posToDraw = pos + (offsetX * right) + (offsetY * up);
		posToDraw.y = posToDraw.y - (cellHeight);
		DrawText3D(line, posToDraw, cellHeight, up, right, tint, font, aspect);
	}


}

AABB2 Renderer::DrawSpriteOnAABB2(const AABB2 & box, Sprite * sprite, RGBA tint)
{
	const Texture* tex = sprite->m_tex;
	BindTexture(*tex);
	AABB2 scaledBox = box;
	scaledBox.StretchToIncludePoint(box.GetCenter() + (sprite->m_dimensions * .5f));
	scaledBox.StretchToIncludePoint(box.GetCenter() - (sprite->m_dimensions * .5f));
	float scale = min((box.GetWidth() / scaledBox.GetWidth()), (box.GetHeight() / scaledBox.GetHeight()));
	scaledBox.UniformScaleFromCenter(scale);
	DrawAABB2(scaledBox, tint, sprite->m_uvs.mins, sprite->m_uvs.maxs);
	ReleaseTexture();
	return scaledBox;
}

void Renderer::DrawSprite(const Vector3 & pos, Sprite * sprite, const Vector3 & right, const Vector3 & up, RGBA tint)
{
	const Texture* tex = sprite->m_tex;
	BindTexture(*tex);
	/*float nx = -1.f * (sprite->m_pivot.x * sprite->m_dimensions.x);
	float fx = nx + sprite->m_dimensions.x;
	float ny = -1.f * (sprite->m_pivot.y * sprite->m_dimensions.y);
	float fy = ny + sprite->m_dimensions.y;

	Vector3 botLeft = pos + (nx * right) + (ny * up);
	Vector3 botRight = pos + (fx * right) + (ny * up);
	Vector3 topLeft = pos + (nx * right) + (fy * up);
	Vector3 topRight = pos + (fx * right) + (fy * up);*/

	Plane* plane = sprite->GetBounds(pos, right, up);

	DrawPlane(*plane, tint, sprite->m_uvs.mins, sprite->m_uvs.maxs);
	ReleaseTexture();
	delete plane;
}

void Renderer::BindTextureCube(const TextureCube & cube, unsigned int textureIndex)
{
	glActiveTexture( GL_TEXTURE0 + textureIndex ); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube.GetHandle());
}

void Renderer::BindTexture(const std::string & fileName, unsigned int textureIndex, const std::string & directory)
{
	Texture* textureToBind = CreateOrGetTexture(fileName, directory);

	if (textureIndex >= m_samplers.size() ){
		m_samplers.resize(textureIndex + 1);
		m_samplers[textureIndex] = new Sampler();
	}

	// Bind the sampler;
	glBindSampler( textureIndex, m_samplers[textureIndex]->GetHandle() ); 

	// Bind the texture
	glActiveTexture( GL_TEXTURE0 + textureIndex ); 
	glBindTexture( GL_TEXTURE_2D, textureToBind->m_textureID );
}

void Renderer::BindTexture(const Texture & texture, unsigned int textureIndex)
{
	//if (textureIndex >= m_samplers.size() ){
	//	m_samplers.resize(textureIndex);
	//	m_samplers[textureIndex] = new Sampler();
	//}

	

	// Bind the texture
	glActiveTexture( GL_TEXTURE0 + textureIndex ); 
	glBindTexture( GL_TEXTURE_2D, texture.m_textureID );
	
	/*
	glBindTexture( GL_TEXTURE_2D, texture.m_textureID );*/
}

void Renderer::BindSampler(const Sampler & sampler, unsigned int samplerINdex)
{
	// Bind the sampler;
	glBindSampler( samplerINdex, sampler.GetHandle()); 
}

void Renderer::BindDefaultTexture(unsigned int textureIndex)
{

	//glEnable(GL_TEXTURE_2D);
									// Bind the sampler;
	if (m_samplers.size() > textureIndex){
		glBindSampler( textureIndex, m_samplers[textureIndex]->GetHandle() ); 

		// Bind the texture
		glActiveTexture( GL_TEXTURE0 + textureIndex ); 
		glBindTexture( GL_TEXTURE_2D, m_defaultTexture->m_textureID );
	}
}

void Renderer::ReleaseTexture(unsigned int textureIndex)
{
	BindDefaultTexture(textureIndex);
	//glDisable(GL_TEXTURE_2D);
}

void Renderer::PushAndTransform2(Vector2 position, float rotation, float scale)
{
	Push();
	Transform2(position, rotation, scale);
	BindModel(m_modelMatrix.m_top);
}

void Renderer::PushAndTransform(Vector3 position, Vector3 rotation, float scale)
{
	Push();
	Transform(position, rotation, scale);
	BindModel(m_modelMatrix.m_top);

}

void Renderer::PushAndTransform(Matrix44 mat)
{
	Push();
	Transform(mat);
	//BindProjection();
}

void Renderer::LoadSpritesFromSpriteAtlas(SpriteSheet * spriteSheet, int PPU, Vector2 pivot)
{
	IntVector2 texDimensions = spriteSheet->GetTexture()->GetDimensions();
	Vector2 spriteDimensions = texDimensions.GetVector2() / (float)PPU;
	int numSprites = spriteSheet->GetNumSprites();
	std::string path = spriteSheet->GetPath();
	for (int i = 0; i < numSprites; i++){
		CreateOrGetSprite(path + "_" + std::to_string(i), spriteSheet->GetTexture(), spriteSheet->GetTexCoordsForSpriteIndex(i), pivot, spriteDimensions);
	}

}

void Renderer::LoadShadersFromFile(std::string fileName)
{
	tinyxml2::XMLDocument shaderDoc;
	std::string filePath = "Data/Data/" + fileName;
	shaderDoc.LoadFile(filePath.c_str());

	for (tinyxml2::XMLElement* shaderElement = shaderDoc.FirstChildElement("shader"); shaderElement != NULL; shaderElement = shaderElement->NextSiblingElement("shader")){
		new Shader(shaderElement, this);
		//TileDefinition::s_definitions.insert(std::pair<std::string, TileDefinition>(newDefinition.m_name, newDefinition));
	}
	return;
}

void Renderer::LoadMaterialsFromFile(std::string fileName)
{
	tinyxml2::XMLDocument matDoc;
	std::string filePath = "Data/Data/" + fileName;
	matDoc.LoadFile(filePath.c_str());

	for (tinyxml2::XMLElement* matElement = matDoc.FirstChildElement("material"); matElement != NULL; matElement = matElement->NextSiblingElement("material")){
		new Material(matElement);
	}
}

Sprite * Renderer::CreateOrGetSprite(const std::string & name, const Texture * texture, AABB2 uvs, Vector2 pivot, Vector2 dimensions)
{
	auto containsSprite = m_loadedSprites.find(name); 
	Sprite* loadedSprite = nullptr;
	if (containsSprite != m_loadedSprites.end()){
		loadedSprite = containsSprite->second;
	} else {
		loadedSprite = new Sprite(texture, uvs, pivot, dimensions);
		m_loadedSprites.insert(std::pair<std::string, Sprite*> (name, loadedSprite));
	}
	return loadedSprite;
}

Sprite * Renderer::GetSprite(const std::string & name)
{
	auto containsSprite = m_loadedSprites.find(name); 
	Sprite* loadedSprite = nullptr;
	if (containsSprite != m_loadedSprites.end()){
		loadedSprite = containsSprite->second;
	}
	return loadedSprite;
}

Texture * Renderer::CreateOrGetTexture(const std::string & path, const std::string & directory, bool setMips)
{
	return Texture::CreateOrGetTexture(path, directory, setMips);		//gotsta go back and remove this from everywhere
	//int indexOfTexture = -1;
	//int numTexturesLoaded = (int) m_loadedTexturePaths.size();
	//for (int i = 0; i <numTexturesLoaded; i++){
	//	if (m_loadedTexturePaths[i] == path){
	//		indexOfTexture = i;
	//		break;
	//	}
	//}

	////the texture hasn't been loaded yet
	//if (indexOfTexture == -1){
	//	Texture* newTexture = new Texture(directory + path);
	//	m_loadedTextures.push_back(newTexture);
	//	m_loadedTexturePaths.push_back(path);
	//	return newTexture;
	//} else {
	//	return m_loadedTextures[indexOfTexture];
	//}
}

BitmapFont * Renderer::CreateOrGetBitmapFont(const char * bitmapFontName)
{
	auto containsFont = m_loadedFonts.find((std::string)bitmapFontName); 
	BitmapFont* loadedFont = nullptr;
	if (containsFont != m_loadedFonts.end()){
		loadedFont = containsFont->second;
	} else {
		Texture* glyphTexture = CreateOrGetTexture(bitmapFontName + FONT_EXTENSION, FONT_DIRECTORY, false);
		SpriteSheet* glyphSheet = new SpriteSheet(*glyphTexture, 16, 16);
		loadedFont = new BitmapFont(bitmapFontName, *glyphSheet, 1.f);
		m_loadedFonts.insert(std::pair<std::string, BitmapFont*> ((std::string)bitmapFontName, loadedFont));
	}
	return loadedFont;
}

void Renderer::DrawText2D(const std::string & asciiText, const Vector2 & drawMins, float cellHeight, const RGBA & tint, const BitmapFont * font, float aspectScale)
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	BitmapFont* drawFont;
	if (font == nullptr){
		drawFont = CreateOrGetBitmapFont("DefaultFont");
		if (drawFont == nullptr){
			ERROR_AND_DIE("No default font and no font specified to draw!");
		}
	} else {
		drawFont = CreateOrGetBitmapFont(font->m_fontName.c_str());
	}

	Vector2 drawPos = drawMins;
	Vector2 increment = Vector2(cellHeight, 0);
	Texture fontTexture = *drawFont->GetTexture();
	BindTexture(fontTexture);
	MeshBuilder mb;
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	for (int charIndex = 0; charIndex < (int) asciiText.size(); charIndex++){
		int glyph = asciiText[charIndex];
		AABB2 texCoords = drawFont->GetUVsForGlyph(glyph);
		Vector2 maxs = drawPos + Vector2(cellHeight * aspectScale, cellHeight);
		AABB2 drawBox = AABB2(drawPos, maxs);
		mb.AppendQuad(Vector3(drawBox.mins), Vector3(drawBox.maxs.x, drawBox.mins.y, 0.f), Vector3(drawBox.mins.x, drawBox.maxs.y, 0.f), Vector3(drawBox.maxs), tint, texCoords);
		//DrawAABB2(drawBox, tint, texCoords.mins, texCoords.maxs);
		//for text debugging:
		//DrawAABB2Outline(drawBox, RGBA(0,0,0,128));
		drawPos +=increment;
	}
	mb.End();
	SubMesh* mesh = mb.CreateSubMesh();
	BindStateAndDrawMesh(mesh);
	delete mesh;
	//delete mb;
	ReleaseTexture();

}

void Renderer::DrawText3D(const std::string & asciiText, const Vector3 & drawMins, float cellHeight, const Vector3 & up, const Vector3 & right, const RGBA & tint, const BitmapFont * font, float aspectScale)
{
	BitmapFont* drawFont;
	if (font == nullptr){
		drawFont = CreateOrGetBitmapFont("DefaultFont");
		if (drawFont == nullptr){
			ERROR_AND_DIE("No default font and no font specified to draw!");
		}
	} else {
		drawFont = CreateOrGetBitmapFont(font->m_fontName.c_str());
	}

	Vector3 drawPos = drawMins;
	Vector3 increment = cellHeight * right;		//check?
	Texture fontTexture = *drawFont->GetTexture();
	float cellWidth = cellHeight * aspectScale;
	BindTexture(fontTexture);
	BindShaderProgram("cutout");
	for (int charIndex = 0; charIndex < (int) asciiText.size(); charIndex++){
		int glyph = asciiText[charIndex];
		AABB2 texCoords = drawFont->GetUVsForGlyph(glyph);

		Vector3 botLeft = drawPos ;
		Vector3 botRight = drawPos + (cellWidth * right);
		Vector3 topLeft = drawPos + (cellHeight * up);
		Vector3 topRight = drawPos + (cellWidth * right) + (cellHeight * up);

		DrawPlane(botLeft, botRight, topRight, topLeft, tint, texCoords.mins, texCoords.maxs);
		drawPos = botRight;
	}
	ReleaseShaderProgram();
	ReleaseTexture();
}

float Renderer::DrawTextInBox2D(const std::string & asciiText, AABB2 box, Vector2 alignment, float cellHeight, eTextDrawMode drawMode, const RGBA & tint, const BitmapFont * font, float aspectScale)
{
	Vector2 sizeOfBox = box.maxs - box.mins;
	//break into lines
	std::vector<std::string> lines;
	Split(asciiText, '\n', lines);

	if (drawMode == TEXT_DRAW_WORD_WRAP){
		WrapLines(lines, box, cellHeight, aspectScale);
	}
	if (drawMode == TEXT_DRAW_SHRINK_TO_FIT){
		float cellScale = FindScaleToFit(lines, box, cellHeight, aspectScale);
		cellHeight *= cellScale;
	}
	return DrawTextLines(lines, box, alignment, cellHeight, tint, font, aspectScale);

}

float Renderer::GetTextWidth(const std::string & asciiText, float cellHeight, float aspectScale)
{
	return asciiText.size() * (cellHeight * aspectScale);
}

void Renderer::SetCamera(Camera * newCamera)
{
	if (newCamera == nullptr){
		newCamera = m_defaultCamera;
	}
	newCamera->Finalize();
	m_currentCamera = newCamera;
}

Texture * Renderer::CreateRenderTarget(unsigned int width, unsigned int height, eTextureFormat format)
{
	Texture* tex = new Texture();
	tex->CreateRenderTarget(width, height, format);
	return tex;
}

Texture * Renderer::CreateDepthStencilTarget(unsigned int width, unsigned int height)
{
	return CreateRenderTarget(width, height, TEXTURE_FORMAT_D24S8);
}

ShaderProgram * Renderer::CreateOrGetShaderProgram(const std::string & shaderRoot)
{
	auto containsShader = m_loadedShaderPrograms.find((std::string)shaderRoot); 
	ShaderProgram* loadedShader = nullptr;
	if (containsShader != m_loadedShaderPrograms.end()){
		loadedShader = containsShader->second;
	} else {
		std::string shaderPath = SHADER_DIRECTORY + shaderRoot;
		loadedShader = new ShaderProgram(shaderPath.c_str());
		if (loadedShader->GetHandle() == NULL){			//handle is null if shader compilation fails
			//shader failed to compile - use invalid shader
			GL_CHECK_ERROR();
			ASSERT_OR_DIE(shaderRoot != "invalid", "Invalid shader failed to compile!");
			ASSERT_OR_DIE(m_invalidShader->m_program != nullptr, "Invalid shader never initialized!");
			//loadedShader = m_invalidShader->m_program;
			delete loadedShader;
			return m_invalidShader->m_program;
		} else {
			m_loadedShaderPrograms.insert(std::pair<std::string, ShaderProgram*> (shaderRoot, loadedShader));
		}
	}
	return loadedShader;
}

ShaderProgram * Renderer::CreateOrGetShaderProgram(const std::string & vsRoot, const std::string & fsRoot)
{
	std::string combinedID = vsRoot + fsRoot;
	auto containsShader = m_loadedShaderPrograms.find(combinedID); 
	ShaderProgram* loadedShader = nullptr;
	if (containsShader != m_loadedShaderPrograms.end()){
		loadedShader = containsShader->second;
	} else {
		loadedShader = new ShaderProgram(vsRoot.c_str(), fsRoot.c_str());
		if (loadedShader->GetHandle() == NULL){			//handle is null if shader compilation fails
														//shader failed to compile - use invalid shader
			GL_CHECK_ERROR();
			ASSERT_OR_DIE(vsRoot != "invalid", "Invalid shader failed to compile!");
			ASSERT_OR_DIE(m_invalidShader->m_program != nullptr, "Invalid shader never initialized!");
			loadedShader = m_invalidShader->m_program;
		} else {
			m_loadedShaderPrograms.insert(std::pair<std::string, ShaderProgram*> (combinedID, loadedShader));
		}
	}
	return loadedShader;
}

ShaderProgram * Renderer::CreatePreDefinedShaderProgram(const std::string & rootName, const char*  vertexShader, const char*  fragmentShader)
{
	auto containsShader = m_loadedShaderPrograms.find((std::string)rootName); 
	ShaderProgram* loadedShader = nullptr;
	if (containsShader != m_loadedShaderPrograms.end()){
		loadedShader = containsShader->second;
	} else {
		loadedShader = new ShaderProgram(vertexShader, fragmentShader);
		if (loadedShader->GetHandle() == NULL){			//this should never happen for pre-defined shaders :v
			//shader failed to compile - use invalid shader
			ASSERT_OR_DIE(rootName != "invalid", "Invalid shader failed to compile!");
			loadedShader = CreateOrGetShaderProgram("invalid");	//this seems slightly Unsafe(TM)
		} else {
			m_loadedShaderPrograms.insert(std::pair<std::string, ShaderProgram*> (rootName, loadedShader));
		}
	}
	return loadedShader;
}

void Renderer::RecompileAllShaders()
{
	for (std::pair<std::string, ShaderProgram*> shaderElement : m_loadedShaderPrograms) {
		// Accessing KEY from element
		std::string root = SHADER_DIRECTORY + shaderElement.first;
		shaderElement.second->RecompileFromRoot(root.c_str());
	}
}

void Renderer::SetDepth(eCompare compare, bool should_write )
{
	// enable/disable the dest
	glEnable( GL_DEPTH_TEST ); 
	GLenum comp = ToGLCompare(compare);
	glDepthFunc( comp ); 

	// enable/disable write
	glDepthMask( should_write ? GL_TRUE : GL_FALSE ); 
}

void Renderer::SetFillMode(eFillMode fill)
{
	switch (fill){
	case FILL_MODE_FILL:
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		break;
	case FILL_MODE_WIRE:
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		break;
	}
}

void Renderer::SetCullMode(eCullMode cull)
{
 	switch(cull){
	case CULLMODE_BACK:
		glEnable(GL_CULL_FACE); 
		glCullFace(GL_BACK);
		break;
	case CULLMODE_FRONT:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	case CULLMODE_NONE:
		glDisable(GL_CULL_FACE);
		break;
	}
}

void Renderer::SetWindOrder(eWindOrder wind)
{
	switch (wind) {
	case WIND_COUNTER_CLOCKWISE:
		//glEnable(GL_FRONT_FACE);
		glFrontFace(GL_CCW);
		break;
	case WIND_CLOCKWISE:
		//glEnable(GL_FRONT_FACE);
		glFrontFace(GL_CW);
		break;

	}
}

void Renderer::SetBlendMode(eBlendMode blend)
{
	switch (blend){
	case BLEND_MODE_ADDITIVE:
		SetAdditiveBlending();
		break;
	case BLEND_MODE_ALPHA:
		SetAlphaBlending();
		break;
	case BLEND_MODE_MULTIPLY:
		SetMultiplyBlending();
		break;
	case BLEND_MODE_NONE:
		DisableBlending();
		break;
	}
}

void Renderer::PostStartup()
{
	wglSwapIntervalEXT(0);
	m_ambientLight;


	m_samplers.push_back(new Sampler());
	GL_CHECK_ERROR();
	std::vector<RGBA> white22 ={ RGBA::WHITE, RGBA::WHITE, RGBA::WHITE, RGBA::WHITE};
	Image* whiteImage = new Image(white22, IntVector2(2,2));
	Texture::RegisterTextureFromImage("white", whiteImage);
	RGBA flat = RGBA(128,128,255,255);
	std::vector<RGBA> flat22 ={ flat, flat, flat, flat};
	Image* flatImage = new Image(flat22, IntVector2(2,2));
	Texture::RegisterTextureFromImage("flat", flatImage);
	m_defaultTexture = CreateOrGetTexture("white");
	GL_CHECK_ERROR();

	// m_defaultVAO is a GLuint member variable
	glGenVertexArrays( 1, &m_defaultVAO ); 
	GL_CHECK_ERROR();
	glBindVertexArray( m_defaultVAO );

	GL_CHECK_ERROR();
	// m_defaultShaderProgram is a ShaderProgram* member
	m_invalidShader = new Shader("invalid", CreatePreDefinedShaderProgram("invalid", INVALID_VS, INVALID_FS));
	m_defaultShader = new Shader("default", CreatePreDefinedShaderProgram("default", DEFAULT_VS, DEFAULT_FS));

	m_currentShader = m_defaultShader;  

	glEnable(GL_BLEND);


	m_defaultColorTarget = CreateRenderTarget((int) g_Window->GetWidth(), (int) g_Window->GetHeight());
	m_defaultDepthTarget = CreateDepthStencilTarget((int) g_Window->GetWidth(), (int) g_Window->GetHeight());

	m_defaultCamera = new Camera();
	m_defaultCamera->SetColorTarget(m_defaultColorTarget);
	m_defaultCamera->SetDepthStencilTarget(m_defaultDepthTarget);

	m_defaultCamera->SetProjectionOrtho(1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);
	Vector2 center = m_defaultCamera->GetBounds().GetCenter();
	m_defaultCamera->LookAt( Vector3( center.x, center.y, -1.f ), Vector3(center.x, center.y, .5f)); 

	m_effectCamera = new Camera();
	m_effectCamera->SetColorTarget(m_defaultColorTarget);
	//m_effectCamera->SetDepthStencilTarget(m_defaultDepthTarget);

	m_copyFBO = new FrameBuffer();
	m_copyFBOScratch = new FrameBuffer();

	SetCamera(nullptr);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	

}

void Renderer::CopyTexture(Texture * dst, Texture * src)
{
	m_copyFBOScratch->SetColorTarget(src);
	m_copyFBOScratch->Finalize();
	if (dst == nullptr){
		CopyFrameBuffer(nullptr, m_copyFBOScratch);
	} else {
		m_copyFBO->SetColorTarget(dst);
		m_copyFBO->Finalize();

		CopyFrameBuffer(m_copyFBO, m_copyFBOScratch);
	}
}

void Renderer::WrapLines(Strings & lines, AABB2 box, float cellHeight, float aspectScale)
{
	float boxWidth = box.maxs.x - box.mins.x;
	unsigned int lineNum = 0;
	while( lineNum < lines.size()){
		std::string line = lines[lineNum];
		float lineWidth = line.size() * (cellHeight * aspectScale);
		if (lineWidth > boxWidth){
			Strings words;
			Split(line, ' ', words);
			float newLineWidth = 0.f;
			std::string newLine = "";
			std::string lineRemainder = "";
			//split the line into a new line that fits, and add the remainder of the line as a new line in the lines vector
			for (unsigned int wordNum = 0; wordNum < words.size(); wordNum++){
				std::string word = words[wordNum];
				float wordLength = (word.size() + 1) * (cellHeight * aspectScale);
				newLineWidth+= wordLength;
				if (newLineWidth < boxWidth || ((wordLength >=boxWidth) && newLineWidth == wordLength)){		//need a case for if a word is longer than the box
					newLine = newLine + word + " ";
				} else {
					lineRemainder = lineRemainder + word + " ";
				}
			}
			newLine = newLine.substr(0,newLine.size() - 1);
			lineRemainder = lineRemainder.substr(0, lineRemainder.size() - 1);
			auto startIter = lines.begin();
			lines[lineNum] = newLine;
			lines.insert( startIter + lineNum + 1, lineRemainder);
		}
		lineNum++;
	}
}

float Renderer::FindScaleToFit(Strings lines, AABB2 box, float cellHeight, float aspectScale)
{
	float textHeight = lines.size() * cellHeight;
	float textWidth = 0.f;
	float boxWidth = (box.maxs.x - box.mins.x);
	float boxHeight = (box.maxs.y - box.mins.y);

	for (std::string line : lines){
		float lineSize = line.size() * (cellHeight * aspectScale);
		if (lineSize > textWidth){
			textWidth = lineSize;
		}
	}
	float scaleX = 1.f;
	float scaleY = 1.f;
	if (textWidth > boxWidth){
		scaleX = boxWidth / textWidth;
	}
	if (textHeight > boxHeight){
		scaleY = boxHeight / textHeight;
	}
	if (scaleY < scaleX){
		return scaleY;
	}
	return scaleX;
}

float Renderer::DrawTextLines(Strings lines, AABB2 box, Vector2 alignment, float cellHeight, const RGBA & tint, const BitmapFont* font, float aspectScale)
{
	Vector2 sizeOfBox = box.maxs - box.mins;
	int maxLines = (int) lines.size();
	float maxWidth = 0.f;
	for(int lineNum = 0; lineNum < maxLines; lineNum++){
		std::string line = lines[lineNum];
		float lineWidth = line.size() * (cellHeight * aspectScale);
		if (lineWidth > maxWidth){
			maxWidth = lineWidth;
		}
		Vector2 leftoverSpace = Vector2(sizeOfBox.x - lineWidth, sizeOfBox.y - (cellHeight * maxLines));
		float lineOffsetY = cellHeight * (maxLines - lineNum);
		Vector2 posToDraw = box.mins + Vector2(leftoverSpace.x * alignment.x, (leftoverSpace.y * alignment.y) + lineOffsetY);
		posToDraw.y = posToDraw.y - (cellHeight);
		DrawText2D(line, posToDraw, cellHeight, tint, font, aspectScale);
	}
	return maxWidth;
}

void Renderer::AddFaceToCube(const Vector3 & mins, const Vector3 & maxs, Vertex3D_PCU(&vertices)[36], unsigned int& startIndex, const RGBA & color, Vector2 uvMins, Vector2 uvMaxs)
{
	Vector3 topLeft		= Vector3(mins.x, maxs.y, mins.z);			//texCoords 0,1
	Vector3 topRight	= Vector3(maxs.x, maxs.y, maxs.z);			//texCoords 1,1
	Vector3 bottomLeft	= Vector3(mins.x, mins.y, mins.z);			//texCoords 0,0
	Vector3 bottomRight = Vector3(maxs.x, mins.y, maxs.z);			//texCoords 1,0

	vertices[startIndex + 0] = Vertex3D_PCU(topLeft,	 color,	Vector2(uvMins.x,uvMaxs.y));
	vertices[startIndex + 1] = Vertex3D_PCU(topRight,	 color, Vector2(uvMaxs.x,uvMaxs.y));
	vertices[startIndex + 2] = Vertex3D_PCU(bottomLeft,  color, Vector2(uvMins.x,uvMins.y));
	vertices[startIndex + 3] = Vertex3D_PCU(bottomLeft,  color, Vector2(uvMins.x,uvMins.y));
	vertices[startIndex + 4] = Vertex3D_PCU(topRight,	 color, Vector2(uvMaxs.x,uvMaxs.y));
	vertices[startIndex + 5] = Vertex3D_PCU(bottomRight, color, Vector2(uvMaxs.x,uvMins.y));
	startIndex+=6;
}

void Renderer::AddFaceToCube(const Vector3 & nearBottomLeft, const Vector3 & nearBottomRight, const Vector3 & farTopRight, const Vector3 & farTopLeft, Vertex3D_PCU(&vertices)[36], unsigned int & startIndex, const RGBA & color, Vector2 uvMins, Vector2 uvMaxs)
{

	vertices[startIndex + 0] = Vertex3D_PCU(farTopLeft,		 color,	Vector2(uvMins.x,uvMaxs.y));
	vertices[startIndex + 1] = Vertex3D_PCU(farTopRight,	 color, Vector2(uvMaxs.x,uvMaxs.y));
	vertices[startIndex + 2] = Vertex3D_PCU(nearBottomLeft,  color, Vector2(uvMins.x,uvMins.y));
	vertices[startIndex + 3] = Vertex3D_PCU(nearBottomLeft,  color, Vector2(uvMins.x,uvMins.y));
	vertices[startIndex + 4] = Vertex3D_PCU(farTopRight,	 color, Vector2(uvMaxs.x,uvMaxs.y));
	vertices[startIndex + 5] = Vertex3D_PCU(nearBottomRight, color, Vector2(uvMaxs.x,uvMins.y));
	startIndex+=6;
}

void FogData_t::SetFogBuffer(RGBA color, float nearPlane, float farPlane, float nearFactor, float farFactor)
{
	fogColor = color.GetNormalized();
	fogNearPlane = nearPlane;
	fogFarPlane = farPlane;
	fogNearFactor = nearFactor;
	fogFarFactor = farFactor;
}

void FogData_t::SetFogColor(RGBA color)
{
	fogColor = color.GetNormalized();
}