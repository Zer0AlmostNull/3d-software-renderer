#pragma once
#include <memory>
#include <vector>

#include "Wall.h"
#include "Sector.h"


class Scene
{
public:

	static Sector s_WorldSector;

	std::vector<Sector> m_Sectors;
	

	std::vector<std::shared_ptr<Wall>> m_Walls;

	Scene(); // constructs default scene


	Sector& searchSectorByID(const uint32_t& id);
	Sector& GetSectorByID(const uint32_t& id);

};