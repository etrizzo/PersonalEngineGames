#include "Game/Asteroid.hpp"




Asteroid::Asteroid(Vector2 initialPos, Vector2 initialVelocity, Disc2 outerRadius, Disc2 innerRadius, float initialRotation, float rotateSpeed, int numSides, int asteroidSize):Entity(initialPos, initialVelocity, outerRadius, innerRadius, initialRotation, rotateSpeed, numSides)
{
	m_size = asteroidSize;
	FunkUpVertices();
}

void Asteroid::Render(bool devMode){
	//draw the main asteroid
	g_theRenderer->PushAndTransform2(m_position,m_rotationDegrees, m_drawingRadius.radius);
	g_theRenderer->DrawShapeFromVertices2(m_vertices, m_sides, m_color);
	
	//draw the face
	RenderFace();

	g_theRenderer->Pop2();

	if (devMode){
		Entity::RenderDevMode();
	}

}

void Asteroid::DrawAsteroid()
{
	g_theRenderer->DrawShapeFromVertices2(m_vertices, m_sides);
}

void Asteroid::FunkUpVertices()
{
	float theta = 360.f /  static_cast<float>(m_sides);

	for (int i = 0; i < m_sides; i++){
		float degrees = (theta * i) + m_rotationDegrees;
		float funkyRadius = GetRandomFloatInRange(.8f,1.f);
		m_vertices[i] =  (Vector2(CosDegreesf(degrees), SinDegreesf(degrees)) * funkyRadius);
	}

}

void Asteroid::RenderFace(){
	RenderEye(Vector2(-.4f,.2f));
	RenderEye(Vector2(.4f,.2f));
	RenderExtra();
	RenderSmile();
}

void Asteroid::RenderEye(Vector2 pos)
{
	Vector2 eyeVertices[20];
	Vector2 sparkleVertices[20];
	Vector2 smallSparkleVertices[20];
	float sparkleScale = GetRandomFloatInRange(.3f, .35f);
	Vector2 sparkleCenter = Vector2(GetRandomFloatInRange(.1f,.2f),GetRandomFloatInRange(.1f,.2f));
	float smallSparkleScale = GetRandomFloatInRange(.1f, .15f);
	Vector2 smallSparkleCenter = Vector2(GetRandomFloatInRange(-.2f,-.3f),GetRandomFloatInRange(-.2f,-.3f));

	float theta = 360.f /  20.f;
	if (m_size == 2 || m_size == 1){

		for (int i = 0; i < 20; i++){
			float degrees = (theta * i);
			eyeVertices[i] =  Vector2(CosDegreesf(degrees), SinDegreesf(degrees)) ;
			sparkleVertices[i] =  ( (Vector2(CosDegreesf(degrees), SinDegreesf(degrees)) * sparkleScale) + sparkleCenter);
			smallSparkleVertices[i] =  ( (Vector2(CosDegreesf(degrees), SinDegreesf(degrees)) * smallSparkleScale) + smallSparkleCenter);
		}


	} else if (m_size == 0){
		for (int i = 0; i < 20; i++){
			float degrees = (theta * i);
			eyeVertices[i] =  Vector2(CosDegreesf(degrees), SinDegreesf(degrees)) *.3f ;
		}

	}

	g_theRenderer->PushAndTransform2(pos,0.f, .2f);
	g_theRenderer->DrawShapeFromVertices2(eyeVertices,20);
	if (m_size > 0){
		g_theRenderer->DrawShapeFromVertices2(sparkleVertices,20);
		g_theRenderer->DrawShapeFromVertices2(smallSparkleVertices,20);
	}
	g_theRenderer->Pop2();



}

void Asteroid::RenderSmile()
{
	if (m_size == 2){
		g_theRenderer->PushAndTransform2(Vector2(0.f,-.2f), 180.f, .4f);
		g_theRenderer->DrawCircleArc(.5f,20);
		g_theRenderer->Pop2();
	} else if (m_size == 1){
		g_theRenderer->PushAndTransform2(Vector2(0.f,-.3f), 0.f, .3f);
		g_theRenderer->DrawCircleArc(.5f,20);
		g_theRenderer->Pop2();
	} else if (m_size == 0){
		g_theRenderer->DrawLine2D(Vector2(-.4f,-.2f), Vector2(.4f,-.2f));

	}


}

void Asteroid::RenderExtra(){
	if (m_size == 2){		//blush
		RenderBlush();
	} else if (m_size == 1){
		RenderTears(Vector2(-.4f,.15f));
		RenderTears(Vector2(.4f,.15f));
	} else if (m_size == 0){
		RenderEyebrows();
	}
}


void Asteroid::RenderBlush()
{

	float startX = .35f;
	float startY = .075f;
	Vector2 offset = Vector2(.1f,0.f);
	RGBA color = RGBA(255,200,200);

	//left
	Vector2 top = Vector2(-startX,startY);
	Vector2 bottom = Vector2(-startX - .1f,-startY);

	for (int i = 0; i < 3; i++){
		g_theRenderer->DrawLine2D(top, bottom, color, color);
		top-=offset;
		bottom-=offset;
	}

	//right
	top = Vector2(startX,startY);
	bottom = Vector2(startX - .1f, -startY);
	for (int i = 0; i < 3; i++){
		g_theRenderer->DrawLine2D(top, bottom, color, color);
		top+=offset;
		bottom+=offset;
	}
}


void Asteroid::RenderTears(Vector2 pos)
{
	float xChange = .4f/20.f;
	Vector2 tearVertices[20];
	RGBA c = RGBA(128,220,255);
	for (int i =0;i < 20;i++){
		float toSin = RangeMapFloat(xChange*i,0.f,.4f,0.f,10.f);
		float sinVal = sinf(toSin+(m_ageInSeconds*50.f))*.05f;
		tearVertices[i] = Vector2(xChange*i, sinVal);

	}

	g_theRenderer->PushAndTransform2(pos + Vector2(.2f,0.f),-90.f,1.f);
	g_theRenderer->DrawShapeFromVertices2(tearVertices,20,c,19);
	g_theRenderer->Pop2();

	g_theRenderer->PushAndTransform2(pos + Vector2(-.2f,0.f),-90.f,1.f);
	g_theRenderer->DrawShapeFromVertices2(tearVertices,20,c,19);
	g_theRenderer->Pop2();




}

void Asteroid::RenderEyebrows()
{
	g_theRenderer->DrawLine2D(Vector2(-.4f,.6f),Vector2(0.f,.35f));

	g_theRenderer->DrawLine2D(Vector2(0.f,.35f), Vector2(.4f,.6f));


}









