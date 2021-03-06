#pragma once
#include "Game/GameState.hpp"
#include "Game/GameCommon.hpp"

class Camera;
class DebugRenderSystem;
class Entity;
class Player;
class Light;
class SpotLight;
class ParticleSystem;
class SpriteRenderPath;
class RenderScene2D;
class Flower;

class GameState_Playing: public GameState{
public:
	GameState_Playing();

	Player* m_player;
	//Light* m_cameraLight;
	//std::vector<Light*> m_lights = std::vector<Light*>();

	SpriteRenderPath* m_renderPath;
	RenderScene2D* m_scene;

	void Update(float ds);
	void RenderGame();
	void RenderUI();
	void HandleInput();

	//void AddNewLight(std::string type, RGBA color = RGBA::WHITE);		//adds in front of camera
	//void AddNewLight(std::string type, Vector3 pos, RGBA color = RGBA::WHITE);
	//void AddNewPointLight(Vector3 pos, RGBA color);
	//void AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle = 20.f, float outerAngle = 25.f);
	//void AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation = Vector3::ZERO);
	//void RemoveLight(int idx = 0);
	//void SetLightPosition(Vector3 newPos, unsigned int idx = 0);
	//void SetLightColor(RGBA newColor, unsigned int idx = 0);
	//void SetLightColor(Vector4 newColor, unsigned int idx = 0);

	//void SetLightAttenuation(int lightIndex, Vector3 att);


	//unsigned int GetNumActiveLights() const;

	RenderScene2D* GetScene() { return m_scene; }


protected:
	Flower* m_flower = nullptr;
	float m_specAmount = .5f;
	float m_specFactor = 3.f;
	int m_numActiveLights = 0;

	eDebugShaders m_debugShader = SHADER_LIT;

	void UpdateShader(int direction);
	void SetShader();		//sets which shader to draw scene with
	std::string GetShaderName() const;

	//for objects drawn using drawmeshimmediate
	Material* m_couchMaterial;

};