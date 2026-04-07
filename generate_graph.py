# generate_graph.py
import random

N = 10
M = 50
bounds = [random.randint(1, 5) for _ in range(N)]

print(f"{N}")
print(" ".join(map(str, bounds)))
for _ in range(M):
    u = random.randint(0, N - 1)
    v = random.randint(0, N - 1)
    print(f"{u} {v}")
