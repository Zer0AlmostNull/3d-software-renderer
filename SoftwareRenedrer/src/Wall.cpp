#include "Wall.h"
#include "DrawUtils.h"

template<typename T>
Wall::Wall(const vec2& p1, const vec2& p2, T _color, std::pair<uint16_t, uint16_t> pair)
{
    // set pair
    m_portalPair = pair;

    // detect if no portal
    if (pair.first == pair.second) m_IsPortal = false;
    else m_IsPortal = true;

    // sort points by x. 
    if (p1.x < p2.x) { m_Point1 = p1; m_Point2 = p2; }
    else { m_Point1 = p2; m_Point2 = p1; }


    // calculate lenght of the wall *might be usefull in future
    m_Length = glm::length((m_Point1 - m_Point2));

    if constexpr (std::is_same_v<T, glm::vec4>)
    {
        m_ColorV = _color;
        m_ColorU = DrawUtils::ConvertToRGBA(_color);
    }
    else if constexpr (std::is_same_v<T, uint32_t>)
    {
        m_ColorU = _color;
        m_ColorV = DrawUtils::ConvertFromRGBA(_color);
    }

}

template Wall::Wall(const vec2& p1, const vec2& p2, uint32_t _color, std::pair<uint16_t, uint16_t> pair);
template Wall::Wall(const vec2& p1, const vec2& p2, vec4 _color, std::pair<uint16_t, uint16_t> pair);
