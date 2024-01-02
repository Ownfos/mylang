#include "vector.h"
vec2 subtract(const vec2& lhs, const vec2& rhs);
float squared_magnitude(const vec2& v);
float squared_distance(const vec2& lhs, const vec2& rhs) {
    vec2 diff = subtract(lhs, rhs);
    return squared_magnitude(diff);
}
vec2 subtract(const vec2& lhs, const vec2& rhs) {
    vec2 result;
    (result.x = (lhs.x - rhs.x));
    (result.y = (lhs.y - rhs.y));
    return result;
}
float squared_magnitude(const vec2& v) {
    return ((v.x * v.x) + (v.y * v.y));
}
