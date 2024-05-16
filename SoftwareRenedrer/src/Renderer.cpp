#include "Renderer.h"
#include "Scene.h"
#include "Physics.h"

void Renderer::Render(Scene& scene, Player& camera)
{
	m_ActivePlayer = &camera;
	m_ActiveScene = &scene;

	// send ptr to image data to draw utils
	m_drawUtils.SetImageDataPtr(m_ImageData);

	switch (m_RenderStyle)
	{
	case Renderer::MINIMAP:
		RenderMap();
		break;
	case Renderer::RAYCASTER:
	default:
		RenderRaycasting();
		break;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	m_ViewportHeight = height;
	m_ViewportWidth = width;


	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	m_drawUtils.SetDimentions(width, height);
	m_drawUtils.SetImageDataPtr(m_ImageData);
}

void Renderer::RenderMap(MinimapSettings* settings)
{
	if (!settings) settings = &m_MinimapSettings;

	const glm::ivec2 screen_cener{ m_ViewportWidth / 2, m_ViewportHeight / 2 };

	auto pointToScreen = [&](const glm::vec2& point) -> glm::ivec2 {
		return glm::ivec2{ point * settings->mapScale } + screen_cener + settings->mapOffset;
		};

	// set background color
	if (settings->fillBackground)
		m_drawUtils.FillColor(settings->backgroundColor);

	// draw each wall
	for (const auto& sector : m_ActiveScene->m_Sectors)
	{
		for (const auto& wall : sector.m_Walls)
		{
			const ivec2 p1 = pointToScreen(wall->m_Point1);
			const ivec2 p2 = pointToScreen(wall->m_Point2);


			m_drawUtils.DrawLine(p1.x, p1.y, p2.x, p2.y, (uint32_t&)wall->m_ColorU);
		}
	}

	// draw player
	const ivec2 playerScreenPos = pointToScreen(m_ActivePlayer->m_Position);
	m_drawUtils.DrawCircle(playerScreenPos.x, playerScreenPos.y, 5.0f, glm::vec4{ 1,1,1,1 });

	// draw vision rays
	const float leftAngle = m_ActivePlayer->m_Angle + m_RaycastingSettings.GetHalfFOV();
	const float rightAngle = m_ActivePlayer->m_Angle - m_RaycastingSettings.GetHalfFOV();

	const ivec2 forward = pointToScreen(m_ActivePlayer->m_Position + m_ActivePlayer->m_ForwardDirection * settings->visionRayScale);
	const ivec2 right = pointToScreen(m_ActivePlayer->m_Position + glm::vec2{ glm::cos(rightAngle), glm::sin(rightAngle) } *settings->visionRayScale);
	const ivec2 left = pointToScreen(m_ActivePlayer->m_Position + glm::vec2{ glm::cos(leftAngle), glm::sin(leftAngle) } *settings->visionRayScale);

	m_drawUtils.DrawLine(playerScreenPos.x, playerScreenPos.y, forward.x, forward.y, settings->middleRayColor);
	m_drawUtils.DrawLine(playerScreenPos.x, playerScreenPos.y, right.x, right.y, settings->rightRayColor);
	m_drawUtils.DrawLine(playerScreenPos.x, playerScreenPos.y, left.x, left.y, settings->leftRayColor);


#if 0



	auto& collision_walls = m_ActiveScene->m_Walls;
	int num_rays = 50;


	for (int i = 0; i < num_rays; ++i)
	{
		// from -1 ..0.. 1 angle redirection cooefitient
		const float ray_deviation = ((float)i / num_rays) * 2 - 1;
		const float ray_angle = m_ActivePlayer->m_Angle - ray_deviation * m_RaycastingSettings.GetFOV();
		const glm::vec2 ray_direction{ glm::cos(ray_angle), glm::sin(ray_angle) };



		Wall* collided_wall = nullptr;
		glm::vec2 collision_point;
		float collision_distance;
		bool collision = false;

		// find with which wall colliison is
		for (auto& wall : collision_walls)
		{
			collision = Physics::GetRaycast(m_ActivePlayer->m_Position, ray_direction, 10.0f, wall.get(), collision_point, collision_distance);

			if (!collision) continue;

			glm::ivec2 p = pointToScreen(collision_point);

			m_drawUtils.DrawLine(
				playerScreenPos.x,
				playerScreenPos.y,
				p.x,
				p.y, glm::vec4{1,1,1,1});

			m_drawUtils.DrawCircle(p.x, p.y, 5.0f, glm::vec4{1, 0, 0, 1});

		}
	}
#endif // DEBUG
}

void Renderer::RenderRaycasting(RaycasterSettings* settings)
{
	if (!settings) settings = &m_RaycastingSettings;


	// clear screen b4 rendering
	//memset(m_ImageData, 0, m_ViewportHeight * m_ViewportWidth * sizeof(uint32_t));


	const float pixel_height_multiplier = (m_ViewportHeight * .5f) * settings->GetScreenDistance();
	const float cameraY = m_ActivePlayer -> GetHeadPosition();


	// main drawing loop TODO: multithreading - execution pool
	for (uint32_t x = 0; x < m_ViewportWidth; x++)
	{
		// from -1 ..0.. 1 angle redirection cooefitient
		const float ray_deviation = ((float)x / m_ViewportWidth) * 2 - 1;
		const glm::vec2& ray_orgin = m_ActivePlayer->m_Position;

		const float ray_angle = m_ActivePlayer->m_Angle - ray_deviation * settings->GetHalfFOV();
		const glm::vec2 ray_direction{ glm::cos(ray_angle), glm::sin(ray_angle) };

		bool rendering_loop = true;


		// holds id of current ray sector 
		uint16_t current_sectorID = m_ActivePlayer->m_currentSectorID;
		Sector* current_sector = nullptr;
		Sector* previous_sector = nullptr;

		// holds pointer to last portal
		Wall* previous_collided_wall = nullptr;

		// rendering frame
		float old_frame_bottom = 0;
		float old_frame_top = m_ViewportHeight;

		// get current sector from scene manager
		if (current_sectorID)
		{
			current_sector = &m_ActiveScene->GetSectorByID(current_sectorID);
		}
		else
		{
			current_sector = &m_ActiveScene->s_WorldSector; // quick hack to render outsude of any sector 
		}

		// --- RENDERING LOOP ---
		// im casting ray within current sector
		// if ray hits wall it is being drawn 
		// if ray hits portal ray is being casted again in new sector 
		do
		{
			Wall* collided_wall = nullptr;
			Sector* new_sector = nullptr;
			uint16_t new_sectorID = 0;

			glm::vec2 collision_point;
			float collision_distance;

			// find with which wall collision is
			for (auto& wall : current_sector->m_Walls)
			{
				bool collision = Physics::GetRaycast(m_ActivePlayer->m_Position, ray_direction, settings->maxRayDeph, wall.get(), collision_point, collision_distance);

				if (collision && wall.get() != previous_collided_wall)
				{
					collided_wall = wall.get();
					break;
				}
			}

			// if no collision skip rendering
			if (!collided_wall) break;
			//if (collision_distance < settings->clippingDistance) break;


			// check for portal 
			bool is_portal = false;
			if (!collided_wall->m_IsPortal) rendering_loop = false; 
			else
			{
				// get new sector id
				std::pair<uint16_t, uint16_t>& portalPair = collided_wall -> m_portalPair;

				if (portalPair.first == current_sectorID) new_sectorID = portalPair.second;
				else new_sectorID = portalPair.first;

				new_sector = &m_ActiveScene->GetSectorByID(new_sectorID);

				previous_collided_wall = collided_wall;
				is_portal = true;
			}

			// Rendering diagram
			/*
			 \                     / <- old frame top
			  \                   /
			   \                 /
			    \_______________/ <- frame top
				|               |
				|_______________| <- portal top
				|\             /|
				|  \         /  |
				|    \_____/    | 
				|    |     |    |
				|    |     |    |
				|    |_____|    |
				|   /      \    |
				| /          \  |
				|/_____________\| <- portal bottom
				|               |
				|_______________|  <- frame bottom
			   /                 \
			  /                   \
		     /                     \  <- old frame bottom
			*/

			// fish eye reduction
			collision_distance *= glm::cos(m_ActivePlayer->m_Angle - ray_angle);

			// one unit of height viewied at current distance (half of screen)
			const float projection_scale_multiplier = (pixel_height_multiplier / (collision_distance + 0.0001f));

			// calculate frame variables accordin to the diagram below
			float frame_bottom = m_ViewportHeight * .5f + (current_sector->GetSectorElevation() - cameraY) * projection_scale_multiplier;
			float frame_top = m_ViewportHeight * .5f + (current_sector->GetCeilingElevation() - cameraY) * projection_scale_multiplier;
			float frame_height = frame_top - frame_bottom;
			// NOTE! at this stage, frame_top, frame_bottom might be out of bounds
			
			float portal_top;
			float portal_bottom;

			if (is_portal)
			{
				portal_bottom = m_ViewportHeight * .5f + (new_sector->GetSectorElevation() - cameraY) * projection_scale_multiplier; 
				portal_top = m_ViewportHeight * .5f + (new_sector->GetCeilingElevation() - cameraY) * projection_scale_multiplier;
			}

			// clamp frame values not to exceed old frame boundries 
			if (frame_top > old_frame_top) frame_top = old_frame_top;
			if (frame_bottom < old_frame_bottom) frame_bottom = old_frame_bottom;


			// calculate wall color based on the distance
			float colorIntensity;
			if (glm::length(collision_point - collided_wall->m_Point1) < 0.015f ||
				glm::length(collision_point - collided_wall->m_Point2) < 0.015f)
			{
				colorIntensity = 0;
			}
			else
			{
				colorIntensity = 1 - powf(collision_distance * (1.0f / settings->maxRayDeph), 4);
			}

			// non normal rendering pass - portrtal rendering
			if (is_portal)
			{
				// drawing portal "edges"

				// UPPER
				if (frame_top > portal_top)
					m_drawUtils.DrawVLine(frame_top,
						portal_top,
						x,
						collided_wall->m_ColorV * colorIntensity);

				// LOWER
				if (frame_bottom < portal_bottom)
					m_drawUtils.DrawVLine(frame_bottom,
						portal_bottom,
						x,
						collided_wall->m_ColorV * colorIntensity);
			}
			else // draw level edges
			{
				
				uint32_t color = DrawUtils::ConvertToRGBA(collided_wall->m_ColorV * colorIntensity);

				m_drawUtils.DrawVLine(frame_bottom,
					frame_top,
					x,
					color);
			}

				// draw floor
				m_drawUtils.DrawVLine(frame_bottom,
					old_frame_bottom,
					x,
					current_sector->m_floorColor);

				// draw ceiling
				m_drawUtils.DrawVLine(frame_top,
					old_frame_top,
					x,
					current_sector->m_ceilingColor);
			

				

			// assign memory values

			current_sectorID = new_sectorID;


			if (is_portal)
			{
				old_frame_top = glm::min(portal_top, frame_top);
				old_frame_bottom = glm::max(portal_bottom, frame_bottom);
			}
			else
			{
				old_frame_top = frame_top;
				old_frame_bottom = frame_bottom;
			}
			

			previous_sector = current_sector;
			current_sector = new_sector;


		} while (rendering_loop);
	}

	/*
	* 
	// draw minimap overlay
	MinimapSettings minimapSettings{ m_MinimapSettings };

	minimapSettings.fillBackground = false;

	RenderMap(&minimapSettings);
	*/
}

