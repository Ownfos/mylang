#ifndef MODULE_circle_H
#define MODULE_circle_H
#include <functional>
#include "vector.h"
struct circle {
    vec2 center;
    float radius;
};
bool is_point_in_circle(const vec2& p, const circle& c);
#endif // MODULE_circle_H
