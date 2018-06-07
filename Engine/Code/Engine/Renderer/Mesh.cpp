#include "Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/ObjLoader.hpp"


std::map<std::string, Mesh*>	Mesh::s_meshes;

SubMesh::SubMesh(VertexLayout layout)
{
	m_layout = layout;
	m_vbo = VertexBuffer(layout);
	m_ibo = IndexBuffer();
	m_instruction = DrawInstruction();
	//m_transform = Transform();
}


void SubMesh::FromBuilder(MeshBuilder * mb, eVertexType type)
{
	switch(type){
	case VERTEX_TYPE_3DPCU:
		FromBuilderPCU(mb);
		break;
	case VERTEX_TYPE_LIT:
		FromBuilderLit(mb);
		break;
	}
}

void SubMesh::SetVertices(size_t const numVerts, void const * data)
{
	m_vbo.SetVertices(numVerts, data);
}

void SubMesh::SetIndices(size_t const numIndices, void const * data)
{
	m_ibo.SetIndices(numIndices, data);
}

void SubMesh::SetLayout(VertexLayout layout)
{
	m_layout = layout;
	m_vbo.SetStride(layout.m_stride);
}



void SubMesh::FromBuilderPCU(MeshBuilder * mb)
{
	m_layout = GetLayoutForType(VERTEX_TYPE_3DPCU);
	
	size_t vsize = m_layout.m_stride * mb->m_vertices.size(); 
	Vertex3D_PCU *buffer = (Vertex3D_PCU*) malloc( vsize ); 
	for (unsigned int i = 0; i < mb->m_vertices.size(); ++i) {
		// copy each vertex
		buffer[i] = Vertex3D_PCU( mb->m_vertices[i] ); 
	}

	SetVertices(sizeof(Vertex3D_PCU) * mb->m_vertices.size(), buffer);		
	free( buffer ); 

	SetIndices(mb->m_indices.size(), mb->m_indices.data());		//sets indices in index buffer
	m_instruction = mb->m_drawInstruction;
}

void SubMesh::FromBuilderLit(MeshBuilder * mb)
{
	m_layout = GetLayoutForType(VERTEX_TYPE_LIT);

	size_t vsize = m_layout.m_stride * mb->m_vertices.size(); 
	Vertex3D_LIT *buffer = (Vertex3D_LIT*) malloc( vsize ); 
	for (unsigned int i = 0; i < mb->m_vertices.size(); ++i) {
		// copy each vertex
		buffer[i] = Vertex3D_LIT( mb->m_vertices[i] ); 
	}

	SetVertices(sizeof(Vertex3D_LIT) * mb->m_vertices.size(), buffer);		
	free( buffer ); 

	SetIndices(mb->m_indices.size(), mb->m_indices.data());		//sets indices in index buffer
	m_instruction = mb->m_drawInstruction;
}

Mesh::Mesh(eVertexType layout)
{
	m_defaultLayout = layout;
	m_subMeshes = std::vector<SubMesh*>();
}

void Mesh::AddSubMesh(SubMesh * smesh)
{
	m_subMeshes.push_back(smesh);
}

void Mesh::SetSubMesh(SubMesh * smesh, int idx)
{
	if (idx >= (int) m_subMeshes.size()){
		m_subMeshes.resize(idx+1);
	}
	if (m_subMeshes[idx] != nullptr){
		delete m_subMeshes[idx];
	}
	m_subMeshes[idx] = smesh;
}

Mesh * Mesh::CreateOrGetMesh(std::string name)
{
	auto containsMesh = s_meshes.find((std::string)name); 
	Mesh* mesh = nullptr;
	if (containsMesh != s_meshes.end()){
		mesh = containsMesh->second;
	} else {
		ObjLoader obj = ObjLoader(name);
		mesh = obj.CreateMesh();
		
	}
	return mesh;
}

Mesh * Mesh::GetMesh(std::string name)
{
	auto containsMesh = s_meshes.find((std::string)name); 
	Mesh* mesh = nullptr;
	if (containsMesh != s_meshes.end()){
		mesh = containsMesh->second;
	}
	return mesh;
}
