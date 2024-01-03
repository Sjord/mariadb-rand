from math import sqrt


def calcA(n):
    return int(
        (1 / 26)
        * (
            (39 - 9 * sqrt(13)) * ((1 / 2) * (5 - sqrt(13))) ** n
            + 3 * (13 + 3 * sqrt(13)) * ((1 / 2) * (5 + sqrt(13))) ** n
        )
    )


def calcB(n):
    return int(
        (1 / 26)
        * (
            (13 - 5 * sqrt(13)) * ((1 / 2) * (5 - sqrt(13))) ** n
            + (13 + 5 * sqrt(13)) * ((1 / 2) * (5 + sqrt(13))) ** n
        )
    )


def calcC(n):
    return int(
        (33 / 26)
        * (
            -(sqrt(13) - 13) * ((1 / 2) * (5 + sqrt(13))) ** n
            + (13 + sqrt(13)) * ((1 / 2) * (5 - sqrt(13))) ** n
            - 26
        )
    )


seed1 = 7
seed2 = 11
n = 10

a = calcA(n)
b = calcB(n)
c = calcC(n)
ai = (a * seed1 + b * seed2 + c) % 0x3FFFFFFF
print(f"seed1[{n}] = {a} * seed1[0] + {b} * seed2[0] + {c} = {ai} (mod 0x3FFFFFFF)")
