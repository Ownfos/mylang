#ifndef MODULE_vector_H
#define MODULE_vector_H
#include <functional>
struct vec2 {
    float x;
    float y;
};
float squared_distance(const vec2& lhs, const vec2& rhs);
#endif // MODULE_vector_H
