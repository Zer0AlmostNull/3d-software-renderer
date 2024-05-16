#pragma once

#include <memory>
#include <cstdint>
#include <vector>

#include "Wall.h"

class Sector
{
private:
	static const float s_maxSectorSize;

	float m_ceilingHeight; // relative to ground
	float m_floorHeight; // reloative to ground

	float m_Height;
public:

	Sector() = default;

	Sector(uint16_t id, std::vector<std::shared_ptr<Wall>>& walls,
		float floorHeight = 0.0f,
		float ceilingHeight = 1.0f, 
		glm::vec4 floorColor = { 0,0,0,1 },
		glm::vec4 ceilingColor = {1,0,1,1}
	);

	const float& GetSectorElevation() const { return m_floorHeight; }
	const float& GetCeilingElevation() const { return m_ceilingHeight; }
	const float& GetHeight() const { return m_Height; }

	uint16_t m_Id;
	
	glm::vec4 m_floorColor{ 0,0,0,1 }; // BLACK
	glm::vec4 m_ceilingColor{1, 0, 1, 1}; // MAGENTA

	std::vector<std::shared_ptr<Wall>> m_Walls;

	bool IsInside(const glm::vec2&);

};


