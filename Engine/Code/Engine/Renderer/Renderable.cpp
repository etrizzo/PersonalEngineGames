#include "Renderable.hpp"
#pragma once
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/ObjLoader.hpp"

Renderable::Renderable()
{
	m_mesh = new Mesh();
	m_materials.push_back(Material::GetMaterial("default_lit"));
	//m_materials[0]->SetShader("default_lit");
}

Renderable::Renderable(eRenderablePrimatives primType, float size)
{
	MeshBuilder mb;
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	switch(primType){
	case RENDERABLE_CUBE:
		mb.AppendCube(Vector3::ZERO, Vector3::ONE * size, RGBA::WHITE);
		break;
	case RENDERABLE_PLANE:
		mb.AppendPlane(Vector3::ZERO, Vector3::Y_AXIS, Vector3::X_AXIS, Vector2::ONE * size, RGBA::WHITE, Vector2::ZERO, Vector2::ONE);
		break;
	case RENDERABLE_SPHERE:
		mb.AppendSphere(Vector3::ZERO, size, 10, 10, RGBA::WHITE);
		break;
	}
	mb.End();
	m_mesh = mb.CreateMesh(VERTEX_TYPE_LIT);

	m_materials.push_back(Material::GetMaterial("default_lit"));
}

Renderable::Renderable(std::string objFile, std::string matFile)
{
	m_mesh = new Mesh();
	Material* mat = Material::GetMaterial(matFile);		//try to load existing material
	if (mat == nullptr){
		Material::LoadMaterials(matFile);		//try to load materials as a .mtl file
		//at this point materials specified in the .obj will load correctly
	} else {
		m_materials.push_back(mat);
	}


	CreateMeshFromOBJ(objFile, mat != nullptr); 
}

Material * Renderable::GetEditableMaterial(int index)
{
	if (index < (int) m_materials.size()){
		if (index >= (int) m_instancedMaterials.size()){
			m_instancedMaterials.resize(index + 1);
			m_instancedMaterials[index] = m_materials[index]->Clone();
		}
		return m_instancedMaterials[index];
	} else {
		if (m_materials.size() == 0){
			return nullptr;		//if there are no materials, return nullptr
		} else {
			return m_instancedMaterials[m_materials.size() - 1];		//clamp to last material in array
		}
	}
}

void Renderable::SetMaterial(int index, std::string diffusePath, std::string normalPath)
{
	if ((int) m_materials.size() <= index){
		m_materials.resize(index + 1);
		m_materials[index] = m_materials[0]->Clone();
	}
	m_materials[index]->SetDiffuseTexture(diffusePath);
	m_materials[index]->SetNormalTexture(normalPath);

	//reset the instanced material, if it already exists
	if ((int) m_instancedMaterials.size() > index){
		m_instancedMaterials[index] = m_materials[index]->Clone();
	}
}

void Renderable::SetMaterial(Material * mat, int index)
{
	if ((int) m_materials.size() <= index){
		m_materials.resize(index + 1);
	}
	m_materials[index] = mat;

	//reset the instanced material, if it already exists
	if ((int) m_instancedMaterials.size() > index){
		m_instancedMaterials[index] = m_materials[index]->Clone();
	}
}

void Renderable::SetMesh(Mesh * mesh)
{
	m_mesh = mesh;
}

void Renderable::SetMesh(SubMesh* smesh, int idx)
{
	m_mesh->SetSubMesh(smesh, idx);
}

void Renderable::SetDiffuseTexture( Texture * tex, int index)
{
	GetEditableMaterial(index)->SetDiffuseTexture(tex);
}

void Renderable::SetNormalTexture( Texture * tex, int index)
{
	GetEditableMaterial(index)->SetNormalTexture(tex);
}

void Renderable::SetShader(std::string shaderName, int index)
{
	GetEditableMaterial(index)->SetShader(shaderName);
}

void Renderable::SetShader(Shader * shader, int index)
{
	GetEditableMaterial(index)->SetShader(shader);
}

void Renderable::SetPosition(Vector3 pos)
{
	m_transform.SetLocalPosition(pos);
}

void Renderable::CreateMeshFromOBJ(std::string objFile, bool matFound)
{
	m_mesh = Mesh::GetMesh(objFile);
	Strings mats;
	if (m_mesh == nullptr){
		ObjLoader obj = ObjLoader(objFile);
		m_mesh = obj.CreateMesh();
		if (!matFound){
			mats = obj.GetMaterials();
		}
	}
	SetMaterials(mats);
	
}

void Renderable::SetMaterials(Strings matNames)
{
	if (matNames.size() > 0){
		m_materials.clear();
		for (std::string matString : matNames){
			Material* mat = Material::GetMaterial(matString);
			if (mat == nullptr){
				ConsolePrintf(RGBA::RED, "Material not found: %s. default_lit material used for mesh.", matString.c_str());
				mat = Material::GetMaterial("default_lit");
			}
			m_materials.push_back(mat);
		}
	} else {
		if (m_materials.size() == 0){
			m_materials.push_back(Material::GetMaterial("default_lit"));
		}
	}
}
