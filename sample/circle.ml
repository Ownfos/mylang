module circle;
import export vector;

export circle: struct = {
    center: vec2;
    radius: f32;
}

export is_point_in_circle: func = (p: vec2, c: circle) -> bool {
    return squared_distance(p, c.center) < c.radius * c.radius;
}