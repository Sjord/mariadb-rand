from collections import deque
from myrnd import Myrnd

seed1 = 7
seed2 = 11

m = Myrnd(seed1, seed2)

A = deque([2/3, 1], maxlen=2)
B = deque([-1/3, 0], maxlen=2)
C = deque([11, 0], maxlen=2)
for i in range(0, 100000):
    # a = (3 * A[-1] + sum(A)) % 0x3FFFFFFF
    a = int(5 * A[-1] - 3 * A[-2]) % 0x3FFFFFFF
    A.append(a)
    # b = (3 * B[-1] + sum(B) + 1) % 0x3FFFFFFF
    b = int(5 * B[-1] - 3 * B[-2])  % 0x3FFFFFFF
    B.append(b)
    # c = (3 * C[-1] + sum(C) + 33 * i) % 0x3FFFFFFF
    c = (5 * C[-1] - 3 * C[-2] + 33)  % 0x3FFFFFFF
    C.append(c)
    ai = (a * seed1 + b * seed2 + c) % 0x3FFFFFFF
    print(f"seed1[{i+1}] = {a} * seed1[0] + {b} * seed2[0] + {c} = {ai} (mod 0x3FFFFFFF)")

    m.myrnd()
    assert ai == m.seed1
