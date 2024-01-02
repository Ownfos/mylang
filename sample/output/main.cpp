#include "main.h"
#include "circle.h"
int main();
int main() {
    circle c;
    (c.center.x = 1);
    (c.center.y = 2);
    (c.radius = 5);
    vec2 p;
    (p.x = 2);
    (p.y = 3);
    if (is_point_in_circle(p, c)) {
        return 1;
    }
    else {
        return 0;
    }
}
