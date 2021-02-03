pub struct Point {
    x: f32,
    y: f32,
    z: f32,
}

pub fn aos_distances(a_src: &[Point], b_src: &[Point], squared_distances: &mut [f32]) {
    let result = a_src
        .iter()
        .zip(b_src.iter())
        .map(|(a, b)| (a.x - b.x, a.y - b.y, a.z - b.z))
        .map(|(dx, dy, dz)| dx * dx + dy * dy + dz * dz)
        .zip(squared_distances.iter_mut())
        .for_each(|(res, dst)| *dst = res);
}
