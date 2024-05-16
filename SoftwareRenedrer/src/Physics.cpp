#include "Physics.h"


// borrowed (stolen) from
// https://en.wikipedia.org/wiki/Line–line_intersection

bool Physics::GetIntersectionPoint(vec2& p1, vec2& p2, vec2& p3, vec2& p4, vec2& out)
{
    float denominator = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);

    // if lines are prarell
    if (denominator == 0)
        return false;

    //
    float t = ((p1.x - p3.x) * (p3.y - p4.y) - (p1.y - p3.y) * (p3.x - p4.x)) / denominator;
    float u = -((p1.x - p2.x) * (p1.y - p3.y) - (p1.y - p2.y) * (p1.x - p3.x)) / (denominator);

    // if not 0<=t<=1 and 0<=u<=1 
    if (t < 0 || t > 1 || u < 0 || u > 1) return false;

    out.x = p1.x + t * (p2.x - p1.x);
    out.y = p1.y + t * (p2.y - p1.y);

    return true;
}
bool Physics::GetIntersectionPoint(Wall& a, Wall& b, vec2& out)
{
    return Physics::GetIntersectionPoint(a.m_Point1, a.m_Point2, b.m_Point1, b.m_Point2, out);
}

bool Physics::IsIntersecting(Wall& a, Wall& b)
{
    return Physics::IsIntersecting(a.m_Point1, a.m_Point2, b.m_Point1, b.m_Point2);
}
bool Physics::IsIntersecting(const vec2& A, const vec2& B, const vec2& C, const vec2& D)
{
    // Define a lambda function for checking if three points are in counter-clockwise order
    auto ccw = [](const vec2& A, const vec2& B, const vec2& C) {
        return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
        };

    // Define a function to check if two line segments intersect
    return ccw(A, C, D) != ccw(B, C, D) && ccw(A, B, C) != ccw(A, B, D);
}

bool Physics::IsIntersecting_a(const vec2& p1, const vec2& p2, const vec2& p3, const vec2& p4)
{
    float denominator = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);

    // if lines are prarell
    if (denominator == 0)
    return false;

    //
    float t = ((p1.x - p3.x) * (p3.y - p4.y) - (p1.y - p3.y) * (p3.x - p4.x)) / denominator;
    float u = -((p1.x - p2.x) * (p1.y - p3.y) - (p1.y - p2.y) * (p1.x - p3.x)) / (denominator);

    // for intersection to fall within segment it needs 0<=u<=1 
    return  (0<=u && u<=1);
}

bool Physics::GetRaycast(const vec2& position, const vec2& direction, float length, const Wall& wall, vec2& out, float& distance)
{
    return Physics::GetRaycast(position, direction, length, wall.m_Point1, wall.m_Point2, out, distance);
}
bool Physics::GetRaycast(const vec2& position, const vec2& direction, float length, const Wall* wall, vec2& out, float& distance)
{
    return Physics::GetRaycast(position, direction, length, wall->m_Point1, wall->m_Point2, out, distance);
}
bool Physics::GetRaycast(const vec2& position, const vec2& direction, float length, const vec2& p3, const vec2& p4, vec2& out, float& distance)
{
    // Almost an identical copy of GetIntersectionPoint but calculates distance
    const vec2& p1 = position;
    const vec2 p2 = p1 + direction * length;

    float denominator = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);

    // if almost pararell
    if (fabs(denominator) < std::numeric_limits<float>::epsilon())
        return false;

    //
    float t = ((p1.x - p3.x) * (p3.y - p4.y) - (p1.y - p3.y) * (p3.x - p4.x)) / denominator;
    float u = -((p1.x - p2.x) * (p1.y - p3.y) - (p1.y - p2.y) * (p1.x - p3.x)) / (denominator);

    // if not 0<=t<=1 and 0<=u<=1 
    if (t < 0 || t > 1 || u < 0 || u > 1) return false;

    out.x = p1.x + t * (p2.x - p1.x);
    out.y = p1.y + t * (p2.y - p1.y);

    distance = t * length;

    return true;
}