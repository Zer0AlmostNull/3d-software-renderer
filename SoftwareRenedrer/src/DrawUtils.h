#pragma once
#include "glm/glm.hpp"

class DrawUtils
{
public:
	DrawUtils(uint32_t*);
	void SetDimentions(uint32_t width, uint32_t height) { m_ViewportHeight = height; m_ViewportWidth = width; }
	void SetImageDataPtr(uint32_t* ptr);


	template <typename T>
	void SetPixel(int x, int y, const T& color);

	template <typename T>
	void SetPixel(uint32_t x, uint32_t y, const T& color);

	void DrawLine(int xa, int ya, int xb, int yb, glm::vec4& color);
	void DrawLine(int xa, int ya, int xb, int yb, uint32_t& color);
	void DrawHLine(int x0, int x1, uint32_t y, glm::vec4& color);
	void DrawHLine(int x0, int x1, uint32_t y, uint32_t& color);

	void DrawVLine(int y0, int y1, uint32_t x, glm::vec4& color);
	void DrawVLine(int y0, int y1, uint32_t x, uint32_t& color);

	void DrawCircle(uint32_t x, uint32_t y, float radius, glm::vec4& color);
	void DrawCircle(uint32_t x, uint32_t y, float radius, uint32_t& color);

	void DrawCircleFilled(uint32_t x, uint32_t y, float radius, glm::vec4& color);

	template <typename T>
	void FillColor(const T& color);

	static uint32_t ConvertToRGBA(const glm::vec4& color);
	static glm::vec4 DrawUtils::ConvertFromRGBA(const uint32_t& color);

private:
	uint32_t* m_ImageData_ptr = nullptr;
	uint32_t m_ViewportWidth, m_ViewportHeight;
};


