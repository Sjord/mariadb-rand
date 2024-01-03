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


relatistic seed:
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



seed1 = seed1 * 3 + seed2
seed2 = seed1 + seed2 + 33

a1 = 3 * a0 + b0
b1 = a1 + b0 + 33

a2 = 3 * a1 + b1
b2 = a2 + b1 + 33

a2 = 3 * 3 * a0 + b0 + 3 * a0 + b0 + b0 + 33
   = 12 * a0 + 3 * b0 + 33
b2 = a2 + 3 * a0 + b0 + b0 + 33 + 33
   = 15 * a0 + 5 * b0 + 99
