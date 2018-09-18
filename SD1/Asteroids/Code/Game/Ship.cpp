#include "Ship.hpp"


Ship::Ship()
{
	m_position = Vector2(SCREEN_SIZE/2, SCREEN_SIZE/2);
	m_velocity = Vector2(0.f,0.f);
	m_drawingRadius = Disc2(m_position,SCREEN_SIZE/40.f);
	m_physicsRadius = Disc2(m_position, (SCREEN_SIZE/40.f)*.6f);
	m_rotationDegrees = 90.f;
	m_spinDegreesPerSecond = 0.f;
	m_sides = 5;
	m_ageInSeconds = 0.f;
	m_controllerID = 0;
	m_color = RGBA(64,200,255);

	m_alive = true;
	m_turnDir = 0;
	m_facing = Vector2::MakeDirectionAtDegrees(90.f);
	m_firingStarburst = false;
	m_starburstStartedTime = -STARBURST_COOLDOWN;
	m_starburstCharge = -1.f;



	m_vertices[0] = Vector2(1.f,0.f);
	m_vertices[1] = Vector2(-.8f,.6f);
	m_vertices[2] = Vector2(-.5f, .3f);
	m_vertices[3] = Vector2(-.5f,-.3f);
	m_vertices[4] = Vector2(-.8f,-.6f);

	m_thrustVertices[0] = Vector2(-1.f,0.f);		//changes based on time
	m_thrustVertices[1] = Vector2(-.5f,.2f);
	m_thrustVertices[2] = Vector2(-.5f,-.2f);

	
}

//for a later time with more ships possibly
Ship::Ship(Vector2 initialPos, Vector2 initialVelocity, Disc2 outerRadius, Disc2 innerRadius, float initialRotation, float rotateSpeed, int numSides):Entity(initialPos, initialVelocity, outerRadius, innerRadius, initialRotation, rotateSpeed, numSides)
{
	m_alive = true;

	m_vertices[0] = Vector2(1.f,0.f);
	m_vertices[1] = Vector2(-.8f,.6f);
	m_vertices[2] = Vector2(-.6f, .3f);
	m_vertices[3] = Vector2(-.6f,-.3f);
	m_vertices[4] = Vector2(-.8f,-.6f);

	m_thrustVertices[0] = Vector2(-1.f,0.f);		//changes based on time
	m_thrustVertices[1] = Vector2(-.6f,.2f);
	m_thrustVertices[2] = Vector2(-.6f,-.2f);
	//init vertices

}

void Ship::Render(bool devMode)
{

	if (m_alive){
		RGBA c = m_color;
		float scale = m_drawingRadius.radius;
		
		//fade in effect
		if (m_ageInSeconds < .5f){
			c.ScaleAlpha(m_ageInSeconds*2);
			scale = m_drawingRadius.radius * (1.5f - m_ageInSeconds);
		}

		if (IsThrusting()){
			RenderThrust();
		}
		g_theRenderer->PushAndTransform2(m_position,m_rotationDegrees, scale);
		g_theRenderer->DrawShapeFromVertices2(m_vertices, m_sides, c);
		//g_theRenderer->DrawAABB2(AABB2(0.f,0.f,100.f,100.f), RGBA::MAGENTA);
		g_theRenderer->Pop2();
		
		if (m_starburstCharge > 0.f){
			RenderStarburstCharge();
		}

		RenderStarburstCooldown();
	}

	if (devMode){
		Entity::RenderDevMode();
	}
}

void Ship::RenderThrust()
{
	g_theRenderer->PushAndTransform2(m_position,m_rotationDegrees, m_drawingRadius.radius);
	g_theRenderer->DrawLine2D(m_thrustVertices[1], m_thrustVertices[0], RGBA(255,240,140), RGBA(180,100,40));
	g_theRenderer->DrawLine2D(m_thrustVertices[2], m_thrustVertices[0], RGBA(255,240,140), RGBA(180,100,40));
	g_theRenderer->Pop2();
}

void Ship::RenderStarburstCooldown(){
	float cooldownPercentage = (m_ageInSeconds - m_starburstStartedTime)/STARBURST_COOLDOWN;
	RenderCooldown(cooldownPercentage, Vector2(SCREEN_MAX - 20.f, SCREEN_MAX -20.f), 0.f,  10.f, RGBA(255,128,0,200));
}

void Ship::RenderStarburstCharge(){
	RenderCooldown(m_starburstCharge, m_position, m_rotationDegrees, m_drawingRadius.radius + 5.f, RGBA(255,128,0,200));
	if (m_starburstCharge > 1.f){
		RenderCooldown(m_starburstCharge-1.f, m_position, m_rotationDegrees,m_drawingRadius.radius + 7.f, RGBA(255,255,0,255));
	}
}

void Ship::RenderCooldown(float percentage, Vector2 pos, float rotation, float radius, RGBA color ){
	ClampZeroToOne(percentage);
	g_theRenderer->PushAndTransform2(pos, rotation, radius);
	g_theRenderer->DrawCircleArc(percentage,40,Vector2(0.f,0.f),1.f, 0.f, color);
	g_theRenderer->Pop2();
}

void Ship::Update(float deltaSeconds)
{
	
	m_position+=(m_velocity*deltaSeconds);
	m_drawingRadius.center=m_position;
	m_physicsRadius.center=m_position;
	m_ageInSeconds+=deltaSeconds;
	if (m_starburstCharge >= 0.f && m_starburstCharge < 2.f){
		m_starburstCharge+=(deltaSeconds);
	}

	if (!m_firingStarburst && (m_starburstCharge < 0.f)){
		m_rotationDegrees+= (m_spinDegreesPerSecond*deltaSeconds);

		if (IsTurningRight() && IsTurningLeft()){
			m_turnDir = 0;
		} else if (IsTurningLeft()){
			m_turnDir = 1;
		} else if (IsTurningRight()){
			m_turnDir = -1;
		} else {
			m_turnDir = 0;
		}

		CheckXboxController();
		


		//if the ship is alive, accept input to change spin speed/velocity
		if (m_alive){
			
			if (IsThrusting()){
				if (IsThrustingKeyboard()){
					m_thrustRate = MAXIMUM_THRUST;
				}
				//m_thrustVertices[0].x = (sinf(m_ageInSeconds*20.f)*.25f) - .7f;
				m_thrustVertices[0].x = (Compute1dPerlinNoise(m_ageInSeconds*20.f, 6.f)*(.25f * (m_thrustRate/MAXIMUM_THRUST))) - .8f;
				m_velocity+=(m_facing * deltaSeconds*m_thrustRate);

			}

			m_spinDegreesPerSecond = m_turnDir * 200.f;
			m_facing = Vector2::MakeDirectionAtDegrees(m_rotationDegrees);
		}

	} else {
		if (m_firingStarburst){
			if (m_turnDir == 0.f){
				m_spinDegreesPerSecond = 200.f;
			} else {
				m_spinDegreesPerSecond = m_turnDir * 200.f;
			}
			m_rotationDegrees+=(m_spinDegreesPerSecond*deltaSeconds*10.f);
			m_facing = Vector2::MakeDirectionAtDegrees(m_rotationDegrees);
			if (m_rotationDegrees - m_starburstStartDegrees > (360.f * m_starburstCharge)){
				m_firingStarburst = false; 
				m_starburstCharge = -1.f;
			}
		} else {		//chargin starburst
			m_rotationDegrees+= (m_spinDegreesPerSecond*deltaSeconds);

			m_turnDir = 0;
			//CheckXboxController();



			//if the ship is alive, accept input to change spin speed/velocity
			if (m_alive){

				m_spinDegreesPerSecond = m_turnDir * 200.f;
				m_facing = Vector2::MakeDirectionAtDegrees(m_rotationDegrees);
			}



		}

	}
	CheckScreen();
	

}

void Ship::ChargeStarburst(){
	m_starburstCharge = 0.f;
}

void Ship::FireStarburst(){
	if (m_ageInSeconds - m_starburstStartedTime > STARBURST_COOLDOWN){
		m_firingStarburst = true;
		m_starburstStartDegrees = m_rotationDegrees;
		m_starburstStartedTime = m_ageInSeconds;
	}
}

void Ship::CheckXboxController(){

	XboxController controller = g_theInput->m_controllers[m_controllerID];
	if (controller.IsConnected()){
		//check to see if left thumbstick is out of deadzone
		if(controller.GetLeftThumbstickMagnitude() > 0){
			m_rotationDegrees = controller.GetLeftThumbstickAngle();

			//set thrusting percentage
			m_thrustRate = controller.GetLeftThumbstickMagnitude() * MAXIMUM_THRUST; 
		}

		//set thrusting percentage


	}
}

bool Ship::CanFireStarburst(){
	return (m_ageInSeconds - m_starburstStartedTime > STARBURST_COOLDOWN);
}

bool Ship::IsTurningLeft()
{
	return (g_theInput->IsKeyDown(VK_LEFT) || g_theInput->IsKeyDown('S'));
}

bool Ship::IsTurningRight()
{
	return (g_theInput->IsKeyDown(VK_RIGHT) || g_theInput->IsKeyDown('F'));
}

bool Ship::IsThrusting()
{
	return (g_theInput->IsKeyDown(VK_UP) || g_theInput->IsKeyDown('E') || (g_theInput->m_controllers[m_controllerID].GetLeftThumbstickMagnitude() > 0));
}

bool Ship::IsThrustingKeyboard()
{
	return (g_theInput->IsKeyDown(VK_UP) || g_theInput->IsKeyDown('E'));
}
