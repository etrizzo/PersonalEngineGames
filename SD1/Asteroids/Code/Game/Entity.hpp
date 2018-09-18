#pragma once
#include "Engine/Math/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/GameCommon.hpp"
//#include "Game/Game.hpp"


#define SCREEN_SIZE 1000.f
#define MAX_SIDES 200

class Entity{
public:
	~Entity() {}									
	Entity() {}										
	Entity(Vector2 initialPos,  Vector2 initialVelocity, Disc2 outerRadius, Disc2 innerRadius, float initialRotation, float rotateSpeed, int sides = 0, RGBA color = RGBA());

	Vector2 m_position;
	Vector2 m_velocity;
	float m_rotationDegrees;
	float m_spinDegreesPerSecond;
	Disc2 m_drawingRadius;
	Disc2 m_physicsRadius;
	float m_ageInSeconds;
	RGBA m_color;

	Vector2 m_vertices[MAX_SIDES];
	int m_sides;



	void Update(float deltaSeconds);

	void Render(bool devMode);
	void RenderDevMode();
	void RenderDevDisc(RGBA color);

	//void PushAndTransform();
	//void Draw();
	//void Pop();

	void CheckScreen();

};



//void DrawPolygon(int numSides, Vector2 pos, float radius, float rotation);