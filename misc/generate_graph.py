import sys
import random


def generate(n, m, filename="input.txt"):
    with open(filename, "w") as f:
        f.write(f"{n}\n")

        bounds = [random.randint(1, (m // n) * 3) for _ in range(n)]
        f.write(" ".join(map(str, bounds)) + "\n")

        for _ in range(m):
            u = random.randint(0, n - 1)
            v = random.randint(0, n - 1)
            f.write(f"{u} {v}\n")


if __name__ == "__main__":
    # default: 100k vertices, 5 million edges
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 100000
    m = int(sys.argv[2]) if len(sys.argv) > 2 else 5000000
    generate(n, m)
    print(f"Generated {n} vertices and {m} edges in input.txt")
