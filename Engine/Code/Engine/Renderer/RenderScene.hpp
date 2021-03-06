#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Skybox.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"

////splitscreen particle emitters - need to be able to pre-render per camera - i.e. billboarding planes to the camera for sprite-based particles
//typedef void (*CameraPreRenderCB)( Camera* cam );		//std::function??


class RenderScene{
public:
	 RenderScene(Renderer* r);
	 ~RenderScene();
	void AddRenderable(Renderable *r);
	void AddLight(Light * l);
	void AddCamera(Camera* c);	//optional?
	void AddParticleSystem(ParticleSystem* ps);

	void RemoveCamera(Camera* c);
	void RemoveRenderable(Renderable* r);
	void RemoveLight(Light* l);

	void SortCameras(){};		//todo

	Light* AddNewLight(std::string type, Vector3 pos, RGBA color = RGBA::WHITE);
	Light* AddNewLight(std::string type, Transform t, RGBA color = RGBA::WHITE);
	Light* AddNewPointLight(Vector3 pos, RGBA color);
	Light* AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle = 20.f, float outerAngle = 25.f);
	Light* AddNewSpotLight(Transform t, RGBA color, float innerAngle = 20.f, float outerAngle = 25.f);
	Light* AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation = Vector3::ZERO);
	void RemoveLight(int idx = 0);
	void SetLightPosition(Vector3 newPos, unsigned int idx = 0);
	void SetLightColor(RGBA newColor, unsigned int idx = 0);
	void SetLightColor(Vector4 newColor, unsigned int idx = 0);

	void SetLightAttenuation(int lightIndex, Vector3 att);

	void SetShadowCameraPosition(Vector3 worldPos);
	void SetShadowCameraTransform(const Transform& t);


public:
	std::vector<Renderable*> m_renderables;
	std::vector<Light*> m_lights;		// we only support 8 per draw call, so we have to figure out which 8 we want to keep
	std::vector<Camera*> m_cameras; 		//or could just have one camera
	std::vector<ParticleSystem*> m_particleSystems;

	Camera* m_shadowCamera;
	Texture* m_shadowDepthTarget;
	Texture* m_shadowColorTarget;


	// //for particle emitters splitscreen
	// std::vector<CameraPreRenderCB> m_preRenders;
};