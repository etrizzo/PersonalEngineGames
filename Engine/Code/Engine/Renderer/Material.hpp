#pragma once
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/MaterialProperty.hpp"

const int DIFFUSE_BIND_POINT = 0;
const int NORMAL_BIND_POINT = 1;
const int DEPTH_BIND_POINT = 6;
const int COLOR_BIND_POINT = 7;

const float DEFAULT_SPEC_AMOUNT = .5f;
const float DEFAULT_SPEC_POWER = 3.f;

const std::string MAT_DIRECTORY = "Data/Models/";


//combination of a shader, textures, and material properties bound in the shader program
class Material{
public:
	Material();
	Material(tinyxml2::XMLElement* matElement, Renderer* renderer);
	Material(std::string name);
	Material(std::string name, std::string shaderPath);

	Material* Clone();

	//Texture Setting!!!!!!!!
	//also adds a sampler w/ the same bindpoint
	void SetTexture(int bindPoint, const Texture* tex);
	void SetDiffuseTexture(const Texture* tex);	
	void SetDiffuseTexture(std::string texPath); //helper
	void SetNormalTexture(const Texture* tex);
	void SetNormalTexture(std::string texPath);	//helper


	//Shader setting
	void SetShader(std::string shaderName);
	void SetShader(Shader* shader);

	//Property Setting
	void RemoveProperty(const char* name); //removes existing property so we don't duplicate
	void SetProperty(const char* name, float value);
	void SetProperty(const char* name, RGBA value);
	void SetProperty(const char* name, Vector2 value);



	//getters
	inline Shader* GetShader() const { return m_shader; }
	inline GLuint GetProgramhandle() const { return m_shader->GetProgramHandle(); }
	inline bool UsesLights() const { return m_lit; }


	static Strings LoadMaterials(std::string matFile);
	static std::map<std::string, Material*> s_materials;
	static Material* GetMaterial(std::string name);

private:
	int getTextureIndexByType(const std::string& type);
	Texture* getTexture(const std::string& path, Renderer* renderer);

public:
	std::string m_name;
	Shader* m_shader;

	std::vector<const Texture*> m_textures;
	std::vector<Sampler*> m_samplers;

	std::vector<MaterialProperty*> m_properties;

	bool m_lit = true;
};



class MtlLoader{
public:
	MtlLoader(std::string mtlFile);

	Material* CreateMaterial();
	Strings m_loadedMaterials;

private:
	RGBA GetColor (std::string line);
	void ProcessLine(std::string line);
	void Reset();

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

	std::string m_name			 = "";
	RGBA		m_ambient		 = RGBA::WHITE;
	RGBA		m_diffuse		 = RGBA::WHITE;
	RGBA		m_specular		 = RGBA::WHITE;
	float		m_alpha			 = 1.f;
	float		m_shininess		 = 1.f;
	std::string m_diffuseTexture = "white";
	std::string m_normalTexture	 = "flat";


};