#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

class Camera;

//just a container for everything
class Skybox {
public:
	Skybox(const char* fileName, const Vector3& right = Vector3::X_AXIS, const Vector3& up = Vector3::Y_AXIS, const Vector3& forward = Vector3::Z_AXIS);
	~Skybox(){};

	void Update();

	TextureCube m_texCube;
	Mesh* m_mesh;
	Transform m_transform; //need to match camera position, but not rotation
};