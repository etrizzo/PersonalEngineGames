#include "ObjLoader.hpp"
#include "Game/EngineBuildPreferences.hpp"

ObjLoader::ObjLoader(std::string fileName)
{
	m_mesh = new Mesh(VERTEX_TYPE_LIT);
	m_name = fileName;
	fileName = OBJ_DIRECTORY + fileName;
	FILE *fp = nullptr;
	fopen_s( &fp, fileName.c_str(), "r" );
	char lineCSTR [1000];
	std::string line;
	int MAX_LINE_LENGTH = 1000;

	ASSERT_OR_DIE(fp != nullptr, "NO .OBJ FILE FOUND");
	mb = new MeshBuilder();
	mb->Begin( PRIMITIVE_TRIANGLES, true);
	while (fgets( lineCSTR, MAX_LINE_LENGTH, fp ) != NULL)
	{
		line = "";
		line.append(lineCSTR);
		ProcessLine(line);
	}
	AddSubMesh("");
	
}

SubMesh * ObjLoader::CreateSubMesh()
{
	mb->End();
	SubMesh* mesh = mb->CreateSubMesh(m_mesh->m_defaultLayout);
	
	mb->Begin( PRIMITIVE_TRIANGLES, true);
	//SubMesh::s_meshes.insert(std::pair<std::string, SubMesh*>(m_name, mesh));
	return mesh;
}

Mesh * ObjLoader::CreateMesh()
{
	Mesh::s_meshes.insert(std::pair<std::string, Mesh*>(m_name, m_mesh));
	return m_mesh;
}

Strings ObjLoader::GetMaterials()
{
	return m_materials;
}

void ObjLoader::AddPosition(std::string line)
{
	Strings splitLine; 
	Split(line, ' ', splitLine);
	Vector3 pos;
#if defined (USE_X_FORWARD_Z_UP)
	pos.y = -1.f * (float) atof(splitLine[1].c_str());
	pos.z = (float) atof(splitLine[2].c_str());
	pos.x = (float) atof(splitLine[3].c_str());
#else
	pos.x = (float) atof(splitLine[1].c_str());
	pos.y = (float) atof(splitLine[2].c_str());
	pos.z = (float) atof(splitLine[3].c_str());
#endif
	m_positions.push_back(pos);

}

void ObjLoader::AddNormal(std::string line)
{
	Strings splitLine; 
	Split(line, ' ', splitLine);
	Vector3 normal;
	normal.x = (float) atof(splitLine[1].c_str());
	normal.y = (float) atof(splitLine[2].c_str());
	normal.z = (float) atof(splitLine[3].c_str());
	m_normals.push_back(normal);
}

void ObjLoader::AddUV(std::string line)
{
	Strings splitLine; 
	Split(line, ' ', splitLine);
	Vector2 uv;
	uv.x = (float) atof(splitLine[1].c_str());
	uv.y = (float) atof(splitLine[2].c_str());
	m_uvs.push_back(uv);
}

void ObjLoader::AddFace(std::string line)
{
	Strings splitLine; 
	Split(line, ' ', splitLine);
	int startIdx = -1;
	int numVerts = 0;
	for(int vertIdx = 1; vertIdx < (int) splitLine.size(); vertIdx++){
		//read the vertex and add it!
		//split the vertex info into it's own dang thing
		Strings vertInfo;
		Split(splitLine[vertIdx], '/', vertInfo);

		if (vertInfo.size() >=3){
			int posIndex	= atoi(vertInfo[0].c_str()) - 1;
			int normalIndex = atoi(vertInfo[2].c_str()) - 1;
			int uvIndex		= atoi(vertInfo[1].c_str()) - 1;

			//push the vertex in the meshbuilder
			Vector3 actualNorm = m_normals[normalIndex];
			actualNorm.x = actualNorm.x * -1.f;
			mb->SetNormal(actualNorm);
			mb->SetUV(m_uvs[uvIndex]);
			TODO("Replace with mikkt");
			mb->SetTangent(Vector3::X_AXIS);		
			Vector3 actualPos =m_positions[posIndex];
			actualPos.x = actualPos.x * -1.f;
			int idx = mb->PushVertex(actualPos);

			if (startIdx == -1){		//save the first index on the face
				startIdx = idx;
			}
			numVerts++;
		} else if (vertInfo.size() == 2){
			int posIndex	= atoi(vertInfo[0].c_str()) - 1;
			int uvIndex		= atoi(vertInfo[1].c_str()) - 1;

			//push the vertex in the meshbuilder
			Vector3 actualNorm = Vector3(.5f,.5f,1.f);
			mb->SetNormal(actualNorm);
			mb->SetUV(m_uvs[uvIndex]);
			TODO("Replace with mikkt");
			mb->SetTangent(Vector3::X_AXIS);		
			Vector3 actualPos =m_positions[posIndex];
			actualPos.x = actualPos.x * -1.f;
			int idx = mb->PushVertex(actualPos);

			if (startIdx == -1){		//save the first index on the face
				startIdx = idx;
			}
			numVerts++;
		}
	}
	//add the face indices
	if (numVerts == 3){
		//mb->AddTriIndices(startIdx + 2, startIdx + 1, startIdx + 0);
		mb->AddTriIndices(startIdx + 0, startIdx + 1, startIdx + 2);
	} else if (numVerts == 4){
		mb->AddTriIndices(startIdx + 0, startIdx + 2, startIdx + 3);
		mb->AddTriIndices(startIdx + 1, startIdx + 2, startIdx + 0);	//this is different??
		//mb->AddTriIndices(startIdx + 3, startIdx + 2, startIdx + 0);
		//mb->AddTriIndices(startIdx + 0, startIdx + 2, startIdx + 1);		
	}
}

void ObjLoader::AddSubMesh(std::string line)
{
	if (m_lastMatName != ""){
		m_mesh->AddSubMesh(CreateSubMesh());
		m_materials.push_back(m_lastMatName);
	}
	Strings splitLine; 
	Split(line, ' ', splitLine);
	if (splitLine.size() > 1){
		m_lastMatName = splitLine[1];
	}
	
}

void ObjLoader::ProcessLine(std::string line)
{
	Strings splitLine; 
	Split(line, ' ', splitLine);
	if (splitLine[0] == "v"){
		AddPosition(line);
	} else if (splitLine[0] == "vn"){
		AddNormal(line);
	} else if (splitLine[0] == "vt"){
		AddUV(line);
	} else if (splitLine[0] == "f"){
		AddFace(line);
	} else if (splitLine[0] == "usemtl"){
		AddSubMesh(line);
	}
}
