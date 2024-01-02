module vector;

export vec2: struct = {
    x: f32;
    y: f32;
}

export squared_distance: func = (lhs: vec2, rhs: vec2) -> f32 {
    diff: vec2 = subtract(lhs, rhs);
    return squared_magnitude(diff);
}

subtract: func = (lhs: vec2, rhs: vec2) -> vec2 {
    result: vec2;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;

    return result;
}

squared_magnitude: func = (v: vec2) -> f32 {
    return v.x * v.x + v.y * v.y;
}