// https://stackoverflow.com/questions/26237419/faster-than-rand

static unsigned int g_seed;

// Used to seed the generator.           
static inline void fast_srand(int seed) {
    g_seed = seed;
}

// Compute a pseudorandom integer.
// Output value in range [0, 32767]
static inline int fast_rand(void) {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}