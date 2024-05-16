#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "Wall.h"
#include "glm/glm.hpp"

using namespace glm;

namespace Physics
{
	/* Gets Intersection point of 2 segments */
	bool GetIntersectionPoint(vec2&, vec2&, vec2&, vec2&, vec2& out);
	/* Gets Intersection point of 2 segments */
	bool GetIntersectionPoint(Wall&, Wall&, vec2& out);

	/* Quick and inaccurate check for segments intersection*/
	bool IsIntersecting(Wall&, Wall&);
	/* Quick and inaccurate check for segments intersection*/
	bool IsIntersecting(const vec2&, const vec2&, const vec2&, const vec2&);

	/* Quick and accurate check for segment with line (first set of points is the line)*/
	bool IsIntersecting_a(const vec2& v_a, const vec2& v_b, const vec2&, const vec2&);

	/* Gets Intersection of a ray (given by direction) with segment */
	bool GetRaycast(const vec2& position, const vec2& direction, float length, const Wall&, vec2& out, float& distance);
	/* Gets Intersection of a ray (given by direction) with segment */
	bool GetRaycast(const vec2& position, const vec2& direction, float length, const Wall*, vec2& out, float& distance);
	/* Gets Intersection of a ray (given by direction) with segment */
	bool GetRaycast(const vec2& position, const vec2& direction, float length, const vec2& p1, const vec2& p2, vec2& out, float& distance);
}