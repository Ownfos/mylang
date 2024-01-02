#ifndef MODULE_vector_H
#define MODULE_vector_H
#include <functional>
struct vec2 {
    float x;
    float y;
};
vec2 subtract(const vec2& lhs, const vec2& rhs);
float squared_magnitude(const vec2& v);
#endif // MODULE_vector_H
