#include "circle.h"
bool is_point_in_circle(const vec2& p, const circle& c) {
    return (squared_distance(p, c.center) < (c.radius * c.radius));
}
