module main;
import circle;

main: func = () -> i32 {
    c: circle;
    c.center.x = 1;
    c.center.y = 2;
    c.radius = 5;

    p: vec2;
    p.x = 2;
    p.y = 3;

    if (is_point_in_circle(p, c)) {
        return 1;
    }
    else {
        return 0;
    }
}