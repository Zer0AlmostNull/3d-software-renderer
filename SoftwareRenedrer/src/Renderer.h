#pragma once 

#include "Walnut/Image.h"
#include "glm/glm.hpp"

#include "DrawUtils.h"
#include "Player.h"
#include "Scene.h"


#define _USE_MATH_DEFINES
#include <math.h>

class Renderer
{
public:
	
	enum RenderStyle
	{
		MINIMAP = 0,
		RAYCASTER = 1,
		EDITOR = 2,
	};
	
	struct MinimapSettings
	{
		glm::vec4 backgroundColor{ 1, 0, 0, 0 };

		glm::vec4 middleRayColor{ 1, 1, 1, 1 };
		glm::vec4 leftRayColor{ 1, .66f, 1, 1 };
		glm::vec4 rightRayColor{ 1, 0, 0, 1 };


		glm::ivec2 mapOffset{ -250, -180 };
		float mapScale = 40.0f;
		float visionRayScale = 1.0f;
		bool fillBackground = true;

		MinimapSettings() = default;

		// parameterized constructor to initialize all members
		MinimapSettings(const glm::vec4& bgColor, const glm::ivec2& offset, float scale, float visionScale, bool fillBg)
			: backgroundColor(bgColor), mapOffset(offset), mapScale(scale), visionRayScale(visionScale), fillBackground(fillBg) {}

		// copy constructor
		MinimapSettings(const MinimapSettings& other)
			: backgroundColor(other.backgroundColor), mapOffset(other.mapOffset), mapScale(other.mapScale),
			visionRayScale(other.visionRayScale), fillBackground(other.fillBackground) {}
	};

	struct RaycasterSettings
	{
	private:
		float FOV = float(M_PI) * 0.333f;
		float halfFOV = FOV / 2;
		float screenDistance = 1.0f / glm::tan(halfFOV);

	public:
		float clippingDistance = 0.001f;
		float maxRayDeph = 10.0f;

		const float& GetFOV() const { return FOV; }
		const float& GetHalfFOV() const { return halfFOV; }

		const float& GetScreenDistance() const { return screenDistance; }

		void setFOV(float& value)
		{
			FOV = value;
			halfFOV = FOV * 0.5f;

			screenDistance = 1.0f / glm::tan(halfFOV);
		}
	};

public:

	Renderer() = default;
	~Renderer() { delete[] m_ImageData; }

	void Render(Scene& scene, Player& camera);
	void OnResize(uint32_t width, uint32_t height);

	uint32_t* GetPixelBuffer() { return m_ImageData; }
	RenderStyle GetRenderStyle() const { return m_RenderStyle; }

	void SetRendererStyle(RenderStyle style) { m_RenderStyle = style; }

private:
	void RenderMap(MinimapSettings* settings = nullptr);
	void RenderRaycasting(RaycasterSettings* settings = nullptr);

public:
	MinimapSettings m_MinimapSettings;
	RaycasterSettings m_RaycastingSettings;

private:
	Scene* m_ActiveScene = nullptr;
	Player* m_ActivePlayer = nullptr;

	uint32_t m_ViewportHeight = 0;
	uint32_t m_ViewportWidth = 0;

	DrawUtils m_drawUtils{ nullptr };

	RenderStyle m_RenderStyle = RenderStyle::MINIMAP;
	uint32_t* m_ImageData = nullptr;
};