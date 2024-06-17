

#define min(a, b) ((a) < (b) ? (a) : (b))

#define max(a, b) ((a) > (b) ? (a) : (b))

#define swap(a, b) \
  { (a) ^= (b), (b) ^= (a), (a) ^= (b); }

#define abs(x) ((x) >= 0 ? (x) : -(x))

#define sgn(x) (((x) > 0) - ((x) < 0))

#define lengthof(list) (sizeof(list) / sizeof(list[0]))