use std::time::Instant;

fn main() {
    const NUM_STEPS: u32 = 100_000_000;
    const WIDTH: f64 = 1.0 / (NUM_STEPS as f64);

    let start_time = Instant::now(); // monotonically increasing clock

    let sum: f64 = (0..NUM_STEPS)
        .map(|i| ((i as f64) + 0.5) * WIDTH)
        .map(|x| 1.0 / (1.0 + x * x))
        .sum();

    let pi = sum * 4.0 * WIDTH;

    let run_time = (start_time.elapsed().as_millis() as f64) / 1000.0;

    println!(
        "pi with {:} steps is {:} in {:} seconds",
        NUM_STEPS, pi, run_time
    );
}
