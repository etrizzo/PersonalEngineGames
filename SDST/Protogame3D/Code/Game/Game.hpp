#pragma once
#include "GameCommon.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"


class Camera;
class PerspectiveCamera;
class DebugRenderSystem;
class Entity;
class Light;
class SpotLight;
class ParticleSystem;

class Map;

enum eDebugShaders{
	SHADER_LIT,			//default
	SHADER_NORMAL,
	SHADER_TANGENT,
	SHADER_BITANGENT,
	SHADER_NORMALMAP,
	SHADER_WORLDNORMAL,
	SHADER_DIFFUSE,
	SHADER_SPECULAR,
	NUM_DEBUG_SHADERS
};


enum EntityTypes{
	NUM_ENTITIES
};

class Game{

public:
	~Game();											// destructor: do nothing (for speed)
	Game();											// default constructor: do nothing (for speed)

	bool m_isPaused;
	bool m_devMode;
	float m_gameTime;
	Map* m_currentMap;
	PerspectiveCamera* m_mainCamera;
	Camera* m_uiCamera;
	PerspectiveCamera* m_currentCamera;
	SpotLight* m_orbitLight;
	Light* m_cameraLight;
	std::vector<Light*> m_lights = std::vector<Light*>();
	Texture* m_tileTexture;
	Entity* m_thaShip;
	Entity* m_thaMiku;

	ForwardRenderPath* m_renderPath;
	RenderScene* m_scene;


	DebugRenderSystem* m_debugRenderSystem;
	void PostStartup();

	void Update();
	void Render();
	void HandleInput();

	float GetDeltaSeconds();

	AABB2 SetMainCamera();
	AABB2 SetUICamera();
	AABB2 GetUIBounds();
	Vector3 GetCurrentCameraUp();
	Vector3 GetCurrentCameraRight();
	void SetGameCamera(PerspectiveCamera* newCam = nullptr);


	void TogglePause();
	void ToggleDevMode();

	void DebugStartup();
	void DebugShutdown();
	void DebugClear();
	void DebugToggleRendering();

	void AddNewLight(std::string type, RGBA color = RGBA::WHITE);		//adds in front of camera
	void AddNewLight(std::string type, Vector3 pos, RGBA color = RGBA::WHITE);
	void AddNewPointLight(Vector3 pos, RGBA color);
	void AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle = 20.f, float outerAngle = 25.f);
	void AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation = Vector3::ZERO);
	void RemoveLight(int idx = 0);
	void SetLightPosition(Vector3 newPos, unsigned int idx = 0);
	void SetLightColor(RGBA newColor, unsigned int idx = 0);
	void SetLightColor(Vector4 newColor, unsigned int idx = 0);

	void SetLightAttenuation(int lightIndex, Vector3 att);


	unsigned int GetNumActiveLights() const { return m_numActiveLights ; }

private:
	void LoadTileDefinitions();
	void LoadMapDefinitions();

	void RenderGame();
	void RenderUI();

	float m_specAmount = .5f;
	float m_specFactor = 3.f;
	int m_numActiveLights = 0;

	eDebugShaders m_debugShader = SHADER_LIT;

	void UpdateShader(int direction);
	void SetShader();		//sets which shader to draw scene with
	std::string GetShaderName() const;

	//for objects drawn using drawmeshimmediate
	Material* m_couchMaterial;

	ParticleSystem* m_particleSystem;
};

extern Game* g_theGame;