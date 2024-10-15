import random
import math


selects = [0, 0, 0, 0, 0, 0]
weights = [1, 2, 3, 1, 0, 1]

for _ in range(100000):
    tw = 0
    s = 0
    for i, w in enumerate(weights):
        r = random.random() * (tw + w)
        if r > tw:
            s = i
        tw += w

    selects[s] += 1

ts = sum(selects)
print([round((s / ts) * sum(weights), 2) for s in selects])


def fast_sqrt(x: float) -> float:
    if x < 8:
        x0: float = 4.0
    else:
        x0: float = 16.0
    x0 = x0 - (x0 * x0 - x) / (2 * x0)
    x0 = x0 - (x0 * x0 - x) / (2 * x0)
    x0 = x0 - (x0 * x0 - x) / (2 * x0)
    return x0


for i in range(1000):
    print(fast_sqrt(i), math.sqrt(i))