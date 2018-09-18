#include "Star.hpp"

Star::Star(Vector2 pos, RGBA color, float size, float duration, float initialRotation, float rotateSpeed)
	:Entity(pos, Vector2(0.f,0.f), Disc2(pos, 0.f), Disc2(pos, 0.f), initialRotation, rotateSpeed, 10, color)
{
	m_duration = duration;
	m_size = size;
	m_sides = GetRandomIntInRange(1,6);
	m_spinDegreesPerSecond = rotateSpeed;
	m_transform = Transform();
	m_transform.SetLocalPosition(Vector3(pos));
	m_transform.SetRotationEuler(Vector3(0.f,0.f,m_rotationDegrees));
	m_transform.SetScale(Vector3(size, size, 1.f));
	FillVerts();
	
}

void Star::Update(float deltaSeconds)
{

	m_rotationDegrees+= (m_spinDegreesPerSecond*deltaSeconds);
	//m_drawingRadius.center=m_position;
	//m_physicsRadius.center=m_position;
	m_ageInSeconds+=deltaSeconds;

	if (m_ageInSeconds <= m_duration * .5f){
		//scale up current color
		m_currentColor = m_color.GetColorWithAlpha((int)RangeMapFloat(m_ageInSeconds, 0.f, m_duration *  .5f, 0, (float) m_color.a));
		m_size += deltaSeconds * .5f;
	} else {
		m_currentColor = m_color.GetColorWithAlpha(m_color.a - (int)RangeMapFloat(m_ageInSeconds, m_duration *  .5f, m_duration,  0, (float) m_color.a));
		m_size -= deltaSeconds * 1.5f;
	}
	for (Vertex3D_PCU& vert : m_vertices){
		vert.m_color = m_currentColor;
	}

	if (m_ageInSeconds > m_duration){
		m_aboutToBeDeleted = true;
	} else {
		m_transform.SetLocalPosition(Vector3(m_position));
		m_transform.SetRotationEuler(Vector3(0.f,0.f,m_rotationDegrees));
		m_transform.SetScale(Vector3(m_size, m_size, 1.f));
	}
}

void Star::Render()
{
	Vertex3D_PCU* vertArray = &m_vertices[0];
	g_theRenderer->PushAndTransform2(m_position, m_rotationDegrees, m_size);
	g_theRenderer->DrawMeshImmediate(vertArray, m_vertices.size(), PRIMITIVE_LINES);
	g_theRenderer->Pop2();

	//g_theRenderer->DrawRegularPolygon2(m_sides, m_position, m_size, m_rotationDegrees, m_currentColor);
}

void Star::FillVerts()
{
	float theta = (360.f /  (float) m_sides);		//internal angle of a "circle" polygon
	Vector2 start;
	Vector2 end;
	int increase = 1;

	//std::vector<Vertex3D_PCU> m_vertices = std::vector<Vertex3D_PCU>();
	bool starShaped = CheckRandomChance(.5f);
	if (starShaped){
		m_sides*=3;
	}
	float radius = 1.f;
	float diff = .9f;
	for (int i = 0; i < m_sides; i+=increase){
		float degreesStart = (theta * i);
		float degreesEnd = (theta * (i+1));
		start = (Vector2(CosDegreesf(degreesStart) * radius, SinDegreesf(degreesStart)) * radius);
		if (starShaped){
			diff *=-1.f;
			radius = radius+ diff;
		}
		end = (Vector2(CosDegreesf(degreesEnd) * radius, SinDegreesf(degreesEnd)) * radius);

		Vertex3D_PCU startPoint = Vertex3D_PCU(start, m_color, Vector2(0.f,0.f));
		Vertex3D_PCU endPoint = Vertex3D_PCU(end, m_color, Vector2(0.f,0.f));
		m_vertices.push_back(startPoint);
		m_vertices.push_back(endPoint);
		//DrawLine(start,end, color, color);
	}
	
}
