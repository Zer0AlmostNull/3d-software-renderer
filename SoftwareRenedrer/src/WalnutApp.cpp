#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "glm/gtc/type_ptr.hpp"

#include "Scene.h"
#include "Player.h"
#include "Renderer.h"

using namespace Walnut;


class ApplicationLayer : public Walnut::Layer
{
public:

	ApplicationLayer()
	{
		// let player find itself inside scene /- kind of initialization
		m_Player.Set(m_Scene);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");


		if (ImGui::CollapsingHeader("Render"))
		{
			ImGui::Text("Last render: %.3f", m_LastRenderTime);


#if 1 /*Debug player movement*/

			ImGui::Separator();
			ImGui::Text("Player State");

			ImGui::Text("Grounded State: %s", m_Player.GetGroundedState() ? "true" : "false");
			ImGui::Text("p vspeed: %.3f", m_Player.m_VerticalSpeed);
			ImGui::Text("p vpos: %.3f", m_Player.m_VerticalPosition);

			ImGui::Text("Player sector: %u", m_Player.m_currentSectorID);
#endif


			// radio buttons - choosing rendering style

			ImGui::Text("renderer style: ");

			int rendererStyleId = (int)m_Renderer.GetRenderStyle();

			bool change = false;
			change |= ImGui::RadioButton("minimap", &rendererStyleId, (int)Renderer::RenderStyle::MINIMAP); ImGui::SameLine();
			change |= ImGui::RadioButton("raycaster", &rendererStyleId, (int)Renderer::RenderStyle::RAYCASTER);

			if (change) m_Renderer.SetRendererStyle((Renderer::RenderStyle)rendererStyleId);

		}
	

		if (ImGui::CollapsingHeader("Minimap Settings"))
		{
			Renderer::MinimapSettings& m_settings = m_Renderer.m_MinimapSettings;
			ImGui::ColorEdit4("bckgColor", glm::value_ptr(m_settings.backgroundColor));

			ImGui::ColorEdit4("middleRayColor", glm::value_ptr(m_settings.middleRayColor));
			ImGui::ColorEdit4("leftRayColor", glm::value_ptr(m_settings.leftRayColor));
			ImGui::ColorEdit4("rightRayColor", glm::value_ptr(m_settings.rightRayColor));


			ImGui::DragInt2("mapOffset", glm::value_ptr(m_settings.mapOffset));
			ImGui::DragFloat("mapScale", &m_settings.mapScale, 1.0f, 1.0f, 0);
			ImGui::DragFloat("visionRayScale", &m_settings.visionRayScale, 1.0f, 0.0f);
		}
	

		if (ImGui::CollapsingHeader("Raycaster Settings"))
		{

			Renderer::RaycasterSettings& r_settings = m_Renderer.m_RaycastingSettings;

			int FOVdeg = (r_settings.GetFOV() / float(M_PI)) * 180.0f;
			bool FOVchanged =
				ImGui::SliderInt("FOV", &FOVdeg, 20, 120);

			if (FOVchanged)
			{
				float newFOV = (FOVdeg / 180.0f) * float(M_PI);
				r_settings.setFOV(newFOV);
			}

			ImGui::SliderFloat("maxRayLength", &r_settings.maxRayDeph, 1.0f, 50.0f, "%.2f");
		}
		ImGui::End();

		// no padding 
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Viewport");

		// get viewport dimentisons
		m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

		//
		if (m_ImageCanvas != nullptr)
			ImGui::Image(m_ImageCanvas-> GetDescriptorSet(), { (float)m_ImageCanvas-> GetWidth(), (float)m_ImageCanvas-> GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));


		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	virtual void OnUpdate(float ts) override
	{
		m_Player.OnUpdate(ts, m_Scene);
	}

	void Resize()
	{
		// if canvas is not inited
		if (m_ImageCanvas == nullptr)
		{
			m_ImageCanvas = std::make_unique<Walnut::Image>(
				m_ViewportWidth,
				m_ViewportHeight,
				Walnut::ImageFormat::RGBA);

			m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
			m_ImageCanvas -> Resize(m_ViewportWidth, m_ViewportHeight);
		}

		// no resize necessary
		if (m_ImageCanvas->GetWidth() == m_ViewportWidth &&
			m_ImageCanvas->GetHeight() == m_ViewportHeight)
			return;

		m_ImageCanvas -> Resize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
	}

	void Render()
	{
		Timer timer;

		Resize();

		m_Renderer.Render(m_Scene, m_Player);
		m_ImageCanvas -> SetData(m_Renderer.GetPixelBuffer());

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	std::unique_ptr<Image> m_ImageCanvas = nullptr;

	Renderer m_Renderer;
	Player m_Player;
	Scene m_Scene;

	uint32_t m_ViewportHeight = 0;
	uint32_t m_ViewportWidth = 0;

	float m_LastRenderTime = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Walnut App";
	spec.Height = 600;
	spec.Width = 800;

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ApplicationLayer>();
	
	return app;
}