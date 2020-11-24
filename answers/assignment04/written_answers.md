# Coding warmup
## Speed up merge sort on page 7.
initially:
```
naive: 37.7612 seconds
std::sort: 62.803 seconds
```

with task (regardless of the size in if)
```
tasked: 22.3237 seconds
std::sort: 64.4527 seconds
```

with insertion sort for n < 32 (similar for 16 and 64)
```
tasked: 18.464 seconds
std::sort: 64.3987 seconds
```
