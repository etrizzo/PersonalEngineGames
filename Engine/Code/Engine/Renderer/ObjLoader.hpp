#pragma once
#include "Engine/Renderer/MeshBuilder.hpp"

//just a helper for mesh builder
class ObjLoader{
public:
	ObjLoader(std::string fileName);

	SubMesh* CreateSubMesh();
	Mesh* CreateMesh();
	Strings GetMaterials();

	std::vector<Vector3> m_positions;
	std::vector<Vector3> m_normals;
	std::vector<Vector2> m_uvs;
	std::string m_name;

	MeshBuilder* mb;
	Mesh* m_mesh;

private:
	std::string m_lastMatName = "";
	Strings m_materials;
	
	void AddPosition(std::string line);
	void AddNormal(std::string line);
	void AddUV(std::string line);
	void AddFace(std::string line);
	void AddSubMesh(std::string line);
	void ProcessLine(std::string line);

};