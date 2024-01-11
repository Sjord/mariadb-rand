class Myrnd:
    max_value = 0x3FFFFFFF

    def __init__(self, seed1, seed2):
        self.seed1 = seed1
        self.seed2 = seed2

    def myrnd(self):
        self.seed1 = (self.seed1 * 3 + self.seed2) % self.max_value
        self.seed2 = (self.seed1 + self.seed2 + 33) % self.max_value
        return self.seed1 / self.max_value

    def reverse(self):
        seed1 = (
            795364314 * self.seed1 + 914668960 * self.seed2 + 238609305
        ) % self.max_value
        self.seed2 = (
            596523234 * self.seed1 + 954437177 * self.seed2 + 715827849
        ) % self.max_value
        self.seed1 = seed1
        return self.seed1 / self.max_value


if __name__ == "__main__":
    m = Myrnd(0, 0)
    for expected in [
        0,
        33,
        198,
        924,
        4059,
        17556,
        75636,
        325545,
        1400850,
        6027648,
        25935723,
        111595704,
        480171384,
        992328018,
        299900502,
        670002135,
        302825556,
        577863231,
        907097697,
        654415179,
        550782837,
        790668681,
        153511281,
        543034041,
        107152749,
        1054145301,
        654300999,
    ]:
        m.myrnd()
        assert m.seed1 == expected

    m = Myrnd(12345, 67890)
    assert m.myrnd() != m.myrnd()

    m = Myrnd(1073741790, 66)
    assert m.myrnd() == m.myrnd()

    m = Myrnd(172, 281)
    forward = [m.myrnd() for i in range(100)]
    m.myrnd()
    backward = [m.reverse() for i in range(100)]
    m.reverse()
    assert forward == list(reversed(backward))
    assert m.seed1 == 172, m.seed1
    assert m.seed2 == 281, m.seed2
