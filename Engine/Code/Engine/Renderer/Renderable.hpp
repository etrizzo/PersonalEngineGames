#pragma once
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Mesh.hpp"


enum eRenderablePrimatives{
	RENDERABLE_SPHERE,
	RENDERABLE_CUBE,
	RENDERABLE_PLANE
};

class Shader;

class Renderable{
public:
	Renderable();
	Renderable(eRenderablePrimatives primType, float size = 1.f);
	Renderable(std::string objFile, std::string material = "default");

public:
	Mesh* m_mesh;
	//Material* m_sharedMaterial;		//whatever material i'm using
	//Material* m_instanceMaterial;	// == nullptr;
	std::vector<Material*> m_materials;
	std::vector<Material*> m_instancedMaterials;
	Transform m_transform;

	Material* GetEditableMaterial(int index = 0);

	void SetMaterial(int index, std::string diffusePath, std::string normalPath = "NONE");
	void SetMaterial(Material* mat, int index = 0);
	void SetMesh(Mesh* mesh);
	void SetMesh(SubMesh* smesh, int idx = 0);

	//void SetDiffuseTexture(Texture* tex, int index = 0);
	//void SetNormalTexture(Texture* tex, int index = 0);
	void SetDiffuseTexture( Texture* tex, int index = 0);		//should use the const ones but keep old ones for backwards compatibility
	void SetNormalTexture( Texture* tex, int index = 0);

	void SetShader(std::string shaderName, int index = 0);
	void SetShader(Shader* shader, int index = 0);

	void SetPosition(Vector3 pos);
	inline Vector3 GetPosition() { return m_transform.GetWorldPosition(); };

	//creates mesh from OBJ file
	void CreateMeshFromOBJ(std::string objFile, bool matFound);
	//loads materials from OBJ file
	void SetMaterials(Strings matNames);

};