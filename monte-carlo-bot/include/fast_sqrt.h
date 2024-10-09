static inline double fast_sqrt(double x) {
  if (x == 0) return 0;
  double x0 = 4.0;
  x0 = (x0 + x / x0) / 2;
  x0 = (x0 + x / x0) / 2;
  x0 = (x0 + x / x0) / 2;
  x0 = (x0 + x / x0) / 2;
  return x0;
}