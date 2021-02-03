struct Point {
  float x, y, z;
};

// can not utilize auto-vectorization
void aos_distances(const Point *__restrict__ a, const Point *__restrict__ b,
                   float *squared_distances, const int size) {
  for (int i = 0; i < size; ++i) {
    float dx = a[i].x - b[i].x;
    float dy = a[i].y - b[i].y;
    float dz = a[i].z - b[i].z;
    squared_distances[i] = dx * dx + dy * dy + dz * dz;
  }
}

struct Point_soa {
  float *x, *y, *z;
};

// does utilize auto-vectorization
void soa_distances(const Point_soa a, const Point_soa b,
                   float *squared_distances, const int size) {
  for (int i = 0; i < size; ++i) {
    float dx = a.x[i] - b.x[i];
    float dy = a.y[i] - b.y[i];
    float dz = a.z[i] - b.z[i];
    squared_distances[i] = dx * dx + dy * dy + dz * dz;
  }
}
