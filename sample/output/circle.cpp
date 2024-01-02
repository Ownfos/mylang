#include "circle.h"
bool is_point_in_circle(const vec2& p, const circle& c) {
    vec2 diff = subtract(p, c.center);
    return (squared_magnitude(diff) < (c.radius * c.radius));
}
