#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Renderer.hpp"

class ShaderProgram;

// This is the part of Shader.hpp
// Shader is just a combination of a program and render state
class Shader
{
public:
	Shader(std::string programRoot, Renderer* r);
	Shader(std::string name, ShaderProgram* program);
	Shader(tinyxml2::XMLElement* shaderElement, Renderer* r);;
	~Shader(){};
	// none of these call GL calls, just sets the internal state
	void SetProgram( ShaderProgram* program); 

	void EnableBlending( eBlendMode mode ); 
	void DisableBlending();

	void SetDepth( eCompare compare, bool write ); 
	void DisableDepth() { SetDepth( COMPARE_ALWAYS, false ); }

	void SetCullMode( eCullMode mode); 
	void SetFillMode( eFillMode mode); 
	void SetFrontFace( eWindOrder order); 

	GLuint GetProgramHandle() const;

	static std::map <std::string, Shader*>	s_shaders;
	static Shader* GetShader(std::string name);

public:
	ShaderProgram *m_program; 
	RenderState m_state;     
	bool m_lit;
	int m_sortLayer = 0;

}; 