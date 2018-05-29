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

	void AddRenderable(Renderable *r);
	void AddLight(Light * l);
	void AddCamera(Camera* c);	//optional?
	void AddSkybox(Skybox* s);
	void AddParticleSystem(ParticleSystem* ps);

	void RemoveCamera(Camera* c);
	void RemoveRenderable(Renderable* r);
	void RemoveLight(Light* l);
	void RemoveSkybox();

	void SortCameras(){};		//todo

	void AddNewLight(std::string type, Vector3 pos, RGBA color = RGBA::WHITE);
	void AddNewLight(std::string type, Transform t, RGBA color = RGBA::WHITE);
	Light* AddNewPointLight(Vector3 pos, RGBA color);
	void AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle = 20.f, float outerAngle = 25.f);
	void AddNewSpotLight(Transform t, RGBA color, float innerAngle = 20.f, float outerAngle = 25.f);
	void AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation = Vector3::ZERO);
	void RemoveLight(int idx = 0);
	void SetLightPosition(Vector3 newPos, unsigned int idx = 0);
	void SetLightColor(RGBA newColor, unsigned int idx = 0);
	void SetLightColor(Vector4 newColor, unsigned int idx = 0);

	void SetLightAttenuation(int lightIndex, Vector3 att);


public:
	std::vector<Renderable*> m_renderables;
	std::vector<Light*> m_lights;		// we only support 8 per draw call, so we have to figure out which 8 we want to keep
	std::vector<Camera*> m_cameras; 		//or could just have one camera
	std::vector<ParticleSystem*> m_particleSystems;

	Skybox* m_skybox = nullptr;		//optional
	// //for particle emitters splitscreen
	// std::vector<CameraPreRenderCB> m_preRenders;
};