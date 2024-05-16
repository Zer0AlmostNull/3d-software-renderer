#pragma once

#include "glm/glm.hpp"

namespace RandomUtils
{
	static uint32_t pcg_hash(uint32_t seed)
	{
		uint32_t state = seed * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	static float RandomFloat(uint32_t& seed)
	{
		constexpr auto SCALE = 1.0f / (float)UINT32_MAX;

		seed = pcg_hash(seed);
		return (float)seed * SCALE;
	}

	static glm::vec3 InUnitSphere(uint32_t& seed)
	{
		return glm::normalize(glm::vec3(
			RandomFloat(seed) * 2.0f - 1.0f,
			RandomFloat(seed) * 2.0f - 1.0f,
			RandomFloat(seed) * 2.0f - 1.0f)
		);
	}

}