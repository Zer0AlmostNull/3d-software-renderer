#pragma once

#include <glm/glm.hpp>

#include "Walnut/Input/Input.h"
#include "Scene.h"

class Player
{
private:
	static float s_GravitationConstant;
	static float s_JumpingHeight;
	static float s_JumpingVelocity;

	static float s_PlayerSpeed;
	static float s_PlayerAngleSpeed;

	static float s_MaxStepSize;

	static float s_Height;

private:
	bool m_IsGrounded = true;
public:

	float m_Angle = 0;
	uint32_t m_currentSectorID = 0;

	float m_VerticalPosition = 0.0f;
	float m_VerticalSpeed = 0.0f; 

	glm::vec2 m_Position {5.0f, 5.0f};
	glm::vec2 m_ForwardDirection {1.0f, 0.0f};
	glm::vec2 m_RightDirection {0.0f, 1.0f };

	const float GetHeadPosition() const { return m_VerticalPosition + s_Height; }
	const bool GetGroundedState() const { return m_IsGrounded; }


	void OnUpdate(float ts, Scene& s);

	// place player in the scene according to its position
	void Set(Scene&);


private:
	void LocatePlayer(Scene& s);
};