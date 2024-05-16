#include "Player.h"
#include "Scene.h"
#include "Physics.h"



/* PHYSICS CONSTANTS */
float Player::s_GravitationConstant = 4.9f;
float Player::s_JumpingHeight = .4f;
float Player::s_JumpingVelocity = glm::sqrt(2 * Player::s_JumpingHeight * Player::s_GravitationConstant);

float Player::s_PlayerAngleSpeed = float(M_PI);
float Player::s_PlayerSpeed = 2; 

float Player::s_MaxStepSize = 0.0f;

float Player::s_Height = .5f;


void Player::OnUpdate(float ts, Scene& activeScene)
{
	bool cameraRotationChanged = false;

	// Handle Camera Rotation
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Left))
	{
		m_Angle += s_PlayerAngleSpeed * ts;
		cameraRotationChanged = true;
	}
	else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Right))
	{
		m_Angle -= s_PlayerAngleSpeed * ts;
		cameraRotationChanged = true;

	}

	// Calculate new forward vector -  normalized rotation vector
	if (cameraRotationChanged)
	{
		m_ForwardDirection = { glm::cos(m_Angle),
							glm::sin(m_Angle) };

		m_RightDirection = { glm::cos(m_Angle + M_PI_2),
							glm::sin(m_Angle + M_PI_2) };


		// clamp angle value between [-2pi, 2pi]
		if (m_Angle > 2.0f * M_PI) m_Angle -= 2.0f * M_PI;
		else if (m_Angle < 2.0f * M_PI) m_Angle += 2.0f * M_PI;

	}


	//
	Sector& currentSector = activeScene.GetSectorByID(m_currentSectorID);

	bool playerPositionChanged = false;
	glm::vec2 playerDisplacement{ 0,0 };

	// Handle Input Movement
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::W))
	{
		playerDisplacement += m_ForwardDirection * s_PlayerSpeed * ts;
		playerPositionChanged = true;
	}
	else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::S))
	{
		playerDisplacement -= m_ForwardDirection * s_PlayerSpeed * ts;
		playerPositionChanged = true;
	}

	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::A))
	{
		playerDisplacement += m_RightDirection * s_PlayerSpeed * ts;
		playerPositionChanged = true;
	}
	else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::D))
	{
		playerDisplacement -= m_RightDirection * s_PlayerSpeed * ts;
		playerPositionChanged = true;
	}

	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Space) && m_IsGrounded)
	{
		m_VerticalSpeed = s_JumpingVelocity;
		m_IsGrounded = false;
	}

	/* Vertical movement handling*/
	const float previousVerticalPosition = m_VerticalPosition;
	if (!m_IsGrounded)
	{
		m_VerticalPosition += m_VerticalSpeed * ts;
		m_VerticalSpeed -= s_GravitationConstant * ts;
	}


	// calculate to which sector player has moved
	Wall* collided_wall = nullptr;

	if (playerPositionChanged)
	{
		for (auto& wall : currentSector.m_Walls)
		{
			// if crossed wall 
			if (Physics::IsIntersecting(wall->m_Point1, wall->m_Point2, m_Position, m_Position + playerDisplacement))
			{
				collided_wall = wall.get();
				break;
			}

		}
	}

	bool sectorCollision = false;

	if (collided_wall && collided_wall->m_IsPortal)
	{

		const uint32_t newSectorID =
			collided_wall -> m_portalPair.first == m_currentSectorID ?
			collided_wall -> m_portalPair.second:
			collided_wall -> m_portalPair.first;

		Sector& newSector = activeScene.GetSectorByID(newSectorID);


		/* FLOOR COLLISION */
		// case 1 - no collision
		// case 2 - collision within margin of step size -> ground player 
		// case 3 - full sector collision

		if (!m_VerticalPosition > newSector.GetSectorElevation()) // not case 1
		{

			// case 3 
			if (m_VerticalPosition < newSector.GetSectorElevation() - s_MaxStepSize)
			{
				sectorCollision = true;
			}
			// case 2
			else
			{
				m_IsGrounded = true;
				m_VerticalSpeed = 0.0f;
				m_VerticalPosition = newSector.GetSectorElevation();
			}
		}

		/* CEILING COLLISION */
		// case 1 - no collision (head below ceiling)
		// case 2 - collision within margin of step size -> shift player pos
		// case 3 - full sector collision

		else if (!(GetHeadPosition() < newSector.GetCeilingElevation())) // not case 1 
		{

			// case 3
			if (GetHeadPosition() > newSector.GetCeilingElevation() + s_MaxStepSize)
			{
				sectorCollision = true;
			}
			// case 2
			else
			{
				m_IsGrounded = false;
				m_VerticalSpeed = 0.0f;
				m_VerticalPosition = newSector.GetCeilingElevation() - s_Height - 0.001f;

			}
		}

		/* Handle Sector collision*/
		if (!sectorCollision)
		{
			m_currentSectorID = newSectorID;

			if (newSector.GetSectorElevation() < currentSector.GetSectorElevation())
			{
				m_IsGrounded = false;
			}
		}


	}
	if ((collided_wall && !collided_wall->m_IsPortal) || sectorCollision)
	{
		/* Handle Bumping into solid walls */
		// calculate new position based on displacement
		// iterating through all walls and removing prpendicular vector enabling crossing walls


		glm::vec2 wall_vector = collided_wall->m_Point1 - collided_wall->m_Point2;
		glm::vec2 wall_direction = glm::normalize(wall_vector);
			
		playerDisplacement = glm::dot(playerDisplacement, wall_direction) * wall_direction;
	
		// update the player's position
		if (currentSector.IsInside(m_Position + playerDisplacement))
		{
			m_Position += playerDisplacement;
		}

	}
	else
	{
		m_Position += playerDisplacement;
	}

	/* Vertcal collision */

	Sector& sector = activeScene.GetSectorByID(m_currentSectorID);

	// floor colision
	if (m_VerticalPosition <= sector.GetSectorElevation())
	{
		m_IsGrounded = true;
		m_VerticalSpeed = 0.0f;
		m_VerticalPosition = sector.GetSectorElevation();
	}

	// ceiling collision
	else if (GetHeadPosition() >= sector.GetCeilingElevation())
	{
		m_VerticalSpeed = 0.0f;
		m_VerticalPosition = sector.GetCeilingElevation() - s_Height - 0.001f;
	}
}

void Player::LocatePlayer(Scene& scene)
{
	// TODO: take andvantgeage of celing top and bottom
	
	for (auto& sector : scene.m_Sectors)
	{
		if (!sector.IsInside(m_Position)) continue;

		m_currentSectorID = sector.m_Id;
		return;

	}

	m_currentSectorID = 0;
}

void Player::Set(Scene& scene)
{
	LocatePlayer(scene);

	m_VerticalPosition = scene.GetSectorByID(m_currentSectorID).GetSectorElevation();
	m_VerticalSpeed = 0.0f;
	m_IsGrounded = true;
}

