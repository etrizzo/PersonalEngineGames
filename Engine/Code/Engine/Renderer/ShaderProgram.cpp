#include "Engine/Renderer/ShaderProgram.hpp"




ShaderProgram::ShaderProgram(const char * shaderRoot)
{
	LoadFromFile(shaderRoot);
}

ShaderProgram::ShaderProgram(const char * vertexShader, const char * fragmentShader, bool isLiteral)
{
	if (isLiteral){
		LoadFromLiteral(vertexShader, fragmentShader);
	} else {
		LoadFromFile(vertexShader, fragmentShader);
	}
}

bool ShaderProgram::RecompileFromRoot(const char* shaderRoot)
{
	if (!m_fromLiteral){
		LoadFromFile(shaderRoot);
		return true;
	}
	return false;
}

bool ShaderProgram::LoadFromFile(const char * shaderRoot)
{
	std::string vs_file = shaderRoot;
	vs_file += ".vs"; 

	std::string fs_file = shaderRoot; 
	fs_file += ".fs"; 

	return LoadFromFile(vs_file.c_str(), fs_file.c_str());

}

bool ShaderProgram::LoadFromFile(const char * vs, const char * fs)
{
	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vert_shader = LoadShader( vs, GL_VERTEX_SHADER ); 
	GLuint frag_shader = LoadShader( fs, GL_FRAGMENT_SHADER ); 


	if (vert_shader == NULL || frag_shader == NULL){
		m_programHandle = NULL;
	} else {
		// Link the program
		// program_handle is a member GLuint. 
		m_programHandle = CreateAndLinkProgram( vert_shader, frag_shader ); 
		glDeleteShader( vert_shader ); 
		glDeleteShader( frag_shader ); 
	}
	return (m_programHandle != NULL); 
}

bool ShaderProgram::LoadFromLiteral(const char * vertexShader, const char * fragmentShader)
{
	GLuint vert_shader = LoadShaderFromLiteral( vertexShader, GL_VERTEX_SHADER ); 
	GLuint frag_shader = LoadShaderFromLiteral( fragmentShader, GL_FRAGMENT_SHADER ); 

	m_fromLiteral = true;
	if (vert_shader == NULL || frag_shader == NULL){
		m_programHandle = NULL;
	} else {
		// Link the program
		// program_handle is a member GLuint. 
		m_programHandle = CreateAndLinkProgram( vert_shader, frag_shader ); 
		glDeleteShader( vert_shader ); 
		glDeleteShader( frag_shader ); 
	}
	return (m_programHandle != NULL); 
}




static GLuint LoadShader( char const *filename, GLenum type )
{
	char *src = (char*)FileReadToNewBuffer(filename);
	ASSERT_OR_DIE(src != nullptr, "Invalid shader path");

	// Create a shader
	GLuint shader_id = glCreateShader(type);
	ASSERT_OR_DIE(shader_id != NULL, "Failed to create shader");

	std::string sourceWithIncludes = GetIncludes(src);
	char* newSrc = (char*) sourceWithIncludes.c_str();

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(newSrc);
	glShaderSource(shader_id, 1, &newSrc, &shader_length);

	


	glCompileShader(shader_id);

	// Check status
	GLint status;
	GLint InfoLogLength;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(shader_id, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		DebuggerPrintf("%s\n", &VertexShaderErrorMessage[0]);
	}
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		LogShaderError(shader_id); // function we write
		glDeleteShader(shader_id);
		shader_id = NULL;
	}

	free(src);

	return shader_id;
}

GLuint LoadShaderFromLiteral(char const * shaderLiteral, GLenum type)
{

	// Create a shader
	GLuint shader_id = glCreateShader(type);
	ASSERT_OR_DIE(shader_id != NULL, "Failed to create shader");

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(shaderLiteral);
	glShaderSource(shader_id, 1, &shaderLiteral, &shader_length);
	glCompileShader(shader_id);

	// Check status
	GLint status;
	GLint InfoLogLength;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(shader_id, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		DebuggerPrintf("%s\n", &VertexShaderErrorMessage[0]);
	}
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		LogShaderError(shader_id); // function we write
		glDeleteShader(shader_id);
		shader_id = NULL;
	}

	return shader_id;
}

void LogShaderError(GLuint shader_id)
{
	// figure out how large the buffer needs to be
	GLint length;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

	// make a buffer, and copy the log to it. 
	char *buffer = new char[length + 1];
	glGetShaderInfoLog(shader_id, length, &length, buffer);

	// Print it out (may want to do some additional formatting)
	buffer[length] = NULL;
	DebuggerPrintf( "class: %i", buffer );
	//DEBUGBREAK();

	// free up the memory we used. 
	delete buffer;
}

static GLuint CreateAndLinkProgram( GLint vs, GLint fs )
{
	// create the program handle - how you will reference
	// this program within OpenGL, like a texture handle
	GLuint program_id = glCreateProgram();
	ASSERT_OR_DIE( program_id != 0 , "Failed to create shader program");

	// Attach the shaders you want to use
	glAttachShader( program_id, vs );
	glAttachShader( program_id, fs );

	// Link the program (create the GPU program)
	glLinkProgram( program_id );

	// Check for link errors - usually a result
	// of incompatibility between stages.
	GLint link_status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

	if (link_status == GL_FALSE) {
		LogProgramError(program_id);
		glDeleteProgram(program_id);
		program_id = 0;
	} 

	// no longer need the shaders, you can detach them if you want
	// (not necessary)
	glDetachShader( program_id, vs );
	glDetachShader( program_id, fs );

	return program_id;
}


static void LogProgramError(GLuint program_id)
{
	// get the buffer length
	GLint length;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);

	// copy the log into a new buffer
	char *buffer = new char[length + 1];
	glGetProgramInfoLog(program_id, length, &length, buffer);

	// print it to the output pane
	buffer[length] = NULL;
	DebuggerPrintf("class", buffer);
	//DEBUGBREAK();           

	// cleanup
	delete buffer;
}

std::string GetIncludes(char * source)
{
	std::string src = std::string(source);
	size_t found = src.find("#include");
	Strings includedFiles = Strings();
	while (found != std::string::npos){
		//get the filename
		size_t quote1 = src.find('\"', found + 1 );
		size_t quote2 = src.find('\"', quote1 + 1);
		std::string fileName = src.substr(quote1 + 1, quote2 - quote1 - 1);

		//get the included file source
		char* includeSource = (char*) FileReadToNewBuffer((SHADER_DIRECTORY + fileName).c_str());
		ASSERT_OR_DIE(includeSource != nullptr, "Invalid shader path in include: " + fileName);
		std::string includeAsString = std::string(includeSource);

		//replace the whole #include line with the source
		src.replace(found, quote2 + 1 - found, includeAsString);
		found = src.find("#include");

	}
	return src;
}
