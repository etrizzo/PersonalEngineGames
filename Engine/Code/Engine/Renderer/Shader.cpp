#include "Shader.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"

std::map<std::string, Shader*>	Shader::s_shaders;

Shader::Shader(std::string programRoot, Renderer* r)
{
	m_program = r->CreateOrGetShaderProgram(programRoot);
	m_state = RenderState();
	s_shaders.insert(std::pair<std::string, Shader*>(programRoot, this));
}

Shader::Shader(std::string name, ShaderProgram * program)
{
	m_program = program;
	m_state = RenderState();
	s_shaders.insert(std::pair<std::string, Shader*>(name, this));
}

Shader::Shader(tinyxml2::XMLElement * shaderElement, Renderer* r)
{
	/*
	Format:
	<shader name="name" sort="0" cull="back|front|none" fill="solid|wire" frontface="ccw|cw" blend = "alpha|additive|multiply" lit="true|false">
		<program id="opt_program_id" define="semicolon;seperated;list">
		<!--  if no id, you can define the program here manually  -->
		<!--  id and defines are mutually exclusive  -->
			<vertex file="shader/glsl/mvp_lit.vs"/>
			<fragment file="shader/glsl/lit.fs"/>
		</program>
		<depth write="true|false" test="less|never|equal|lequal|greater|gequal|not|always"/>
	</shader>
	*/

	//get shader data
	std::string name	 = ParseXmlAttribute(*shaderElement, "name"			, "NO_NAME");
	std::string cullStr  = ParseXmlAttribute(*shaderElement, "cull"			, "back");
	std::string fillStr  = ParseXmlAttribute(*shaderElement, "fill"			, "solid");
	std::string faceStr  = ParseXmlAttribute(*shaderElement, "frontFace"	, "ccw");
	std::string blendStr = ParseXmlAttribute(*shaderElement, "blend"		, "alpha");
	std::string litStr	 = ParseXmlAttribute(*shaderElement, "lit"			, "true");
	m_sortLayer			 = ParseXmlAttribute(*shaderElement, "sort"			, 0);

	//get depth data
	tinyxml2::XMLElement* depthElement = shaderElement->FirstChildElement("depth");
	std::string testStr = ParseXmlAttribute(*depthElement, "test", "less");
	bool write			= ParseXmlAttribute(*depthElement, "write", true);

	//get program data
	tinyxml2::XMLElement* progElement = shaderElement->FirstChildElement("program");
	std::string programID = ParseXmlAttribute(*progElement, "id", "");
	if (programID == ""){
		//get vert and fragment shaders and load from those
		std::string vsFile = ParseXmlAttribute(*(progElement->FirstChildElement("vertex")), "file", "NO_FILE");
		std::string fsFile = ParseXmlAttribute(*(progElement->FirstChildElement("fragment")), "file", "NO_FILE");
		m_program = r->CreateOrGetShaderProgram(vsFile, fsFile);
	} else {
		m_program = r->CreateOrGetShaderProgram(programID);
	}
	m_state;
	SetCullMode(	StringToCullMode(cullStr)	);
	SetFillMode(	StringToFillMode(fillStr)	);
	SetFrontFace(	StringToFrontFace(faceStr)  );
	EnableBlending(	StringToBlendMode(blendStr)	);

	SetDepth(	StringToDepthCompare(testStr), write);

	if (litStr == "true"){
		m_lit = true;
	} else {
		m_lit = false;
	}

	s_shaders.insert(std::pair<std::string, Shader*>(name, this));
}

void Shader::SetProgram(ShaderProgram * program)
{
	m_program = program;
}

void Shader::EnableBlending(eBlendMode mode)
{
	m_state.m_blendMode = mode;
}

void Shader::DisableBlending()
{
	EnableBlending(BLEND_MODE_NONE);
}

void Shader::SetDepth(eCompare compare, bool write)
{
	m_state.m_depthCompare = compare;
	m_state.m_depthWrite = write;
}

void Shader::SetCullMode(eCullMode mode)
{
	m_state.m_cullMode = mode;
}

void Shader::SetFillMode(eFillMode mode)
{
	m_state.m_fillMode = mode;
}

void Shader::SetFrontFace(eWindOrder order)
{
	m_state.m_frontFace = order;
}

GLuint Shader::GetProgramHandle() const
{
	return m_program->GetHandle();
}

Shader * Shader::GetShader(std::string name)
{
	auto contains = s_shaders.find((std::string)name); 
	Shader* shader = nullptr;
	if (contains != s_shaders.end()){
		shader = contains->second;
	} 
	return shader;
}
