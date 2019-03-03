#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/Chunk.hpp"
#include "Game/BlockDefinition.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"

Player::Player(GameState_Playing* playState, Vector3 position)
{
	float size = 1.f;
	m_collider = Sphere(Vector3::ZERO, size);
	m_renderable = new Renderable();//new Renderable(RENDERABLE_CUBE, 1.f);
	//make tank
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(Vector3(0.f, 0.f, 0.f), Vector3::ONE * .5f, RGBA::GREEN, RIGHT, UP, FORWARD);
	mb.End();
	m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_LIT));



	m_spinDegreesPerSecond = 45.f;
	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;


	m_positionXZ = position.XZ();

	//set up auxilary transforms: camera target, turret renderable, barrel fire position, laser sight...


	//m_turretRenderable->m_transform.SetParent(m_cameraTarget);

	m_playState = playState;

	m_health = 50;
	m_maxHealth = m_health;

	m_cameraTarget = new Transform();
	m_cameraTarget->SetParent(&m_renderable->m_transform);

	SetPosition(position);
	SetScale(Vector3(size,size,size));
}

Player::~Player()
{

}

void Player::Update()
{

	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds+=ds;
	if (!g_theGame->IsDevMode()){
		m_digRaycast = m_playState->m_world->Raycast(m_cameraTarget->GetWorldPosition(), m_cameraTarget->GetForward(), m_digDistance);
	}

}

void Player::HandleInput()
{
//	float ds = g_theGame->GetDeltaSeconds();
	HandleInputFreeCamera();
	HandleInputDigPlace();


	if (g_theInput->WasKeyJustPressed('U'))
	{
		m_playState->m_world->DebugDeactivateAllChunks();
	}


	if (g_theInput->WasKeyJustPressed(BRACKET_SQUARE_RIGHT))
	{
		m_currentPlaceBlockType = (m_currentPlaceBlockType + 1) % BlockDefinition::s_blockDefinitions.size();
		if (m_currentPlaceBlockType == BLOCK_AIR)
		{
			//wrap to 1 instead of air
			m_currentPlaceBlockType = 1U;
		}
	}

	if (g_theInput->WasKeyJustPressed(BRACKET_SQUARE_LEFT))
	{
		m_currentPlaceBlockType = (m_currentPlaceBlockType + (uchar) BlockDefinition::s_blockDefinitions.size() - 1) % (uchar) BlockDefinition::s_blockDefinitions.size();
		if (m_currentPlaceBlockType == BLOCK_AIR)
		{
			//wrap to highest id instead of air (Note: even if s_blockDefinitions isn't ordered by ID this should still be the highest ID we have.
			m_currentPlaceBlockType = (uchar) BlockDefinition::s_blockDefinitions.size() - 1;
		}
	}
}

void Player::Damage()
{
	if (!g_theGame->m_godMode){
		m_health--;
		if (m_health <= 0){
			m_aboutToBeDeleted = true;
		}
	}
}

void Player::Respawn()
{
	AddRenderable();
	m_aboutToBeDeleted = false;
	m_health = m_maxHealth;
}

void Player::RemoveRenderable()
{
	g_theGame->GetScene()->RemoveRenderable(m_renderable);
}

void Player::AddRenderable()
{
	g_theGame->GetScene()->AddRenderable(m_renderable);
}

float Player::GetPercentageOfHealth() const
{
	float t = RangeMapFloat((float) m_health, 0.f, (float) m_maxHealth, 0.f, 1.f);
	return t;
}

void Player::HandleInputFreeCamera()
{
	float ds = g_theGame->GetDeltaSeconds();
	float speed = m_speed;
	if (g_theInput->IsKeyDown(VK_SHIFT)){
		speed *= m_shiftMultiplier;
	}

	float deltaYaw = 0.f;
	float deltaPitch = 0.f;

	Vector2 mouseDirection = g_theInput->GetMouseDirection();
	mouseDirection *= .5f;


	if (g_theInput->IsKeyDown(VK_UP)){
		deltaPitch -= 1.f;			// positive deltaPitch is from z towards x
	}
	if (g_theInput->IsKeyDown(VK_DOWN)){
		deltaPitch +=1.f;
	}
	if (g_theInput->IsKeyDown(VK_RIGHT)){
		deltaYaw -= 1.f;		//positive yaw is from right to left
	}
	if (g_theInput->IsKeyDown(VK_LEFT)){
		deltaYaw +=1.f;		
	}

	deltaPitch += 1.f * mouseDirection.y;		//up and down is  deltaPitch
	deltaYaw += -1.f * mouseDirection.x;				//left and right is yaw

	Vector2 controllerRotation = g_theInput->GetController(0)->GetRightThumbstickCoordinates();
	deltaYaw += -1.f * controllerRotation.x;
	deltaPitch += -1.f * controllerRotation.y;

	/*deltaYaw *= degPerSecond * ds;
	deltaPitch *= degPerSecond * ds;
	*/
	Rotate(deltaYaw, deltaPitch, 0.f);

	Vector3 forward = GetForward();
	Vector3 forwardLockedVertical = Vector3(forward.x, forward.y, 0.f);
	forwardLockedVertical.NormalizeAndGetLength();

	//why are there 6 keys if there are only 4 directions
	if (g_theInput->IsKeyDown('D')){
		Translate(GetRight() * ds * speed);
	}
	if (g_theInput->IsKeyDown('A')){
		Translate(GetRight() * ds * speed * -1.f);
	}
	if (g_theInput->IsKeyDown('W')){
		Translate(forwardLockedVertical * ds* speed );
	}
	if (g_theInput->IsKeyDown('S')){
		Translate(forwardLockedVertical * ds * speed * -1.f);
	}
	if (g_theInput->IsKeyDown('E') || g_theInput->GetController(0)->IsButtonDown(XBOX_BUMPER_RIGHT)){
		Translate(UP * ds* speed );		//going off of camera's up feels very weird when it's not perfectly upright
	}
	if (g_theInput->IsKeyDown('Q') || g_theInput->GetController(0)->IsButtonDown(XBOX_BUMPER_LEFT)){
		Translate(UP * ds * speed * -1.f);
	}

	Vector2 controllerTranslation = g_theInput->GetController(0)->GetLeftThumbstickCoordinates();

	Translate(forwardLockedVertical * controllerTranslation.y * ds * speed);
	Translate(GetRight() * controllerTranslation.x * ds * speed);


}

void Player::HandleInputDigPlace()
{
	if (g_theInput->WasMouseButtonJustPressed(MOUSE_BUTTON_LEFT))
	{
		//dig the block we're looking at
		if (m_digRaycast.DidImpact()){
			m_digRaycast.m_impactBlock.m_chunk->DigBlock(m_digRaycast.m_impactBlock.m_blockIndex, BLOCK_AIR);
		}
	}

	if (g_theInput->WasMouseButtonJustPressed(MOUSE_BUTTON_RIGHT))
	{
		//Place a block
		if (m_digRaycast.DidImpact()){
			Vector3 placePosition = m_digRaycast.m_impactBlock.GetBlockCenterWorldPosition() + m_digRaycast.m_impactNormal;
			BlockLocator placeBlock = m_playState->m_world->GetBlockLocatorAtWorldPosition(placePosition);
			placeBlock.m_chunk->PlaceBlock(placeBlock.m_blockIndex, m_currentPlaceBlockType);
		}
	}
}


