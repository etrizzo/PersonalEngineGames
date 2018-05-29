#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/MatrixStack.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"

class SubMesh;

class Camera
{
public:
	Camera();
	~Camera();

	void SetColorTarget( Texture *color_target );
	void SetDepthStencilTarget( Texture *depth_target );

	// model setters
	void LookAt( Vector3 position, Vector3 target, Vector3 up = Vector3::UP ); 

	// projection settings
	void SetProjection( Matrix44 proj ); 
	void SetProjectionOrtho( float size, float aspect, float nearVal, float farVal ); //how can you set screenMin to anything but 0,0??
	void SetProjectionOrtho( const Vector3 & nearBottomLeft, const Vector3 & farTopRight );
	void SetProjectionOrtho( const Vector2 & nearBottomLeft, const Vector2 & farTopRight );

	void Translate(Vector3 translation);
	void Rotate(Vector3 eulerChange);

	void RotateAroundX(float x);
	void RotateAroundY(float y);
	void RotateAroundZ(float z);


	

	bool Finalize();

	GLuint GetFramebufferHandle() { return m_output.GetHandle(); };

	Vector3 GetPosition();
	Vector3 GetForward() const;
	Vector3 GetRight() const;
	Vector3 GetUp() const;
	float GetOrthographicSize() const;

	Matrix44 GetViewMatrix();

	AABB2 GetBounds() const;

	SubMesh* GetDebugMesh();

public:
	// default all to identity
	//Matrix44 m_cameraMatrix;  // where is the camera?
	Transform m_transform;
	
	MatrixStack m_projMatrix;    // projection
	//Matrix44 m_modelMatrix;

	FrameBuffer m_output; 
	float m_orthographicSize;
	AABB2 m_bounds;

	SubMesh* m_debugmesh;

private:
	Matrix44 m_viewMatrix;    // inverse of camera (used for shader)
};