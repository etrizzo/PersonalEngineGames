#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"

struct DrawInstruction{
	eDrawPrimitiveType m_primitive;
	unsigned int m_start_index;		//where in the vertex buffer do you start?
	unsigned int m_elem_count;		//glDrawArrays should be numVertices, glDrawElement should be numIndices

	bool m_useIndices;
};

class MeshBuilder;

class SubMesh
{
public:		//mostly need geometric data, plus a 'material' and a draw instruction (to determine what primitive)
			// vertex buffer
	SubMesh(VertexLayout layout = Vertex3D_LIT::LAYOUT);

	//really just a counterpart to MeshBuilder::CreateMesh() in case you wanted to build from the other direction
	void FromBuilder(MeshBuilder* mb, eVertexType type = VERTEX_TYPE_LIT);

	void SetVertices(size_t const numVerts, void const * data);
	void SetIndices(size_t const numIndices, void const * data);

	void SetLayout(VertexLayout layout);
	VertexBuffer m_vbo;
	VertexLayout m_layout;
	// index buffer
	IndexBuffer m_ibo;
	// shader
	//draw instruction
	DrawInstruction m_instruction;
	//Transform m_transform;

	bool UsesIndices() const { return m_instruction.m_useIndices; };
	eDrawPrimitiveType GetPrimitiveType() const { return m_instruction.m_primitive; };
	unsigned int GetStartIndex() const { return m_instruction.m_start_index; };
	unsigned int GetElementCount() const { return m_instruction.m_elem_count; };
	unsigned int GetAttributeCount() const { return m_layout.GetAttributeCount(); };
	unsigned int GetVertexStride() const { return m_layout.m_stride; };

	//Matrix44 GetModelMatrix() const { return m_transform.GetLocalMatrix();}

	//void Translate(Vector3 translation) { m_transform.TranslateLocal(translation); }
	//void Rotate(Vector3 rotation) {m_transform.RotateByEuler(rotation); }
	//void SetPosition(Vector3 pos) { m_transform.SetLocalPosition(pos); }
	//void SetTransform(Transform t) { m_transform = t; }


	

private:
	//todo: t e m p l a t e s
	void FromBuilderPCU(MeshBuilder* mb);
	void FromBuilderLit(MeshBuilder* mb);
};

class Mesh{
public:
	Mesh(eVertexType layout = VERTEX_TYPE_LIT);
	void AddSubMesh(SubMesh* smesh);
	void SetSubMesh(SubMesh* smesh, int idx = 0);
	std::vector<SubMesh*> m_subMeshes;

	static std::map <std::string, Mesh*>	s_meshes;
	static Mesh* CreateOrGetMesh(std::string name);
	static Mesh* GetMesh(std::string name);

	eVertexType m_defaultLayout;
};



