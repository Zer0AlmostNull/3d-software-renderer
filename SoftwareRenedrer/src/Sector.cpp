#include "Sector.h"
#include "Wall.h"
#include "Physics.h"

#include <type_traits>

const float Sector::s_maxSectorSize = 50.0f;

Sector::Sector(uint16_t id, std::vector<std::shared_ptr<Wall>>& walls, float floorHeight, float ceilingHeight, glm::vec4 floorColor, glm::vec4 ceilingColor) :
    m_Id(id), m_Walls(walls), m_floorHeight(floorHeight), m_ceilingHeight(ceilingHeight), m_floorColor(floorColor), m_ceilingColor(ceilingColor)
{

    if (m_ceilingHeight < m_floorHeight) std::swap(m_floorHeight, m_ceilingHeight);
    m_Height = m_ceilingHeight - m_floorHeight;


}

bool Sector::IsInside(const glm::vec2& point)
{
    uint32_t c1 = 0, c2 = 0, c3 = 0;

    glm::vec2 pos1{ Sector::s_maxSectorSize, 0};
    glm::vec2 pos2{ -Sector::s_maxSectorSize, 0};
    glm::vec2 pos3{ 0, Sector::s_maxSectorSize};

    for (const auto& wall : m_Walls)
    {

        if (Physics::IsIntersecting(point, pos1, wall->m_Point1, wall->m_Point2))
            c1 += 1;

        if (Physics::IsIntersecting(point, pos1, wall->m_Point1, wall->m_Point2))
            c2 += 1;

        if (Physics::IsIntersecting(point, pos1, wall->m_Point1, wall->m_Point2))
            c3 += 1;
    }

    // if at least two odd then inside
    return (c1 % 2 == 1) + (c2 % 2 == 1) + (c3 % 2 == 1) >= 2;
}