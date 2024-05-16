#include "Renderer.h"

#include "Walnut/Image.h"
#include "glm/glm.hpp"

DrawUtils::DrawUtils(uint32_t* _imageData): m_ViewportHeight(0), m_ViewportWidth(0)
{
	m_ImageData_ptr = _imageData;
}

void DrawUtils::SetImageDataPtr(uint32_t* ptr)
{
	m_ImageData_ptr = ptr;
}

template <typename T>
void DrawUtils::SetPixel(int x, int y, const T& color) {
    if (x < 0 || y < 0 || x >= m_ViewportWidth || y >= m_ViewportHeight) return;

    if constexpr (std::is_same_v<T, glm::vec4>) {
        m_ImageData_ptr[x + y * m_ViewportWidth] = ConvertToRGBA(color);
    }
    else if constexpr (std::is_same_v<T, uint32_t>) {
        m_ImageData_ptr[x + y * m_ViewportWidth] = color;
    }
}

template <typename T>
void DrawUtils::SetPixel(uint32_t x, uint32_t y, const T& color) {
    if (x >= m_ViewportWidth || y >= m_ViewportHeight) return;

    if constexpr (std::is_same_v<T, glm::vec4>) {
        m_ImageData_ptr[x + y * m_ViewportWidth] = ConvertToRGBA(color);
    }
    else if constexpr (std::is_same_v<T, uint32_t>) {
        m_ImageData_ptr[x + y * m_ViewportWidth] = color;
    }
}

template<typename T>
void DrawUtils::FillColor(const T& color)
{
    if constexpr (std::is_same_v<T, glm::vec4>) 
    {
        uint32_t _color = ConvertToRGBA(color);
        for (uint32_t i = 0; i < m_ViewportHeight * m_ViewportWidth; ++i)
        {
            m_ImageData_ptr[i] = _color;
        }

    }
    else if constexpr (std::is_same_v<T, uint32_t>)
    {
        for (uint32_t i = 0; i < m_ViewportHeight * m_ViewportWidth; ++i)
        {
            m_ImageData_ptr[i] = color;
        }
    }
}

template void DrawUtils::FillColor(const glm::vec4& color);
template void DrawUtils::FillColor(const uint32_t& color);


// Ensure template definitions are available to the linker
template void DrawUtils::SetPixel(int x, int y, const glm::vec4& color);
template void DrawUtils::SetPixel(int x, int y, const uint32_t& color);
template void DrawUtils::SetPixel(uint32_t x, uint32_t y, const glm::vec4& color);
template void DrawUtils::SetPixel(uint32_t x, uint32_t y, const uint32_t& color);

// Thanks to ssavi-ict
// https://gist.github.com/ssavi-ict
void DrawUtils::DrawLine(int xa, int ya, int xb, int yb, glm::vec4& color)
{
    uint32_t color_ = ConvertToRGBA(color);

    DrawLine(xa, ya, xb, yb, color_);
}
void DrawUtils::DrawLine(int xa, int ya, int xb, int yb, uint32_t& color)
{
    int dx = abs(xb - xa);
    int dy = abs(yb - ya);
    int sx = (xa < xb) ? 1 : -1;
    int sy = (ya < yb) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        SetPixel(xa, ya, color);

        if (xa == xb && ya == yb)
            break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            xa += sx;
        }
        if (e2 < dx) {
            err += dx;
            ya += sy;
        }
    }
}

void DrawUtils::DrawHLine(int x0, int x1, uint32_t y, glm::vec4& color)
{
    // sort points x0 < x1
    if (x0 > x1) std::swap(x1, x0);

    uint32_t color_ = ConvertToRGBA(color);

    for (int i = x0; i <= x1; ++i)
    {
        SetPixel((int)i, (int)y, (uint32_t&)color_);
    }
}

void DrawUtils::DrawHLine(int x0, int x1, uint32_t y, uint32_t& color)
{
    // sort points x0 < x1
    if (x0 > x1) std::swap(x1, x0);

    for (int i = x0; i <= x1; ++i)
    {
        SetPixel((int)i, (int)y, (uint32_t&)color);
    }
}

void DrawUtils::DrawVLine(int y0, int y1, uint32_t x, glm::vec4& color)
{

    // sort points y0 < x1
    if (y0 > y1) std::swap(y0, y1);

    if (y0 >= m_ViewportHeight) y0 = m_ViewportHeight - 1;
    if (y1 >= m_ViewportHeight) y1 = m_ViewportHeight - 1;
    if (x >= m_ViewportWidth) return;

    uint32_t _color = ConvertToRGBA(color);

    for (int i = y0; i <= y1; ++i)
    {
        m_ImageData_ptr[x + i * m_ViewportWidth] = _color;
    }
}

void DrawUtils::DrawVLine(int y0, int y1, uint32_t x, uint32_t& color)
{
    // sort points y0 < x1
    if (y0 > y1) std::swap(y0, y1);

    if (y0 >= m_ViewportHeight) y0 = m_ViewportHeight - 1;
    if (y1 >= m_ViewportHeight) y1 = m_ViewportHeight - 1;
    if (x >= m_ViewportWidth) return;


    for (int i = y0; i <= y1; ++i)
    {
        m_ImageData_ptr[x + i * m_ViewportWidth] = color;
    }
}

// Thanks to Gumichan01
// https://gist.github.com/Gumichan01/
void DrawUtils::DrawCircle(uint32_t x, uint32_t y, float radius, glm::vec4& color)
{
    uint32_t color_ = ConvertToRGBA(color);

    DrawCircle(x, y, radius, color_);
}

void DrawUtils::DrawCircle(uint32_t x, uint32_t y, float radius, uint32_t& color)
{
    int offsetx, offsety, d;

    offsetx = 0;
    offsety = (int)radius;
    d = offsety - 1;

    while (offsety >= offsetx)
    {
        SetPixel(x + offsetx, y + offsety, color);
        SetPixel(x + offsety, y + offsetx, color);
        SetPixel(x - offsetx, y + offsety, color);
        SetPixel(x - offsety, y + offsetx, color);
        SetPixel(x + offsetx, y - offsety, color);
        SetPixel(x + offsety, y - offsetx, color);
        SetPixel(x - offsetx, y - offsety, color);
        SetPixel(x - offsety, y - offsetx, color);

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }
}

void DrawUtils::DrawCircleFilled(uint32_t x, uint32_t y, float radius, glm::vec4& color)
{
    int offsetx, offsety, d;

    offsetx = 0;
    offsety = (int)radius;
    d = offsety - 1;


    while (offsety >= offsetx) {

        DrawHLine(x - offsety, x + offsety, y + offsetx, color);
        DrawHLine(x - offsetx, x + offsetx, y + offsety, color);
        DrawHLine(x - offsetx, x + offsetx, y - offsety, color);
        DrawHLine(x - offsety, x + offsety, y - offsetx, color);

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }
}

uint32_t DrawUtils::ConvertToRGBA(const glm::vec4& color)
{
    uint8_t r = (uint8_t)(color.r * 255.0f);
    uint8_t g = (uint8_t)(color.g * 255.0f);
    uint8_t b = (uint8_t)(color.b * 255.0f);
    uint8_t a = (uint8_t)(color.a * 255.0f);

    return ((a << 24) | (b << 16) | (g << 8) | r);
}

glm::vec4 DrawUtils::ConvertFromRGBA(const uint32_t& color)
{
    uint8_t r = color & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = (color >> 16) & 0xFF;
    uint8_t a = (color >> 24) & 0xFF;


    return glm::vec4{
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f
    };
}


