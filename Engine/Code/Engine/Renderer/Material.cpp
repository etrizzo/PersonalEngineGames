#include "Material.hpp"
#pragma once
#include "Engine/Renderer/Sampler.hpp"

std::map<std::string, Material*> Material::s_materials;

Material::Material()
{
	m_name = "NO_NAME";
	m_lit = true;
	//m_properties = std::vector<MaterialProperty>();
	SetProperty("SPECULAR_AMOUNT", DEFAULT_SPEC_AMOUNT);
	SetProperty("SPECULAR_POWER" , DEFAULT_SPEC_POWER);
	SetProperty("TINT", RGBA::WHITE);
}

Material::Material(tinyxml2::XMLElement * matElement)
{
	m_name					= ParseXmlAttribute(*matElement, "name"		, "NO_NAME");
	std::string shaderName	= ParseXmlAttribute(*matElement, "shader"	, "default");

	m_shader = Shader::GetShader(shaderName);

	tinyxml2::XMLElement* textureElement = matElement->FirstChildElement("textures");
	tinyxml2::XMLElement* propsElement = matElement->FirstChildElement("properties");

	if (textureElement != nullptr){
		for (tinyxml2::XMLElement* texElement = textureElement->FirstChildElement("texture"); texElement != nullptr; texElement = texElement->NextSiblingElement("texture")){
			int idx = ParseXmlAttribute(*texElement, "index", -1);
			std::string path = ParseXmlAttribute(*texElement, "path", "NO_PATH");
			ASSERT_OR_DIE(path != "NO_PATH", "Must specify texture path in material data!!!!!");
			if (idx == -1){
				idx = (int) m_textures.size();
			}
			SetTexture(idx, Texture::CreateOrGetTexture(path, IMAGE_DIRECTORY));
		}
	}

	if (propsElement != nullptr){
		for (tinyxml2::XMLElement* propElement = propsElement->FirstChildElement("property"); propElement != nullptr; propElement = propElement->NextSiblingElement("property")){
			std::string propType = ParseXmlAttribute(*propElement, "type", "float");
			std::string propName = ParseXmlAttribute(*propElement, "name", "NO_NAME");
			if (propType == "float"){
				float valueFloat = ParseXmlAttribute(*propElement, "value", 1.f);
				SetProperty(propName.c_str(), valueFloat);
			} else if (propType == "RGBA"){
				RGBA valueRGBA = ParseXmlAttribute(*propElement, "value", RGBA::WHITE);
				SetProperty(propName.c_str(), valueRGBA);
			}
		}
	}

	s_materials.insert(std::pair<std::string, Material*>(m_name, this));
}

Material::Material(std::string name)
{
	m_name = name;
	m_lit = true;
	//m_properties = std::vector<MaterialProperty>();
	SetProperty("SPECULAR_AMOUNT", DEFAULT_SPEC_AMOUNT);
	SetProperty("SPECULAR_POWER" , DEFAULT_SPEC_POWER);
	SetProperty("TINT", RGBA::WHITE);
	s_materials.insert(std::pair<std::string, Material*>(name, this));
}

Material::Material(std::string name, std::string shaderPath)
{
	m_name = name;
	SetShader(shaderPath);
	if (m_lit){
		SetProperty("SPECULAR_AMOUNT", DEFAULT_SPEC_AMOUNT);
		SetProperty("SPECULAR_POWER" , DEFAULT_SPEC_POWER);
	}
	SetProperty("TINT", RGBA::WHITE);
	s_materials.insert(std::pair<std::string, Material*>(name, this));
}

Material * Material::Clone()
{
	Material* mat = new Material();
	mat->m_properties	= m_properties;
	mat->m_samplers		= m_samplers;
	mat->m_shader		= m_shader;
	mat->m_textures		= m_textures;
	return mat;
	//return nullptr;
}

void Material::SetTexture(int bindPoint, const Texture * tex)
{
	if (bindPoint >= (int) m_textures.size()){
		m_textures.resize(bindPoint + 1);
		m_samplers.resize(bindPoint + 1);
	}
	m_textures[bindPoint] = tex;

	if (m_samplers[bindPoint] == nullptr){
		m_samplers[bindPoint] = new Sampler();
	}
}

void Material::SetDiffuseTexture(const Texture * tex)
{
	SetTexture(DIFFUSE_BIND_POINT, tex);
}

void Material::SetDiffuseTexture(std::string texPath)
{
	Texture* tex = Texture::CreateOrGetTexture(texPath);
	SetTexture(DIFFUSE_BIND_POINT, tex);
}

void Material::SetNormalTexture(const Texture * tex)
{
	SetTexture(NORMAL_BIND_POINT, tex);
}

void Material::SetNormalTexture(std::string texPath)
{
	Texture* tex ;
	if (texPath == "NONE"){
		tex = Texture::CreateOrGetTexture("flat");
	} else {
		tex = Texture::CreateOrGetTexture(texPath);
	}
	SetTexture(NORMAL_BIND_POINT, tex);
}

void Material::SetShader(std::string shaderName)
{
	SetShader(Shader::GetShader(shaderName));
}

void Material::SetShader(Shader * shader)
{
	m_shader = shader;
	if (m_shader != nullptr){
		m_lit = m_shader->m_lit;
	}
}

void Material::RemoveProperty(const char * name)
{
	for (int i = (int) m_properties.size() - 1; i >=0; i--){
		if (m_properties[i]->m_name == name){
			MaterialProperty* p = m_properties[i];
			RemoveAtFast(m_properties, i);
			delete p;
		}
	}
}

void Material::SetProperty(const char * name, float value)
{
	RemoveProperty( name );		//need this because otherwise you have duplicates
	MaterialPropertyFloat* prop = new MaterialPropertyFloat(name, value);
	m_properties.push_back(prop);
}

void Material::SetProperty(const char * name, RGBA value)
{
	RemoveProperty( name );		//need this because otherwise you have duplicates
	MaterialPropertyRGBA* prop = new MaterialPropertyRGBA(name, value);
	m_properties.push_back(prop);
}

void Material::SetProperty(const char * name, Vector2 value)
{
	RemoveProperty( name );
	MaterialPropertyVec2* prop = new MaterialPropertyVec2(name, value);
	m_properties.push_back(prop);
}

Strings Material::LoadMaterials(std::string mtlFile)
{
	MtlLoader loader = MtlLoader(mtlFile);
	return loader.m_loadedMaterials;

}

Material * Material::GetMaterial(std::string name)
{
	auto contains = s_materials.find((std::string)name); 
	Material* mat = nullptr;
	if (contains != s_materials.end()){
		mat = contains->second;
	} 
	return mat;
}



MtlLoader::MtlLoader(std::string mtlFile)
{
	/*
	Ka - r g b
	defines the ambient color of the material to be (r,g,b). The default is (0.2,0.2,0.2);
	Kd - r g b
	defines the diffuse color of the material to be (r,g,b). The default is (0.8,0.8,0.8);
	Ks - r g b
	defines the specular color of the material to be (r,g,b). This color shows up in highlights. The default is (1.0,1.0,1.0);
	d or Tr - alpha
	defines the transparency of the material to be alpha. The default is 1.0 (not transparent at all) Some formats use Tr instead of d;
	Ns - s
	defines the shininess of the material to be s. The default is 0.0;
	illum - n
	denotes the illumination model used by the material. illum = 1 indicates a flat material with no specular highlights, so the value of Ks is not used. illum = 2 denotes the presence of specular highlights, and so a specification for Ks is required.
	map_Kd - filename
	names a file containing a texture
	*/


	mtlFile = MAT_DIRECTORY + mtlFile;
	FILE *fp = nullptr;
	fopen_s( &fp, mtlFile.c_str(), "r" );
	char lineCSTR [1000];
	std::string line;
	int MAX_LINE_LENGTH = 1000;

	if (fp == nullptr){
		ConsolePrintf(RGBA::RED, "NO .mtl FILE FOUND : %s", mtlFile.c_str());
		return;
	}
	while (fgets( lineCSTR, MAX_LINE_LENGTH, fp ) != NULL)
	{
		line = "";
		line.append(lineCSTR);
		ProcessLine(line);
	}
	CreateMaterial();		//save the last material
}

Material * MtlLoader::CreateMaterial()
{
	if (m_name != ""){
		Material* mat = new Material(m_name);
		mat->SetShader("default_lit");
		mat->SetDiffuseTexture(m_diffuseTexture);
		mat->SetNormalTexture(m_normalTexture);
		mat->SetProperty("AMBIENT_COLOR", m_ambient.GetColorWithAlpha(unsigned char(m_alpha* 255.f))	);
		mat->SetProperty("DIFFUSE_COLOR", m_diffuse.GetColorWithAlpha(unsigned char(m_alpha* 255.f))	);
		mat->SetProperty("SPECULAR_COLOR", m_specular.GetColorWithAlpha(unsigned char(m_alpha* 255.f))	);

		m_loadedMaterials.push_back(m_name);
		return mat;
	}
	return nullptr;
}

RGBA MtlLoader::GetColor(std::string line)
{
	Strings splitLine; 
	Split(line, ' ', splitLine);
	RGBA color;
	float r = (char) atoi(splitLine[1].c_str());
	float g = (char) atoi(splitLine[2].c_str());
	float b = (char) atoi(splitLine[3].c_str());
	color.SetAsFloats(r,g,b,1.f);
	return color;
}

void MtlLoader::ProcessLine(std::string line)
{
	Strings splitLine; 
	Split(line, ' ', splitLine);
	if (splitLine[0] == "Ka"){
		m_ambient = GetColor(line);
	} else if (splitLine[0] == "Kd"){
		m_diffuse = GetColor(line);
	} else if (splitLine[0] == "Ks"){
		m_specular = GetColor(line);
	} else if (splitLine[0] == "d" || splitLine[0] == "Tr"){
		m_alpha = (float) atof(splitLine[1].c_str());
	} else if (splitLine[0] == "map_Kd"){
		m_diffuseTexture = splitLine[1];
		Strip(m_diffuseTexture, '\n');
	} else if (splitLine[0] == "newmtl"){
		CreateMaterial();		//Make a material w/ current values and store it in static library
		Reset();
		m_name = splitLine[1];	//get the name of the next material
		
	}
}

void MtlLoader::Reset()
{
	m_name			 = "";
	m_ambient		 = RGBA::WHITE;
	m_diffuse		 = RGBA::WHITE;
	m_specular		 = RGBA::WHITE;
	m_alpha			 = 1.f;
	m_shininess		 = 1.f;
	m_diffuseTexture = "white";
	m_normalTexture	 = "flat";
}
