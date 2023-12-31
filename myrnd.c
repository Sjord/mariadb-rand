// export LD_LIBRARY_PATH=/usr/local/lib
// gcc -o myrnd myrnd.c -ltestu01

#include <sys/random.h>
#include <stdio.h>
#include "unif01.h"
#include "bbattery.h"
#include <time.h>
#include <stdlib.h>

typedef unsigned long	ulong;		  /* Short for unsigned long */

struct my_rnd_struct {
  unsigned long seed1,seed2,max_value;
  double max_value_dbl;
};

void my_rnd_init(struct my_rnd_struct *rand_st, ulong seed1, ulong seed2);
double my_rnd(struct my_rnd_struct *rand_st);

void my_rnd_init(struct my_rnd_struct *rand_st, ulong seed1, ulong seed2)
{
#ifdef HAVE_valgrind
  bzero((char*) rand_st,sizeof(*rand_st));      /* Avoid UMC varnings */
#endif
  rand_st->max_value= 0x3FFFFFFFL;
  rand_st->max_value_dbl=(double) rand_st->max_value;
  rand_st->seed1=seed1%rand_st->max_value ;
  rand_st->seed2=seed2%rand_st->max_value;
}

double my_rnd(struct my_rnd_struct *rand_st)
{
  unsigned long seed1;
  seed1= (rand_st->seed1*3+rand_st->seed2) % rand_st->max_value;
  rand_st->seed2=(seed1+rand_st->seed2+33) % rand_st->max_value;
  rand_st->seed1= seed1;
  return (((double) seed1)/rand_st->max_value_dbl);
}

double my_rnd_rev(struct my_rnd_struct *rand_st)
{
    rand_st->seed2 = (rand_st->seed1 - rand_st->seed2 - 33) % rand_st->max_value;
    rand_st->seed1 = (rand_st->seed1 - rand_st->seed2) / 3 % rand_st->max_value;
    return (((double) rand_st->seed1)/rand_st->max_value_dbl);
}


static struct my_rnd_struct randstruct;

double my_rnd_void() {
    // return (double)(rand() & 0x3FFFFFFFL) / 0x3FFFFFFFL;
    return my_rnd(&randstruct);
}

ulong actual_random() {
    ulong r;
    getrandom(&r, sizeof(r), 0);
    return r;
}

int main_repeat() {
    double r, r2;
    int same = 0;
    // my_rnd_init(&randstruct, 357913932, 57);
    for (int i = 0; i < 10000000; i++) {
        my_rnd_init(&randstruct, actual_random(), actual_random());
        // printf("seeds: %ld %ld\n", randstruct.seed1, randstruct.seed2);
        r = my_rnd(&randstruct);
        // printf("seeds: %ld %ld\n", randstruct.seed1, randstruct.seed2);

        // printf("%.10f\n", r);
        for (int i = 0; i < 2399; i++) {
            my_rnd(&randstruct);
        }
        r2 = my_rnd(&randstruct);
        // printf("%.10f\n", r2);
        if (r == r2) {
            same += 1;
            printf("same %d %d\n", same, i);
        }
    }
    // printf("same %d %d\n", same, i);
}

void printbinary(ulong* num) {
    printf("0b");
    for (int i = 0; i < 64; i++) {
        printf("%d", *num >> (63 - i) & 1);
    }
    printf("\n");
}

int main62() {
    ulong l;
    double r;

    my_rnd_init(&randstruct, 0, 0);
    for (int j =0; j < 10000; j++) {
        r = my_rnd_void();
        l = r * 0x3FFFFFFFL;
        printf("%d\n", l % 99);
    }
}

int main() {
    my_rnd_init(&randstruct, 757185266, 993062626);
    for (int i = 0; i < 21; i++) {
        my_rnd_void();
        printf("%lu %lu\n", randstruct.seed1, randstruct.seed2);
    }
}

int mainz() {
    int group = 993;
    ulong l;
    double r;
    double lowest_r = 1.0;
    int lowest_i = 0;

    for (int j =0; j < 100000; j++) {
        my_rnd_init(&randstruct, actual_random() * 33, actual_random() * 33);
        lowest_r = 1.0;
        lowest_i = 0;
        for (int i = 0; i < 2400; i++) {
            r = my_rnd_void();
            l = r * 0x3FFFFFFFL;
            // printbinary(&r);
            // printf("%d %.10f %lu %d\n", i, r, l, l % group);
            if (r < lowest_r) {
                lowest_r = r;
                lowest_i = i;
            }
        }
        l = lowest_r * 0x3FFFFFFFL;
        printf("lowest: %d %.10f %lu %d\n", lowest_i, lowest_r, l, l % group);
    }
}

int main5() {
    ulong seed1 = 0;
    ulong seed2 = 1;
    for (seed1 = 0; seed1 < 10000; seed1++) {
        for (seed2 = 0; seed2 < 10000; seed2++) {
            my_rnd_init(&randstruct, seed1, seed2);
            double r;
            unsigned long l;
            for (int i = 0; i < 2438; i++) {
                r = my_rnd(&randstruct);
                if (seed1 == randstruct.seed1) {
                    printf("%d %ld %ld %ld %ld\n", i, seed1, seed2, randstruct.seed1, randstruct.seed2);
                    break;
                }
            }
        }
    }
}

int main4() {
    my_rnd_init(&randstruct, 30, 120);
    int num = 959;
    double r;
    unsigned long l;
    for (int i = 0; i < num; i++) {
        // r = my_rnd(&randstruct);
        r = my_rnd_void();
        // l = r * 0x3FFFFFFFL;
        l = r * num;
        if (i == 0) 
        printf("%d %.10f %lu %d\n", i, r, l, l % 33);
    }
    for (int i = 0; i < num; i++) {
        // r = my_rnd(&randstruct);
        r = my_rnd_void();
        // l = r * 0x3FFFFFFFL;
        l = r * num;
        if (i == 0) 
        printf("%d %.10f %lu %d\n", i, r, l, l % 33);
    }
}

int main3() {
    my_rnd_init(&randstruct, 12345, 67890);

    unif01_Gen *gen;
    gen = unif01_CreateExternGen01 ("my_rnd", my_rnd_void);
    bbattery_SmallCrush (gen);
    // bbattery_pseudoDIEHARD(gen);
    // bbattery_FIPS_140_2(gen);
    unif01_DeleteExternGen01 (gen);
}
