#include "MaterialProperty.hpp"
#include "Engine/Math/Renderer.hpp"

MaterialPropertyFloat::MaterialPropertyFloat(std::string name, float val)
	:MaterialProperty(name)
{
	m_value = val;
}

void MaterialPropertyFloat::Bind(unsigned int programHandle)
{
	int bind_point = glGetUniformLocation(programHandle, m_name.c_str());		// this can also be done in the set_material function and just have the actual bind
	if (bind_point >= 0){
		glProgramUniform1f(programHandle, bind_point, (GLfloat) m_value);
	}
}

MaterialProperty::MaterialProperty(std::string name)
{
	m_name = name;
}

MaterialPropertyRGBA::MaterialPropertyRGBA(std::string name, RGBA val)
	:MaterialProperty(name)
{
	m_value = val;
}

void MaterialPropertyRGBA::Bind(unsigned int programHandle)
{
	int bind_point = glGetUniformLocation(programHandle, m_name.c_str());		// this can also be done in the set_material function and just have the actual bind
	Vector4 floatVals = m_value.GetNormalized();
	if (bind_point >= 0){
		//glProgramUniform4fv(programHandle, bind_point, 1, (GLfloat*)&floatVals);
		glUniform4fv(bind_point, 1, (GLfloat*) &floatVals);
	}
}
