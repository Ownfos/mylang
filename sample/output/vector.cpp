#include "vector.h"
vec2 subtract(const vec2& lhs, const vec2& rhs) {
    vec2 result;
    (result.x = (lhs.x - rhs.x));
    (result.y = (lhs.y - rhs.y));
    return result;
}
float squared_magnitude(const vec2& v) {
    return ((v.x * v.x) + (v.y * v.y));
}
