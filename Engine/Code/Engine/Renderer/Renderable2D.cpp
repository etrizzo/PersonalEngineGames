#include "Renderable2D.hpp"
#pragma once
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/ObjLoader.hpp"

Renderable2D::Renderable2D()
{
	m_mesh = new Mesh();
	m_mb = new MeshBuilder();
	m_materials.push_back(Material::GetMaterial("default"));
	//m_materials[0]->SetShader("default_lit");
}

Renderable2D::Renderable2D(eRenderablePrimatives primType, float size)
{
	m_mb = new MeshBuilder();
	m_mb->Begin(PRIMITIVE_TRIANGLES, true);
	switch(primType){
	case RENDERABLE_PLANE:
		m_mb->AppendPlane(Vector3::ZERO, Vector3::UP, Vector3::RIGHT, Vector2::ONE * size, RGBA::WHITE, Vector2::ZERO, Vector2::ONE);
		break;
	//case RENDERABLE_CIRCLE:
	//	mb.AppendSphere(Vector3::ZERO, size, 10.f, 10.f, RGBA::WHITE);
	//	break;
	}
	m_mb->End();
	m_mesh = m_mb->CreateMesh(VERTEX_TYPE_3DPCU);

	m_materials.push_back(Material::GetMaterial("default_lit"));
}

Renderable2D::~Renderable2D()
{
	delete m_mesh;
	delete m_mb;
}

//Renderable2D::Renderable2D(AABB2 quad)
//{
//	MeshBuilder mb;
//	mb.Begin(PRIMITIVE_TRIANGLES, true);
//}


Material * Renderable2D::GetEditableMaterial(int index)
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

void Renderable2D::SetMaterial(int index, std::string diffusePath, std::string normalPath)
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

void Renderable2D::SetMaterial(Material * mat, int index)
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

void Renderable2D::SetMesh(Mesh * mesh)
{
	m_mesh = mesh;
}

void Renderable2D::SetMesh(SubMesh* smesh, int idx)
{
	m_mesh->SetSubMesh(smesh, idx);
}

void Renderable2D::SetMesh(const AABB2& drawingBox, const AABB2& currentUVS, const RGBA& color)
{
	delete m_mesh;
	m_mb->Clear();
	m_mb->Begin(PRIMITIVE_TRIANGLES, true);
	m_mb->AppendPlane2D(drawingBox, color, currentUVS);
	m_mb->End();
	m_mesh = m_mb->CreateMesh(VERTEX_TYPE_3DPCU);
}

void Renderable2D::SetSubMesh(const AABB2 & drawingBox, const AABB2 & currentUVs, const RGBA & color, const int & meshIndex)
{
	//delete m_mesh;
	
	m_mb->Clear();
	m_mb->Begin(PRIMITIVE_TRIANGLES, true);
	m_mb->AppendPlane2D(drawingBox, color, currentUVs);
	m_mb->End();
	m_mesh->SetSubMesh(m_mb->CreateSubMesh(VERTEX_TYPE_3DPCU), meshIndex);
}

void Renderable2D::SetDiffuseTexture( Texture * tex, int index)
{
	GetEditableMaterial(index)->SetDiffuseTexture(tex);
}

void Renderable2D::SetNormalTexture( Texture * tex, int index)
{
	GetEditableMaterial(index)->SetNormalTexture(tex);
}

void Renderable2D::AddDiffuseTexture(Texture * tex, int index)
{
	if ((int) m_materials.size() <= index){
		SetMaterial(m_materials[0], index);
	}
	SetDiffuseTexture(tex, index);
}

void Renderable2D::SetShader(std::string shaderName, int index)
{
	GetEditableMaterial(index)->SetShader(shaderName);
}

void Renderable2D::SetShader(Shader * shader, int index)
{
	GetEditableMaterial(index)->SetShader(shader);
}

void Renderable2D::SetPosition(Vector2 pos)
{
	m_transform.SetLocalPosition(pos);
}

void Renderable2D::CreateMeshFromOBJ(std::string objFile)
{
	m_mesh = Mesh::GetMesh(objFile);
	Strings mats;
	if (m_mesh == nullptr){
		ObjLoader obj = ObjLoader(objFile);
		m_mesh = obj.CreateMesh();
		mats = obj.GetMaterials();
	}
	SetMaterials(mats);

}

void Renderable2D::SetMaterials(Strings matNames)
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
		m_materials.push_back(Material::GetMaterial("default_lit"));
	}
}

bool Renderable2D::UsesLights() const
{
	return m_usesLights;
}

int Renderable2D::GetZOrder() const
{
	return m_zOrder;
}

void Renderable2D::Clear()
{
	m_mesh->ClearSubMeshes();
	m_materials.clear();
	m_materials.push_back(Material::GetMaterial("default"));
}
