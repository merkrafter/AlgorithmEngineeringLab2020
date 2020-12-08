use rand::prelude::*;
use std::time::Instant;

fn get_random_int_vector(n: u32) -> Vec<u32> {
    let mut rng = rand::thread_rng();
    (0..n).map(|_| rng.gen()).collect()
}

/// Copies all elements from a and b into c, sorted in ascending order.
#[inline]
fn merge(a: &[u32], b: &[u32], c: &mut [u32]) {
    let mut a_idx = 0;
    let mut b_idx = 0;
    let mut c_idx = 0;
    while a_idx < a.len() && b_idx < b.len() {
        if a[a_idx] < b[b_idx] {
            c[c_idx] = a[a_idx];
            a_idx += 1;
        } else {
            c[c_idx] = b[b_idx];
            b_idx += 1;
        }
        c_idx += 1;
    }
    if a_idx < a.len() {
        c[c_idx..].copy_from_slice(&a[a_idx..]);
    }
    if b_idx < b.len() {
        c[c_idx..].copy_from_slice(&b[b_idx..]);
    }
}

/// Sorts the in_arr into the out_arr in ascending order
fn insertion_sort(in_arr: &mut [u32], out_arr: &mut [u32]) {
    for i in 0..in_arr.len() {
        let curr_val = in_arr[i];
        let mut j = i;
        while j > 0 && out_arr[j - 1] > curr_val {
            out_arr[j] = out_arr[j - 1];
            in_arr[j] = in_arr[j - 1];
            j -= 1;
        }
        out_arr[j] = curr_val;
        in_arr[j] = curr_val;
    }
}

fn merge_sort_tasked_prealloc(mut arr: &mut [u32], mut buffer: &mut [u32]) {
    if arr.len() > 1 {
        if arr.len() < 32 {
            insertion_sort(&mut arr, &mut buffer);
            return;
        }
        let middle_idx = arr.len() / 2;
        let (buffer_low, buffer_high) = buffer.split_at_mut(middle_idx);
        let (arr_low, arr_high) = arr.split_at_mut(middle_idx);
        merge_sort_tasked_prealloc(buffer_low, arr_low);
        merge_sort_tasked_prealloc(buffer_high, arr_high);

        merge(&arr[..middle_idx], &arr[middle_idx..], &mut buffer);
    }
}

fn merge_sort(mut a: &mut Vec<u32>) {
    let mut buffer = vec![0; a.len()];
    merge_sort_tasked_prealloc(&mut a, &mut buffer);
    a.copy_from_slice(&buffer);
}

fn main() {
    const N: u32 = 100_000_000;
    let mut v = get_random_int_vector(N);
    let mut v_copy = v.clone();

    let start = Instant::now();
    merge_sort(&mut v);
    println!(
        "naive: {:} seconds",
        (start.elapsed().as_millis() as f64) / 1000.0
    );
    let start = Instant::now();
    v_copy.sort();
    println!(
        "Vec::sort: {:} seconds",
        (start.elapsed().as_millis() as f64) / 1000.0
    );

    if v != v_copy {
        println!("sort implementation is buggy");
    }
}
