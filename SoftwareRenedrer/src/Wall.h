#pragma once

#include <set>
#include "glm/glm.hpp"

using namespace glm;

class Wall
{
private:
	float m_Length;

public:
	Wall() : m_Point1({ 0,0 }), m_Point2({ 0,0 }), m_IsPortal(false), m_ColorU(0xFFFFFFFF), m_ColorV({1, 1, 1, 1}), m_Length(0.0f) {}

	template<typename T>
	Wall(const vec2& p1, const vec2& p2, T _color, std::pair<uint16_t, uint16_t> pair = {0, 0});

	const float& GetLength() const { return m_Length; }


	// if is not a portal is 0
	std::pair<uint16_t, uint16_t> m_portalPair;
	bool m_IsPortal;

	uint32_t m_ColorU;
	vec4 m_ColorV;


	vec2 m_Point1, m_Point2;

};