"""
Python equivalent of BuLang hotspots benchmark.
"""
import time

N_RET = 5_000_000
N_STR = 500_000
N_MAP = 200_000
N_ARR = 40_000
N_OBJ = 40_000


def inc(x):
    return x + 1


def fwd_inc(x):
    return inc(x)


def pair(x):
    return x, x + 1


def bench_return_scalar(n):
    s = 0
    i = 0
    while i < n:
        s = s + inc(i)
        i = i + 1
    return s


def bench_return_forward_single(n):
    s = 0
    i = 0
    while i < n:
        s = s + fwd_inc(i)
        i = i + 1
    return s


def bench_multi_return_unpack(n):
    s = 0
    i = 0
    while i < n:
        a, b = pair(i)
        s = s + a + b
        i = i + 1
    return s


def bench_concat_plus(n):
    s = ""
    i = 0
    while i < n:
        s = "Points " + str(i)
        i = i + 1
    return len(s)


def bench_fstring(n):
    s = ""
    i = 0
    while i < n:
        s = f"Points {i}"
        i = i + 1
    return len(s)


def bench_format(n):
    s = ""
    i = 0
    while i < n:
        s = "Points {}".format(i)
        i = i + 1
    return len(s)


def bench_map_string_keys(n):
    keys = []
    m = {}
    i = 0
    while i < n:
        k = f"k{i}"
        keys.append(k)
        m[k] = i
        i = i + 1

    s = 0
    i = 0
    while i < n:
        s = s + m[keys[i]]
        i = i + 1
    return s

class BenchPoint:
    def __init__(self, x, y):
        self.x = x
        self.y = y


def bench_array_index_rw(n):
    arr = []
    i = 0
    while i < n:
        arr.append(i)
        i = i + 1

    s = 0
    i = 0
    while i < n:
        v = arr[i]
        arr[i] = v + 1
        s = s + arr[i]
        i = i + 1
    return s


def bench_class_array_index(n):
    arr = []
    i = 0
    while i < n:
        arr.append(BenchPoint(i, i + 1))
        i = i + 1

    s = 0
    i = 0
    while i < n:
        p = arr[i]
        s = s + p.x + p.y
        p.x = p.x + 1
        i = i + 1
    return s


print("=== Python Hotspots Benchmark ===")
print(f"N_RET={N_RET} N_STR={N_STR} N_MAP={N_MAP} N_ARR={N_ARR} N_OBJ={N_OBJ}")
print()

t0 = time.perf_counter()
r1 = bench_return_scalar(N_RET)
t1 = time.perf_counter()
print(f"1. Return scalar:         {t1 - t0:.4f} s")

t2 = time.perf_counter()
r2 = bench_return_forward_single(N_RET)
t3 = time.perf_counter()
print(f"2. Return forward 1-val:  {t3 - t2:.4f} s")

t4 = time.perf_counter()
r3 = bench_multi_return_unpack(N_RET)
t5 = time.perf_counter()
print(f"3. Multi-return unpack:   {t5 - t4:.4f} s")

t6 = time.perf_counter()
r4 = bench_concat_plus(N_STR)
t7 = time.perf_counter()
print(f"4. String +:              {t7 - t6:.4f} s")

t8 = time.perf_counter()
r5 = bench_fstring(N_STR)
t9 = time.perf_counter()
print(f"5. fstring:               {t9 - t8:.4f} s")

t10 = time.perf_counter()
r6 = bench_format(N_STR)
t11 = time.perf_counter()
print(f"6. format():              {t11 - t10:.4f} s")

t12 = time.perf_counter()
r7 = bench_map_string_keys(N_MAP)
t13 = time.perf_counter()
print(f"7. Map string keys:       {t13 - t12:.4f} s")

t14 = time.perf_counter()
r8 = bench_array_index_rw(N_ARR)
t15 = time.perf_counter()
print(f"8. Array index R/W:       {t15 - t14:.4f} s")

t16 = time.perf_counter()
r9 = bench_class_array_index(N_OBJ)
t17 = time.perf_counter()
print(f"9. Class array index:     {t17 - t16:.4f} s")

print()
total = (t1 - t0) + (t3 - t2) + (t5 - t4) + (t7 - t6) + (t9 - t8) + (t11 - t10) + (t13 - t12) + (t15 - t14) + (t17 - t16)
print(f"TOTAL:                    {total:.4f} s")
print(f"checksum: {r1 + r2 + r3 + r4 + r5 + r6 + r7 + r8 + r9}")
