#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

class Camera;

//just a container for everything
class Skybox {
public:
	Skybox(Camera* cam, const char* fileName);
	~Skybox(){};

	void Update(Camera* cam);

	TextureCube m_texCube;
	Mesh* m_mesh;
	Transform m_transform; //need to match camera position, but not rotation
};