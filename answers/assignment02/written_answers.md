# Coding assignment
## Fix race condition bug on page 13 with a `std::mutex`.
My implementation uses the `std::mutex` together with a `std::lock_guard`.
The latter takes ownership of the mutex and locks it if possible (else it blocks).
This approach has the advantage that the mutex is unlocked and released automatically as the `lock_guard` goes out of scope after the pi summation statement.
