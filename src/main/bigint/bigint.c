#include "bigint.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

Bigint* init() {
    Bigint* bigint = (Bigint *) malloc(sizeof(Bigint));
    if (bigint == NULL) {
        return NULL;
    }

    bigint->numbers = (unsigned int *) malloc(sizeof(unsigned int) * 2);

    return bigint;
}

void delete(Bigint* number) {
    if (number == NULL) {
        return;
    }

    if (number->numbers != NULL) {
        free(number->numbers);
    }

    free(number);
}

int inputBigint(Bigint* number) {
    if (number == NULL) {
        return 1;
    }

    char buffer[64];
    char* end_pointer;

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return 1;
    }

    errno = 0;
    unsigned long long k = strtoull(buffer, &end_pointer, 10);
    if (end_pointer == buffer || errno == ERANGE || k == 0) {
        return 1;
    }

    unsigned int *tmp = (unsigned int *) realloc(number->numbers, sizeof(unsigned int) * (k + 1));
    if (tmp == NULL) {
        return 1;
    }

    number->numbers = tmp;
    number->numbers[0] = (unsigned int) k;

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return 1;
    }

    errno = 0;
    const long long firstNumber = strtoll(buffer, &end_pointer, 10);
    if (end_pointer == buffer || errno == ERANGE || firstNumber > (long long) BASE) {
        return 1;
    }

    number->firstDigit = (int) firstNumber;
    k--;

    // We assure that a user will enter a bigint number in little endian format
    for (unsigned long long i = 1; i <= k; i++) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 1;
        }

        errno = 0;
        const unsigned long long digit = strtoull(buffer, &end_pointer, 10);
        if (end_pointer == buffer || errno == ERANGE || digit > BASE) {
            return 1;
        }

        number->numbers[i] = digit;
    }

    return 0;
}

/**
 * It prepares two arguments for any operation to be executed.
 *
 * Generally, it makes two arguments be the same maximum size,
 * and it transfers firstDigits to arrays of numbers (for convenience).
 *
 * @return Returns 1 if executing of this function fails, 0 if it executes correctly.
 */
static int prepare(Bigint* a, Bigint* b, unsigned int* maximum) {
    unsigned int max;
    if (a->numbers[0] >= b->numbers[0]) {
        max = a->numbers[0];
    } else {
        max = b->numbers[0];
    }

    unsigned int* tmp1 = realloc(a->numbers, sizeof(unsigned int) * (2 * max + 4));
    if (tmp1 == NULL) {
        return 1;
    }

    a->numbers = tmp1;
    a->numbers[a->numbers[0]] = a->firstDigit;

    unsigned int* tmp2 = realloc(b->numbers, sizeof(unsigned int) * (2 * max + 4));
    if (tmp2 == NULL) {
        return 1;
    }

    b->numbers = tmp2;
    b->numbers[b->numbers[0]] = b->firstDigit;

    for (unsigned int i = a->numbers[0] + 1; i <= 2 * max + 2; i++) {
        a->numbers[i] = 0;
    }

    for (unsigned int i = b->numbers[0] + 1; i <= 2 * max + 2; i++) {
        b->numbers[i] = 0;
    }

    a->numbers[0] = max;
    b->numbers[0] = max;

    *maximum = max;
    return 0;
}

Bigint* summation(Bigint* a, Bigint* b) {
    if (a == NULL || b == NULL) {
        return NULL;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return NULL;
    }

    if (a->firstDigit == 0 && b->firstDigit == 0) {
        return a;
    }

    if (a->firstDigit <= 0 && b->firstDigit <= 0) {
        a->firstDigit = a->firstDigit * (-1);
        b->firstDigit = b->firstDigit * (-1);

        Bigint* new_a = summation(deep_copy(a), b);
        if (new_a == NULL) {
            return NULL;
        }

        a->firstDigit = new_a->firstDigit * (-1);
        free(a->numbers);
        a->numbers = new_a->numbers;

        return a;
    }

    if (a->firstDigit >= 0 && b->firstDigit <= 0) {
        b->firstDigit = b->firstDigit * (-1);
        return substraction(a, b);
    }

    if (a->firstDigit <= 0 && b->firstDigit >= 0) {
        a->firstDigit = a->firstDigit * (-1);
        return substraction(b, a);
    }

    unsigned int max;
    int try = prepare(a, b, &max);
    if (try == 1) {
        return NULL;
    }

    unsigned int shift = 0;
    for (unsigned int i = 1; i <= max; i++) {
        unsigned int digit1 = a->numbers[i];
        unsigned int digit2 = b->numbers[i];

        unsigned int digit1_initial = digit1;
        digit1 = digit1 + digit2 + shift;
        a->numbers[i] = digit1 % BASE;

        if (digit1_initial >= digit1) {
            shift = digit1 / BASE;
        } else {
            shift = 0;
        }
    }

    if (shift > 0) {
        a->numbers[max + 1] = shift;
        a->numbers[0]++;
    } else {
        a->numbers[0] = max;
    }

    a->firstDigit = (int) a->numbers[a->numbers[0]];
    a->numbers[a->numbers[0]] = 0;

    return a;
}

Bigint* substraction(Bigint* a, Bigint* b) {
    if (a == NULL || b == NULL) {
        return NULL;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return NULL;
    }

    if (a->firstDigit == 0 && b->firstDigit == 0) {
        return a;
    }

    if (a->firstDigit < 0 && b->firstDigit <= 0) {
        // -a - (-b) = b - a
        a->firstDigit = a->firstDigit * (-1);
        b->firstDigit = b->firstDigit * (-1);

        Bigint* new_a = substraction(deep_copy(b), a);
        if (new_a == NULL) {
            return NULL;
        }

        a->firstDigit = new_a->firstDigit;
        free(a->numbers);
        a->numbers = new_a->numbers;
        new_a->numbers = NULL;
        delete(new_a);
        return a;
    }

    if ((a->firstDigit >= 0 && b->firstDigit < 0) || (a->firstDigit <= 0 && b->firstDigit > 0)) {
        // a - (-b) = a + b
        // -a - b = (a + b) * (-1). Sign changing is executed in summation function
        b->firstDigit = -b->firstDigit;
        return summation(a, b);
    }

    unsigned int max;
    int try = prepare(a, b, &max);
    if (try == 1) {
        return NULL;
    }

    // Compare magnitudes from most significant digit to find which is larger
    int cmp = 0;
    if (a->numbers[a->numbers[0]] > b->numbers[b->numbers[0]]) {
        cmp = 1;
    } else if (a->numbers[a->numbers[0]] < b->numbers[b->numbers[0]]) {
        cmp = -1;
    } else {
        a->firstDigit = 0;
        a->numbers[0] = 1;
        return a;
    }

    // a < b
    if (cmp == -1) {
        unsigned int* tmp = a->numbers;
        a->numbers = b->numbers;
        b->numbers = tmp;
    }

    long long borrow = 0;
    for (unsigned int i = 1; i <= max; i++) {
        long long digit1 = a->numbers[i];
        long long digit2 = b->numbers[i];

        long long diff = digit1 - digit2 - borrow;

        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }

        a->numbers[i] = (unsigned int) diff;
    }

    while (max > 1 && a->numbers[max] == 0) {
        max--;
    }

    a->numbers[0] = max;
    a->firstDigit = (int) a->numbers[max];
    a->numbers[max] = 0;

    if (cmp == -1) {
        a->firstDigit = -a->firstDigit;
    }

    return a;
}

/**
 * @return The least significant half of an argument.
 * Списал с семинара :D
 */
static unsigned int low_word(unsigned int value) {
    return value & ((1 << (sizeof(unsigned int) << 2)) - 1);
}

/**
 * @return The most significant half of an argument.
 * Это тоже списал :D
 */
static unsigned int high_word(unsigned int value) {
    return (value >> (sizeof(unsigned int) << 2)) & ((1 << (sizeof(unsigned int) << 2)) - 1);
}

/**
 * It multiplies two unsigned int numbers and write the result in low and high part.
 * Splits each operand into two 16-bit halves, then combines four 16x16-bit products.
 * a*b = p3*2^32 + (p1+p2)*2^16 + p0
 */
static void mul_word(unsigned int a, unsigned int b, unsigned int* low, unsigned int* high) {
    if (low == NULL || high == NULL) {
        return;
    }

    unsigned int a0 = low_word(a);
    unsigned int a1 = high_word(a);
    unsigned int b0 = low_word(b);
    unsigned int b1 = high_word(b);

    unsigned int p0 = a0 * b0;
    unsigned int p1 = a0 * b1;
    unsigned int p2 = a1 * b0;
    unsigned int p3 = a1 * b1;

    unsigned int middle = p1 + p2;
    unsigned int carryMid = (middle < p1);

    unsigned int lowPart = p0 + (middle << (sizeof(unsigned int) << 2));
    unsigned int carryLow = (lowPart < p0);

    // carryMid represents an overflow of 2^32 in middle; its contribution to high is 2^32/2^16 = 2^16
    unsigned int highPart = p3 + (middle >> (sizeof(unsigned int) << 2))
    + (carryMid << (sizeof(unsigned int) << 2)) + carryLow;

    *low = lowPart;
    *high = highPart;
}

static void normalize_bigint(Bigint* a) {
    if (a == NULL || a->numbers == NULL) {
        return;
    }

    while (a->firstDigit == 0 && a->numbers[0] > 1) {
        a->numbers[0]--;
        a->firstDigit = (int) a->numbers[a->numbers[0]];
        a->numbers[a->numbers[0]] = 0;
    }
}

Bigint* multiply(Bigint* a, Bigint* b) {
    if (a == NULL || b == NULL) {
        return NULL;
    }

    if (a->numbers == NULL || b->numbers == NULL) {
        return NULL;
    }

    // We assume that a and b are normalized numbers written in BASE number system.
    if (a->firstDigit == 0) {
        return a;
    }

    if (b->firstDigit == 0) {
        return b;
    }

    int sign = 1;
    if ((a->firstDigit < 0 && b->firstDigit > 0) || (a->firstDigit > 0 && b->firstDigit < 0)) {
        sign = -1;
    }

    unsigned int max;
    int try = prepare(a, b, &max);
    if (try == 1) {
        return NULL;
    }

    unsigned int *res = calloc(2 * max + 4, sizeof(unsigned int));
    if (res == NULL) {
        return NULL;
    }

    for (unsigned int j = 1; j <= max; j++) {
        unsigned int bj = b->numbers[j];
        unsigned int shift = 0;

        for (unsigned int i = 1; i <= max; i++) {
            unsigned int ai = a->numbers[i];
            unsigned int k = i + j - 1;

            unsigned int mulLow, mulHigh;
            mul_word(ai, bj, &mulLow, &mulHigh);

            unsigned int tmp = mulLow + shift;
            mulHigh += (tmp < mulLow);

            unsigned int old = res[k];
            res[k] += tmp;
            mulHigh += (res[k] < old);

            shift = mulHigh;
        }

        unsigned int pos = max + j;
        while (shift != 0) {
            unsigned int old = res[pos];
            res[pos] += shift;
            shift = (res[pos] < old) ? 1 : 0;
            pos++;
        }
    }

    unsigned int count_real_quantity_of_digits = 2 * max;
    while (count_real_quantity_of_digits > 1 && res[count_real_quantity_of_digits] == 0) {
        count_real_quantity_of_digits--;
    }

    free(a->numbers);
    a->numbers = res;
    a->numbers[0] = count_real_quantity_of_digits;
    a->firstDigit = (int) a->numbers[count_real_quantity_of_digits];

    if (sign < 0) {
        a->firstDigit = a->firstDigit * (-1);
    }

    return a;
}

/**
 * It splits a Bigint number of two halves.
 * @param x A Bigint number.
 * @param high_number Pointer to the high half part of {@code x}.
 * @param small_number Pointer to the small half part of {@code x}.
 */
static void split_on_half(Bigint* x, Bigint* high_number, Bigint* small_number) {
    if (x == NULL || high_number == NULL || small_number == NULL) {
        return;
    }

    unsigned int n = x->numbers[0];
    if (n < 2) {
        return;
    }

    unsigned int *tmp1 = realloc(high_number->numbers, sizeof(unsigned int) * (n / 2 + 2));
    unsigned int *tmp2 = realloc(small_number->numbers, sizeof(unsigned int) * (n / 2 + 2));

    if (tmp1 == NULL || tmp2 == NULL) {
        return;
    }

    high_number->numbers = tmp1;
    high_number->numbers[0] = 0;
    small_number->numbers = tmp2;
    small_number->numbers[0] = 0;

    /*
     * Due to Little Endian (LE) the first n/2 numbers are smallest digits in a bigint,
     * and the last n/2 numbers are biggest.
     */
    unsigned int l = 1, h = 1;
    for (unsigned int i = 1; i <= n; i++) {
        if (i < n/2) {
            small_number->numbers[0]++;
            small_number->numbers[l] = x->numbers[i];
            l++;
        } else if (i == n/2) {
            small_number->numbers[0]++;
            small_number->firstDigit = (int) x->numbers[i];
        } else if (i > n/2 && i < n) {
            high_number->numbers[0]++;
            high_number->numbers[h] = x->numbers[i];
            h++;
        } else if (i == n) {
            high_number->firstDigit = x->firstDigit;
            high_number->numbers[0]++;
        }
    }
}

/**
 * It shifts all numbers in array to the right on m.
 */
static Bigint* shift_right(Bigint* a, unsigned int m) {
    if (a == NULL || m == 0)
        return a;

    unsigned int old_quantity = a->numbers[0];
    unsigned int* tmp = realloc(a->numbers,sizeof(unsigned int) * (old_quantity + m + 1));

    if (tmp == NULL) {
        return NULL;
    }

    a->numbers = tmp;

    for (unsigned int i = old_quantity; i >= 1; i--) {
        a->numbers[i + m] = a->numbers[i];
    }

    for (unsigned int i = 1; i <= m; i++) {
        a->numbers[i] = 0;
    }

    a->numbers[0] = old_quantity + m;

    return a;
}

void delete_many_bigint(int count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        delete(va_arg(args, Bigint*));
    }

    va_end(args);
}

Bigint* Karatsuba(Bigint* a, Bigint* b) {
    if (a == NULL || b == NULL) {
        return NULL;
    }

    unsigned int n = a->numbers[0] > b->numbers[0] ? a->numbers[0] : b->numbers[0];
    unsigned int m = a->numbers[0] < b->numbers[0] ? a->numbers[0] : b->numbers[0];

    if (n < 2 || m < 2) {
        return multiply(a, b);
    }

    Bigint* aHigh = init();
    Bigint* aLow = init();
    Bigint* bHigh = init();
    Bigint* bLow = init();

    split_on_half(a, aHigh, aLow);
    split_on_half(b, bHigh, bLow);

    Bigint* aLowCopy = deep_copy(aLow);
    Bigint* bLowCopy1 = deep_copy(bLow);

    Bigint* s1 = summation(deep_copy(aHigh), aLowCopy);
    Bigint* s2 = summation(deep_copy(bHigh), bLowCopy1);

    Bigint* bLowCopy2 = deep_copy(bLow);
    Bigint* s2Copy = deep_copy(s2);
    Bigint* bHighCopy = deep_copy(bHigh);

    Bigint* res1 = multiply(deep_copy(aLow), bLowCopy2);
    Bigint* tmpRes1 = multiply(deep_copy(s1), s2Copy);
    Bigint* res3 = multiply(deep_copy(aHigh), bHighCopy);

    Bigint* res3Copy = deep_copy(res3);
    Bigint* res1Copy1 = deep_copy(res1);

    Bigint* tmpRes2 = substraction(deep_copy(tmpRes1), res3Copy);
    Bigint* res2 = substraction(deep_copy(tmpRes2), res1Copy1);

    res3 = shift_right(res3, 2 * (n / 2));
    res2 = shift_right(res2, n / 2);

    Bigint* res2Copy = deep_copy(res2);
    Bigint* res1Copy2 = deep_copy(res1);

    Bigint* result = summation(deep_copy(res3), res2Copy);
    result = summation(result, res1Copy2);

    free(a->numbers);
    a->firstDigit = result->firstDigit;
    a->numbers = result->numbers;
    result->numbers = NULL;

    delete_many_bigint(21,
        aLowCopy, bLowCopy1, bLowCopy2, s2Copy, bHighCopy,
        res3Copy, res1Copy1, res1Copy2, res2Copy,
        aHigh, bHigh, aLow, bLow,
        s1, s2, res1, tmpRes1, res3, tmpRes2, res2, result
        );

    normalize_bigint(a);
    return a;
}

void increment(Bigint* n) {
    if (n == NULL) {
        return;
    }

    Bigint *i = init();
    if (i == NULL) {
        return;
    }

    i->numbers = calloc(2, sizeof(unsigned int));
    i->firstDigit= 1;
    i->numbers[0] = 1;

    summation(n, i);
    delete(i);
}

void decrement(Bigint* n) {
    if (n == NULL) {
        return;
    }

    Bigint *i = init();
    if (i == NULL) {
        return;
    }

    i->numbers = calloc(2, sizeof(unsigned int));
    i->firstDigit= 1;
    i->numbers[0] = 1;

    substraction(n, i);
    delete(i);
}

Bigint* deep_copy(Bigint* n) {
    if (n == NULL) {
        return n;
    }

    Bigint* new = init();
    if (new == NULL) {
        return NULL;
    }

    new->firstDigit = n->firstDigit;
    new->numbers = calloc(n->numbers[0] + 1, sizeof(unsigned int));

    for (unsigned int i = 0; i < n->numbers[0]; i++) {
        new->numbers[i] = n->numbers[i];
    }

    return new;
}

int compareBigint(Bigint* a, Bigint* b) {
    if (a->numbers[0] != b->numbers[0]) {
        return a->numbers[0] > b->numbers[0] ? 1 : -1;
    }

    unsigned int am = (unsigned int)(a->firstDigit >= 0 ? a->firstDigit : -a->firstDigit);
    unsigned int bm = (unsigned int)(b->firstDigit >= 0 ? b->firstDigit : -b->firstDigit);

    if (am != bm) {
        return am > bm ? 1 : -1;
    }

    for (int i = (int) a->numbers[0] - 1; i >= 1; i--) {
        if (a->numbers[i] != b->numbers[i]) {
            return a->numbers[i] > b->numbers[i] ? 1 : -1;
        }
    }

    return 0;
}

Bigint* factorial(Bigint* multiplication_algorithm(Bigint* a, Bigint* b), Bigint* n) {
    if (n == NULL) {
        return NULL;
    }

    if (n->firstDigit < 0) {
        return NULL;
    }

    Bigint *i = init();
    if (i == NULL) {
        return NULL;
    }

    i->firstDigit = 1;
    i->numbers = calloc(2, sizeof(unsigned int));
    i->numbers[0] = 1;

    Bigint *result = deep_copy(i);
    if (result == NULL) {
        delete(i);
        return NULL;
    }

    for (; compareBigint(i, n) <= 0; increment(i)) {
        Bigint *i_copy = deep_copy(i);
        result = multiplication_algorithm(result, i_copy);
        delete(i_copy);
    }

    delete(i);
    return result;
}

/**
 * function of counting digits in numbers written in 10-numeric system.
 */
static unsigned int count_number_of_digits(unsigned int result, unsigned int n) {
    if (n / 10 > 0) {
        result++;
        count_number_of_digits(result, n/10);
    }

    return result;
}

Bigint* to_base(unsigned int n) {
    Bigint* new = init();
    if (new == NULL) {
        return NULL;
    }

    new->numbers = calloc(count_number_of_digits(1, n) + 2, sizeof(unsigned int) * 1);
    new->numbers[0] = 1;

    unsigned int i = 1;
    while (n / BASE > 0) {
        new->numbers[0]++;
        new->numbers[i] = n % BASE;
        i++;
        n = n / BASE;
    }


    new->numbers[i] = n % BASE;
    new->firstDigit = (int) new->numbers[new->numbers[0]];
    new->numbers[new->numbers[0]] = 0;

    return new;
}

Bigint* positive_degree(Bigint *multiplication_algorithm(Bigint* a, Bigint* b), Bigint* base, Bigint* degree) {
    if (base == NULL || degree == NULL) {
        return NULL;
    }

    Bigint* result = init();
    if (result == NULL) {
        return NULL;
    }

    Bigint* degree_copy = deep_copy(degree);
    Bigint* base_copy1 = deep_copy(base);

    while (degree_copy->firstDigit > 1) {
        Bigint* base_copy2 = deep_copy(base);
        multiplication_algorithm(base_copy1, base_copy2);
        result = deep_copy(base_copy1);

        delete(base_copy1);
        delete(base_copy2);

        if (result == NULL) {
            return NULL;
        }

        decrement(degree_copy);
        base_copy1 = deep_copy(result);
    }

    delete_many_bigint(2, base_copy1, degree_copy);

    return result;
}

int get_pow2_exponent(Bigint* a) {
    if (a == NULL || a->firstDigit <= 0) {
        return -1;
    }

    // Checks whether is the 2^n or not.
    unsigned int N = a->numbers[0];
    for (unsigned int i = 1; i < N; i++) {
        if (a->numbers[i] != 0) {
            return -1;
        }
    }

    unsigned int top = (unsigned int) a->firstDigit;
    if (top == 0 || (top & (top - 1)) != 0) {
        return -1;
    }

    int bit = 0;
    while (!((top >> bit) & 1)) {
        bit++;
    }

    return bit + 32 * (int)(N - 1);
}

static Bigint* mod_pow2(Bigint* a, unsigned int n) {
    if (a == NULL) {
        return NULL;
    }

    if (n == 0) {
        Bigint* zero = init();
        if (zero == NULL) {
            return NULL;
        }

        zero->firstDigit = 0;
        zero->numbers[0] = 1;
        return zero;
    }

    Bigint* result = deep_copy(a);
    unsigned int N = result->numbers[0];
    result->numbers[N] = (unsigned int) result->firstDigit;

    unsigned int full = n / 32;
    unsigned int rem  = n % 32;
    unsigned int keep = full + (rem > 0 ? 1 : 0);

    if (keep > N) {
        result->numbers[N] = 0;
        return result;
    }

    if (rem > 0) {
        result->numbers[keep] &= (1 << rem) - 1;
    }

    // Отсекаются все левые биты, меньшие n
    for (unsigned int i = keep + 1; i <= N; i++) {
        result->numbers[i] = 0;
    }

    unsigned int new_N = keep;
    while (new_N > 1 && result->numbers[new_N] == 0) {
        new_N--;
    }

    result->numbers[0] = new_N;
    result->firstDigit = (int) result->numbers[new_N];
    result->numbers[new_N] = 0;

    return result;
}

Bigint* mod(Bigint* a, Bigint* b) {
    if (a == NULL || b == NULL) {
        return NULL;
    }

    int p = get_pow2_exponent(b);
    if (p >= 0) {
        return mod_pow2(a, (unsigned int)p);
    }

    Bigint* result = deep_copy(a);
    if (result == NULL) {
        return NULL;
    }

    while (compareBigint(result, b) >= 0) {
        Bigint *b_copy = deep_copy(b);
        result = substraction(result, b_copy);
        delete(b_copy);

        if (result == NULL) {
            return NULL;
        }
    }
    return result;
}

void printBigint(Bigint* n) {
    if (n == NULL) {
        return;
    }

    printf("Printing Bigint number in Little Endian (LE) format...\n");
    for (unsigned int i = 1; i < n->numbers[0]; i++) {
        printf("%u\n", n->numbers[i]);
    }

    printf("%d\n", n->firstDigit);
}