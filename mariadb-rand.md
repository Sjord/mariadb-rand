- item_create.cc https://github.com/MariaDB/server/blob/3fad2b115569864d8c1b7ea90ce92aa895cfef08/sql/item_create.cc#L6045
- Create_func_rand
- Create_func_rand::create_native https://github.com/MariaDB/server/blob/3fad2b115569864d8c1b7ea90ce92aa895cfef08/sql/item_create.cc#L5155
- Item_func_rand https://github.com/MariaDB/server/blob/3fad2b115569864d8c1b7ea90ce92aa895cfef08/sql/item_func.h#L2174
- Item_func_rand::seed_random https://github.com/MariaDB/server/blob/3fad2b115569864d8c1b7ea90ce92aa895cfef08/sql/item_func.cc#L2764
    - my_rnd_init https://github.com/MariaDB/server/blob/3fad2b115569864d8c1b7ea90ce92aa895cfef08/mysys/my_rnd.c#L28
    - 2 x 30 bits
- Item_func_rand::val_real
    - my_rnd

seed1 = seed1 * 3 + seed2
seed2 = seed1 + seed2 + 33

return (double) seed1


init with server time: https://github.com/MariaDB/server/blob/3fad2b115569864d8c1b7ea90ce92aa895cfef08/sql/mysqld.cc#L4890
overwritten by my_uuid_init on line 4901 (no, another instance)

next1 = prev1 * 3 + prev2
next2 = prev1 * 3 + prev2 + prev2 + 33

Alle uitkomsten deelbaar door 3?
select unique(mod(r * 1073741823, 3)) from (select rand() as r from stamspel_transacties) t;

x mod 9 = 0 of 6

Alle uitkomsten deelbaar door 33?
select unique(mod(r * 1073741823, 33)) from (select rand() as r from stamspel_transacties) t;

Hij valt hier niet in terug bij bepaalde seed, dus er zijn verschillende groepen
Is er een groep groter dan 33?
mod 99 komt er nooit 66 uit

Short loop:
959 30 120
959 93 174
959 156 228
959 219 282
959 282 336
959 345 390
959 408 444
959 471 498
959 534 552
959 597 606
959 660 660
959 723 714
959 786 768
959 849 822
959 912 876
959 975 930

0 0 0 0 33
116 682 5092 682 606063164
172 634 5035 634 71692586
239 167 4288 167 727155328
239 267 6399 267 16991136
239 367 8510 367 380568767
239 67 2177 67 363577697
419 1454 5512 1454 814219231
479 866 8053 866 223067428
49 1470 2568 1470 461755722
492 1258 6010 1258 363650864
519 1430 5203 1430 658036027
558 416 9218 416 601216207
569 1529 4078 1529 547735042
569 748 2072 748 273867554
604 148 7861 148 901418561
719 1385 6490 1385 127115164
719 676 3278 676 63557615
731 1078 2966 1078 470415263
77 121 2600 121 146013290
77 198 3867 198 219019902
77 275 5134 275 292026514
77 352 6401 352 365033126
77 429 7668 429 438039738
77 44 1333 44 73006678
77 506 8935 506 511046350
809 430 3812 430 654449846
809 893 7558 893 235157803
839 191 4543 191 972866257
839 415 9020 415 871990625
896 1320 1023 1320 159869259
899 921 7311 921 120017388

959: same 57/10000000
= 2^-17.4

77: same 1/10000000
239: same 2/10000000
101: same 0/10000000

expected: 2^-30 = 1e-9

nu 2438 kksnaps

768/0
384/884
0/1769
357913930/0
0/536870862
178956970/536870862
178956970/0

seeds voor eerste 10 oplopend:
768/0
384/884
0/1769
~1 promile

seed voor de eerste 10 aflopend:
357913930/0

ook rare seed:
0/536870862
178956970/536870862
178956970/0

order by rand:
2432 slecht
2400 superslecht

Minder slecht met random salt.

0b0011111111100010010100000110011100000001010010010100000110011100
                                           ^ altijd 1

seed:
my_rnd_init(&sql_rand,(ulong) server_start_time,(ulong) server_start_time/2);

init van uuid_rand:
    my_uuid_init((ulong) (my_rnd(&sql_rand))*12345,12345);
        my_rnd_init(&uuid_rand, (ulong) (seed1 + now), (ulong) (now/2+ getpid()));

create temporary table numbers (num integer);

does the client initialize random?

THD
my_rnd_init
with my_rnd(&sql_rand) * 0xffffffff
my_rnd_init(&rand, tmp + (ulong)((size_t) &rand), tmp + (ulong) ::global_query_id);

(ulong)((size_t) &rand) = position of rand pointer in memory?
always aligned? on 16-byte boundary?
could compromise ASLR

https://github.com/MariaDB/server/blob/3fad2b115569864d8c1b7ea90ce92aa895cfef08/sql/sql_class.cc#L889

my_rnd   = seed1  / 0x3FFFFFFF
THD seed = my_rnd * 0xffffffff
4 x 0x3FFFFFFF = 0xffffffff

THD: A thread context data structure created for each user connection.


subsequent connections
seed1 seed2
757185266 993062626
982664921 335153887
1041970646 950728705
570651941 367551421

Why isn't seed1 always divisible by 4?

Rand isn't seeded but just called many times?
in thd_create_random_password
thd->rand
20 calls to rnd
Yes this checks out
In password_authenticate? Generates a "scramble".

my_rnd_init
- init_server_components () at ./sql/mysqld.cc:4902
- my_uuid_init (seed1=0, seed2=<optimized out>) at ./mysys/my_uuid.c:117
- THD::THD (this=0x555557bc2f78, id=<optimized out>, is_wsrep_applier=<optimized out>) at ./sql/sql_class.cc:888
    - multipler times from THD::THD, one from create_background_thd, once from plugin_load
    - my_tz_init (org_thd=org_thd@entry=0x0, default_tzname=0x0, bootstrap=<optimized out>) at ./sql/tztime.cc:1622

Kan je de seeds van een thread veranderen en zo de scramble setten?

sql_rand is seeded with time, and this is used again to seed THD->rand
::global_query_id is 1 at startup
Does SHOW PROCESSLIST list the query id?


+---------------+-----------+
| Variable_name | Value     |
+---------------+-----------+
| rand_seed1    | 654415179 |
| rand_seed2    | 735020946 |
+---------------+-----------+

+---------------+-----------+
| Variable_name | Value     |
+---------------+-----------+
| rand_seed1    | 654415179 |
| rand_seed2    | 735020946 |
+---------------+-----------+

Can you set the RAND seeds for other users?
Seems like it.
https://mariadb.com/kb/en/server-system-variables/#rand_seed1

Can the PRNG run in reverse?
No
seed1 * 3 mod 0x3FFFFFFF has multiple solutions


show variables like 'rand_seed%';
set session rand_seed1=0, rand_seed2=0;

set session rand_seed1=147413137, rand_seed2=3572461763;

show variables like 'thread_stack';
The stack size for each thread

show variables like 'pseudo_thread_id';
pseudo_thread_id 25352706

security [AT] mariadb (dot) org

Op eigen DB:
maak nieuwe gebruiker
set de seeds op elke thread
vraag rand() op in applicatie


relatistic seed: realistic
seed1=140740522780686, seed2=4175270791


========= Summary results of SmallCrush =========

 Version:          TestU01 1.2.3
 Generator:        my_rnd
 Number of statistics:  15
 Total CPU time:   00:00:06.45
 The following tests gave p-values outside [0.001, 0.9990]:
 (eps  means a value < 1.0e-300):
 (eps1 means a value < 1.0e-15):

       Test                          p-value
 ----------------------------------------------
  1  BirthdaySpacings                 eps
  6  MaxOft                           eps
  6  MaxOft AD                      1 - 2.7e-13
 ----------------------------------------------
 All other tests were passed


Static points:

a = 33 (32537631 c_1 + 32537630) and 
b = 33 (32537631 c_2 + 2)
and c_1 element Z and c_2 element Z

a = 1073741790
b = 66

https://www.wolframalpha.com/input?i=system+equation+calculator&assumption=%7B%22F%22%2C+%22SolveSystemOf2EquationsCalculator%22%2C+%22equation1%22%7D+-%3E%222a%2Bb+mod+1073741823%22&assumption=%22FSelect%22+-%3E+%7B%7B%22SolveSystemOf2EquationsCalculator%22%7D%2C+%22dflt%22%7D&assumption=%7B%22F%22%2C+%22SolveSystemOf2EquationsCalculator%22%2C+%22equation2%22%7D+-%3E%22a%2B33+mod+1073741823%22
2a+b mod 1073741823
a+33 mod 1073741823


The database engines MySQL and MariaDB have a pseudo-random number generator (PRNG). This PRNG is not very good, in the sense that the numbers it generates are not randomly distributed. I want to write a blog post about this, and for this I need some more information and analysis of this PRNG. I would like to have some quantitive data about how it differs from a random distribution, how that affects real-world situations where this PRNG is used, and if possible an explanation for its behavior.

I will provide you with a Python implementation of the PRNG, the draft of my blog post so far, and some more notes I have. I have some examples of situations in which the PRNG behaves poorly.

It this something you can do?


Questions:
- When the global PRNG is initialized, seed1 is always twice seed2. Does this have consequences? Both these seeds are divisible by 33 every 66 seconds, right? If they were unrelated, would be expect them to be divisible by 33 with a change of 1/33^2, instead of 1/66?
- When the thread-specific PRNG is initialized, seed1 is incremented with a multiple of 8, and seed2 is incremented by 1. Does this have consequences?
- The PRNG has at most 30 bits of randomness. Is this ever a problem in real-world use?
- There are multiple groups modulo 99, depending on the seeds. Are some groups better than others? Is there a recommended property of good seed values? 
- I wrote "These groups range in size from 2 modulo 99 to 23 modulo 99", is this the right language?
- I wrote "losing between 5.6 and 2.1 bits of randomness", but which group the PRNG ends up in is also random, so is this accurate?
- Some seeds have pretty stable behaviour, such as low seed values. Does the PRNG ever end up in such a situation by itself? What portion of the seed value space is problematic?
- The PRNG is prone to repeating itself (generating the same output) after 960 steps. Why 960? Are there any other such values?
- The probability of repeating itself is still pretty low. Would this pose a problem in actual use?
- When picking a random row from a table with 2400 rows, the behaviour becomes less random. Why 2400? How much less random? How can I quantify how much this behavior differs from a random distribution? What are the implications of this?
- Are the numbers (especially the probabilities I mention) in my draft correct?
- Should seed1 and seed2 be relative prime for better random numbers?

# Formula's

0 (7, 11)
1 (32, 76)
2 (172, 281)
3 (797, 1111)
4 (3502, 4646)

seed1 = seed1 * 3 + seed2
seed2 = seed1 + seed2 + 33

a1 = 3 * a0 + b0
b1 = a1 + b0 + 33
   = 3 * a0 + 2 * b0 + 33

a2 = 3 * a1 + b1
b2 = a2 + b1 + 33

a2 = 3 * (3 * a0 + b0) + ((3 * a0 + b0) + b0 + 33)
   = 12 * a0 + 5 * b0 + 33
b2 = (12 * a0 + 5 * b0 + 33) + ((3 * a0 + b0) + b0 + 33) + 33
   = 15 * a0 + 7 * b0 + 99

a3 = 3 * a2 + b2
   = 3 * (12 * a0 + 5 * b0 + 33) + 15 * a0 + 7 * b0 + 99
   = 51 * a0 + 22 * b0 + 198
b3 = a3 + b2 + 33
   = (51 * a0 + 22 * b0 + 198) + (15 * a0 + 7 * b0 + 99) + 33
   = 66 * a0 + 29 * b0 + 330

a4 = 3 * (51 * a0 + 22 * b0 + 198) + (66 * a0 + 29 * b0 + 330)
   = 219 * a0 + 95 * b0 + 924
b4 = 219 * a0 + 95 * b0 + 924 + 66 * a0 + 29 * b0 + 330 + 33
   = 285 * a0 + 124 * b0 + 1287

a5 = 3 * 219 * a0 + 95 * b0 + 924 + 285 * a0 + 124 * b0 + 1287
   = 942 * a0 + ...

3, 12, 51, 219, 942

1: 3 = 3
2: 12 = 3 * 3 + 3
3: 51 = 3 * 12 + (12 + 3)
4: 219 = 3 * 51 + (51 + (12 + 3))
       = 3 * 51 + 51 + 51 - 3 * 12

An = 3 * An-1 + An-1 + An-2 + An-3 ... A0
   = 4 * An-1 + An-2 + An-3 ... A0
   = 5 * An-1 - 3 * An-2

219 = 5 * 51 - 3 * 12
 51 = 5 * 12 - 3 * 3
 12 = 5 * 3 - 3 * 1
  3 = 5 * 1 - 3 * 2/3

51 = 4 * 12 + 3
   = 4 * (4 * 3) + 3
   = 4 * 12 + 3
219 = 4 * 51 + 12 + 3
    = 4 * (4 * (4 * 3) + 3) + (4 * 3) + 3
    = 4 * 51 + 12 + 3
942 = 4 * (4 * (4 * (4 * 3) + 3) + (4 * 3) + 3) + (4 * (4 * 3) + 3) + (4 * 3) + 3
           ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    = 4 * 219 + 51 + 12 + 3
    = 4 * 219 + 219 - 3 * 51

n ^2 * 3
1 1    3
2 4    12
3 9    36
4 16   48
5 25   75

a(0) = 0
b(0) = 0
a(n) = 3 * a(n-1) + b(n-1)
b(n) = a(n) + b(n - 1) + 33

f(n) = 3 * f(n-1) + f(n-1) + f(n-2) + f(n-3) + ... + f(1) + f(0)

A(n) = 4 * A(n-1) + A(n-2) + A(n-3) + ... + A(0)

1
5 = 3 + 1 + 1
22 = 3 * 5 + 6 + 1
95 = 3 * 22 + 28 + 1
A(n) = 3 * A(n-1) + sum(A) + 1
A(n) = 5 * A(n-1) + 2 * A(n-2) - 1
95   = 3 * 22     + 22     - 2 * 5 - 1

95 = 5 * 22 - 3 * 5
22 = 5 * 5 - 3 * 1
5 = 5 * 1 - 3 * 0
1 = 5 * 0 - 3 * -1/3


0
33 = 33
198 = 3 * 33 + 33 + 33 + 33
924 = 3 * 198 + 198 + 33 + 33 + 33 + 33
4059 = 3 * 924 + 924 + 198 + 33 + 33 + 33 + 33 + 33

924 = 5 * 198 - 3 * 33 + 33
198 = 5 * 33 - 3 * 0 + 33
33 = 5 * 0 - 3 * 0 + 33
0 = 

 = 5 * 924 - 3 * 198 + 33

A(n) = 5A(n-1) - 3A(n-2)

## seed2

a
3
15 = 12 + 3
66 = 51 + 15
285 = 219 + 66

a = seed1[n] + seed2[n-1] = sum(seed1)

b
2
7 = 5 + 2
29 = 22 + 7
124 = 95 + 29

b = seed1[n] + seed2[n-1]

c
33
99 = 3 * 33
330 = 198 + 99 + 33
1287 = 924 + 330 + 33

# Repititions

seed1[960] = 
790318992 * seed1[0] + 887414990 * seed2[0] + 797133876 = 30 (mod 0x3FFFFFFF)
790318992 * seed1[0] + 887414990 * seed2[0] = 276607977 (mod 0x3FFFFFFF)

790318992 * seed1 + 887414990 * seed2 + 797133876 = seed1 (mod 0x3FFFFFFF)
790318991 * seed1 + 887414990 * seed2 + 797133876 = 0
887414990 * seed2 = -797133876 - 790318991 * seed1
790318991 * seed1 + 4774 * (185885 * seed2 + 166974) = 0

Repeat after 91199? (+1)
seed1[91200] = 332322915 * seed1[0] + 214460015 * seed2[0] + 33520641 = 304929732 (mod 0x3FFFFFFF)
91200 yes very much repetition

6467? (+1)
seed1[6468] = 441184836 * seed1[0] + 296541905 * seed2[0] + 356042940 = 858234585 (mod 0x3FFFFFFF)
Not really

1824?
no rep
seed1[1824] = 558034464 * seed1[0] + 292314407 * seed2[0] + 196128240 = 475283496 (mod 0x3FFFFFFF)

4800?
some rep

91200
332322915 * seed1[0] + 214460015 * seed2[0] + 33520641 = 304929732 (mod 0x3FFFFFFF)

536870911


a1 = a0 * 3 + b0
b1 = a1 + b0 + 33



## repeat 91200

332322915 * seed1 + 214460015 * seed2 + 33520641 = seed1 (mod 0x3FFFFFFF)
332322914 * seed1 + 214460015 * seed2 + 33520641 = 0 (mod 0x3FFFFFFF)
214460015 * i mod 0x3FFFFFFF generates only 2979 different values
gcd(214460015, 0x3FFFFFFF) = 360437
gcd(332322914, 0x3FFFFFFF) = 360437
0x3FFFFFFF / 360437 = 2979
it generates 0 in 1/2979 cases
it generates 1040221182 = -33520641

332322914 * seed1 + 214460015 * seed2 + 33520641 = 0 (mod 0x3FFFFFFF)
360437 * (922 * seed1 + 595 seed2 + 93) = 0 (mod 0x3FFFFFFF)
922 * seed1 + 595 seed2 + 93 = 0 (mod 2979)
this has 2979 solutions for seed1, seed2 between 0 and 2979
so chance of repetition is 1/2979
= 335 in 1e6
repeat.py confirms approximately

## repeat 960

repeat 960:
gcd = 2387
0x3FFFFFFF / 2387 = 449829
probability: 1/449829


seed1[960] = 790318992 * seed1[0] + 887414990 * seed2[0] + 797133876 = 1058546188 (mod 0x3FFFFFFF)
790318992 * seed1 + 887414990 * seed2 + 797133876 = seed1 (mod 0x3FFFFFFF)
790318991 * seed1 + 887414990 * seed2 + 797133876 = 0 (mod 0x3FFFFFFF)
2387 * (331093 * seed1 + 371770 * seed2 + 333948) = 0 (mod 0x3FFFFFFF)

## repeat 4800

also 2387

# ditribution

➜  mariadb cat lowest_check.txt| sort -n | uniq -c | sort -n | awk '{print $1}' | uniq -c | sort -nk2 > prob_check.txt
➜  mariadb cat lowest.txt| sort -n | uniq -c | sort -n | awk '{print $1}' | uniq -c | sort -nk2 > prob.txt

when reseeding, no skewed distribution

# Modulo

% 0x3FFFFFFF is probably a mistake
& 0x3FFFFFFF would make sense, % 0x40000000 would make sense.

# Period

seed1 = seed1 * 3 + seed2
seed2 = seed1 + seed2 + 33

f(n)=(1/26) * ((39 - 9 * sqrt(13)) * ((1/2) * (5 - sqrt(13)))^n + 3 * (13 + 3 * sqrt(13)) * ((1/2) * (5 + sqrt(13)))^n) (mod 1073741823)
f(n)=(1/26)*((39-9*sqrt(13))*((1/2)*(5-sqrt(13)))^n+3*(13+3*sqrt(13))*((1/2)*(5+sqrt(13)))^n)

f(-1) = 1

https://stackoverflow.com/questions/15766420/mysql-rand-how-often-can-it-be-used-does-it-use-dev-random


((1 / 26)
        * (
            (39 - 9 * sqrt(13)) * ((1 / 2) * (5 - sqrt(13))) ** n
            + 3 * (13 + 3 * sqrt(13)) * ((1 / 2) * (5 + sqrt(13))) ** n
        ) % 27917287398)
