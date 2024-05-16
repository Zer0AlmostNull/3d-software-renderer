#include <memory>


#include "Scene.h"
#include "DrawUtils.h"

Sector Scene::s_WorldSector;

Scene::Scene()
{

	m_Walls.clear();
	m_Walls.insert(m_Walls.end(), {
		std::make_shared<Wall>(vec2{2.0f, 8.0f}, vec2{2.0f, 4.0f}, glm::vec4{0.00f, 0.00f, 1.00f, 1.00f}),
		std::make_shared<Wall>(vec2{2.0f, 4.0f}, vec2{3.0f, 2.0f}, glm::vec4{0.84f, 0.26f, 0.96f, 1.00f}),
		std::make_shared<Wall>(vec2{3.0f, 2.0f}, vec2{6.0f, 4.0f}, glm::vec4{0.26f, 0.93f, 0.96f, 1.00f}),
		std::make_shared<Wall>(vec2{6.0f, 4.0f}, vec2{8.0f, 6.0f}, glm::vec4{0.13f, 0.96f, 0.42f, 1.00f}),
		std::make_shared<Wall>(vec2{2.0f, 8.0f}, vec2{8.0f, 8.0f}, glm::vec4{0.00f, 0.00f, 1.00f, 1.00f}),

		std::make_shared<Wall>(vec2{8.0f, 6.0f}, vec2{8.0f, 8.0f}, glm::vec4{0.96f, 0.26f, 0.27f, 1.00f}, std::make_pair<uint16_t, uint16_t>(1, 2)),

		std::make_shared<Wall>(vec2{10.0f, 8.0f}, vec2{8.0f, 8.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.00f}),
		std::make_shared<Wall>(vec2{10.0f, 8.0f}, vec2{10.0f, 6.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.00f}),
		std::make_shared<Wall>(vec2{8.0f, 6.0f}, vec2{10.0f, 6.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.00f}),
	});

	uint16_t currentSectorID = 0;
	s_WorldSector = Sector(currentSectorID++, m_Walls);

	// add walls to sectors
	m_Sectors.reserve(2);
	{
		std::vector<std::shared_ptr<Wall>> walls;

		for (int i = 0; i < 6; ++i)
		{
			walls.emplace_back(m_Walls[i]);
		}
		glm::vec4 sector_color{ 5.00f / 255, 8.20f / 255, 16.80f / 255, 1.0f };
		m_Sectors.emplace_back(currentSectorID++, walls, 0.0f, 1.0f, sector_color, sector_color);
	}

	{
		std::vector<std::shared_ptr<Wall>> walls;

		for (int i = 5; i < 9; ++i)
		{
			walls.emplace_back(m_Walls[i]);
		}
		
		glm::vec4 sector_color{ 10.0f / 255, 16.4f / 255, 32.0f / 255, 1.0f };
		m_Sectors.emplace_back(currentSectorID++, walls, 0.3f, 1.1f, sector_color, sector_color);
	}
}

// Iterates through all sector checking the IDs
// if fails to find sector returns reference to first sector
Sector& Scene::searchSectorByID(const uint32_t& id)
{
	for (auto& sector : m_Sectors)
	{
		if (sector.m_Id == id)
			return sector;
	}

	return m_Sectors[0];
}

// Assumes sectors id are according to the convention that id = [index] + 1  
Sector& Scene::GetSectorByID(const uint32_t& id)
{
	return m_Sectors[id - 1];
}
