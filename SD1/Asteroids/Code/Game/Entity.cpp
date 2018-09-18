#include "Entity.hpp"

Entity::Entity(Vector2 initialPos,  Vector2 initialVelocity, Disc2 outerRadius, Disc2 innerRadius, float initialRotation, float rotateSpeed, int sides, RGBA color)
{

	m_position = initialPos;
	m_velocity = initialVelocity;
	m_drawingRadius = outerRadius;
	m_physicsRadius = innerRadius;
	m_rotationDegrees = initialRotation;
	m_spinDegreesPerSecond = rotateSpeed;
	m_ageInSeconds = 0.f;
	m_color = color;

	m_sides = sides;
	
	
}

void Entity::Update(float deltaSeconds)
{
	m_rotationDegrees+= (m_spinDegreesPerSecond*deltaSeconds);
	m_position+=(m_velocity*deltaSeconds);
	m_drawingRadius.center=m_position;
	m_physicsRadius.center=m_position;
	m_ageInSeconds+=deltaSeconds;
	

	CheckScreen();
}

void Entity::Render(bool devMode)
{
 	g_theRenderer->PushAndTransform2(m_position,m_rotationDegrees, m_drawingRadius.radius);
	g_theRenderer->DrawShapeFromVertices2(m_vertices, m_sides, m_color);
	g_theRenderer->Pop2();



	if (devMode){
		RenderDevMode();
	}
}

void Entity::RenderDevMode()
{
	//draw cosmetic radius
	g_theRenderer->PushAndTransform2(m_drawingRadius.center,0.f, m_drawingRadius.radius);
	RenderDevDisc(RGBA(255,0,255));
	g_theRenderer->Pop2();

	//draw physics radius
	g_theRenderer->PushAndTransform2(m_physicsRadius.center, 0.f, m_physicsRadius.radius);
	RenderDevDisc(RGBA(0,255,255));
	g_theRenderer->Pop2();

	//draw velocity vector
	g_theRenderer->PushAndTransform2(m_position, m_velocity.GetOrientationDegrees(), m_velocity.GetLength());
	Vector2 start = Vector2(0.f, 0.f);
	Vector2 end = Vector2(1.f, 0.f);
	g_theRenderer->DrawLine2D(start,end, RGBA(255,255,0), RGBA(255,255,0));
	g_theRenderer->Pop2();

}


void Entity::RenderDevDisc(RGBA color)
{
	g_theRenderer->DrawRegularPolygon2(40,Vector2(0,0), 1.f,0.f,color,true);
}


void Entity::CheckScreen()
{
	if (m_position.x > SCREEN_SIZE + m_drawingRadius.radius){
		m_position.x = -1 * m_drawingRadius.radius;
	} else if (m_position.x < (-1 * m_drawingRadius.radius)){
		m_position.x = SCREEN_SIZE + m_drawingRadius.radius;
	}

	if (m_position.y > SCREEN_SIZE + m_drawingRadius.radius){
		m_position.y = -1 * m_drawingRadius.radius;
	} else if (m_position.y < (-1 * m_drawingRadius.radius)){
		m_position.y = SCREEN_SIZE + m_drawingRadius.radius;
	}
}
