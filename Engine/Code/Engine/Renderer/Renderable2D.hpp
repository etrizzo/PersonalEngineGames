#pragma once
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Math/Transform2D.hpp"

class Shader;

class Renderable2D{
public:
	Renderable2D();
	Renderable2D(eRenderablePrimatives primType, float size = 1.f);
	~Renderable2D();
	//Renderable2D(AABB2 quad);
	//Renderable2D(std::string objFile, std::string matFile = "default");

public:
	Mesh* m_mesh;
	MeshBuilder* m_mb;
	//Material* m_sharedMaterial;		//whatever material i'm using
	//Material* m_instanceMaterial;	// == nullptr;
	std::vector<Material*> m_materials;
	std::vector<Material*> m_instancedMaterials;
	Transform2D m_transform;
	bool m_usesLights = false;
	int m_zOrder = 0;

	Material* GetEditableMaterial(int index = 0);

	void SetMaterial(int index, std::string diffusePath, std::string normalPath = "NONE");
	void SetMaterial(Material* mat, int index = 0);
	void SetMesh(Mesh* mesh);
	void SetMesh(SubMesh* smesh, int idx = 0);

	void SetMesh(const AABB2& drawingBox, const AABB2& currentUVS, const RGBA& color = RGBA::WHITE);
	void SetSubMesh(const AABB2& drawingBox, const AABB2& currentUVs, const RGBA& color, const int& meshIndex);

	//void SetDiffuseTexture(Texture* tex, int index = 0);
	//void SetNormalTexture(Texture* tex, int index = 0);
	void SetDiffuseTexture( Texture* tex, int index = 0);		//should use the const ones but keep old ones for backwards compatibility
	void SetNormalTexture( Texture* tex, int index = 0);
	void AddDiffuseTexture(Texture* tex, int index = 0);

	void SetShader(std::string shaderName, int index = 0);
	void SetShader(Shader* shader, int index = 0);

	void SetPosition(Vector2 pos);
	inline Vector2 GetPosition() { return m_transform.GetWorldPosition(); };

	//creates mesh from OBJ file
	void CreateMeshFromOBJ(std::string objFile);
	//loads materials from OBJ file
	void SetMaterials(Strings matNames);

	bool UsesLights() const;
	int GetZOrder() const;

	//clears meshes and materials
	void Clear();

};