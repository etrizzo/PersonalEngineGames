#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

class Camera;

//just a container for everything
class Skybox {
public:
	Skybox(const char* fileName);
	~Skybox(){};

	void Update();

	TextureCube m_texCube;
	Mesh* m_mesh;
	Transform m_transform; //need to match camera position, but not rotation
};