from collections import deque
from myrnd import Myrnd

seed1 = 7
seed2 = 11

m = Myrnd(seed1, seed2)

A1 = deque([2/3, 1], maxlen=2)
B1 = deque([-1/3, 0], maxlen=2)
C1 = deque([11, 0], maxlen=2)

a2 = 0
b2 = 1
c2 = 0

for i in range(0, 32537631):
    # a = (3 * A[-1] + sum(A)) % 0x3FFFFFFF
    a1 = int(5 * A1[-1] - 3 * A1[-2]) % 0x3FFFFFFF
    a2 = (a1 + a2) % 0x3FFFFFFF
    A1.append(a1)

    # b = (3 * B[-1] + sum(B) + 1) % 0x3FFFFFFF
    b1 = int(5 * B1[-1] - 3 * B1[-2]) % 0x3FFFFFFF
    b2 = (b1 + b2) % 0x3FFFFFFF
    B1.append(b1)

    # c = (3 * C[-1] + sum(C) + 33 * i) % 0x3FFFFFFF
    c1 = (5 * C1[-1] - 3 * C1[-2] + 33) % 0x3FFFFFFF
    c2 = (c1 + c2 + 33) % 0x3FFFFFFF
    C1.append(c1)

    ai1 = (a1 * seed1 + b1 * seed2 + c1) % 0x3FFFFFFF
    ai2 = (a2 * seed1 + b2 * seed2 + c2) % 0x3FFFFFFF
    
    print(f"seed1[{i+1}] = {a1} * seed1[0] + {b1} * seed2[0] + {c1} = {ai1} (mod 0x3FFFFFFF)")
    print(f"seed2[{i+1}] = {a2} * seed1[0] + {b2} * seed2[0] + {c2} = {ai2} (mod 0x3FFFFFFF)")

    m.myrnd()
    assert ai1 == m.seed1
    assert ai2 == m.seed2
