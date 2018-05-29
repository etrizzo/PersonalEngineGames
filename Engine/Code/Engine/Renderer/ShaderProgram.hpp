#pragma once
#include "Engine/Math/Renderer.hpp"

class ShaderProgram{
public:
	ShaderProgram() { m_programHandle = NULL; };
	ShaderProgram(const char* shaderRoot);
	ShaderProgram(const char* vertexShader, const char* fragmentShader, bool isLiteral = true);
	~ShaderProgram() {};

	//recompiles from a new root if the shader was not initially loaded from a string literal
	bool RecompileFromRoot(const char* shaderRoot);

	bool LoadFromFile(const char* shaderRoot);
	bool LoadFromFile(const char* vs, const char* fs);
	bool LoadFromLiteral(const char* vertexShader, const char* fragmentShader);

	GLuint GetHandle() const { return m_programHandle; };

private:
	GLuint m_programHandle;
	bool m_fromLiteral = false;

};

static GLuint LoadShader( char const *filename, GLenum type );
static GLuint LoadShaderFromLiteral( char const *shaderLiteral, GLenum type);
static void LogShaderError(GLuint shader_id);
static GLuint CreateAndLinkProgram( GLint vs, GLint fs );
static void LogProgramError(GLuint program_id);


std::string GetIncludes(char* src);

