module circle;
import export vector;

export circle: struct = {
    center: vec2;
    radius: f32;
}

export is_point_in_circle: func = (p: vec2, c: circle) -> bool {
    diff: vec2 = subtract(p, c.center);
    
    return squared_magnitude(diff) < c.radius * c.radius;
}