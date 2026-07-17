#ifndef LAB_H
#define LAB_H

#include "../bigint.h"

/**
 * ТЗ номер 3 п.a
 */
static inline Bigint *af(Bigint *multiplication_algorithm(Bigint *a, Bigint *b), Bigint *n) {
    if (n == NULL) {
        return NULL;
    }

    if (n->firstDigit <= 0) { // assume Bigint n is always natural.
        return NULL;
    }

    long long check;
    if (n->numbers[0] == 1) {
        check = (long long) n->firstDigit;
    } else {
        check = (long long) n->numbers[1];
    }

    if (check % 2 == 0) {
        Bigint *result = init();
        if (result == NULL) {
            return NULL;
        }

        result->firstDigit = 0;
        result->numbers[0] = 1;
        return result;
    }

    return factorial(multiplication_algorithm, n);
}

/**
 * Т3 номер 3, п.b — computes 115249^4183 mod 2^n
 *
 * Counts modulus of result of exponentiation repeatedly to make a number be beyond the modulus.
 */
static inline Bigint *count(Bigint *multiplication_algorithm(Bigint *a, Bigint *b), Bigint *n) {
    if (n == NULL) {
        return NULL;
    }

    Bigint *two   = to_base(2);
    Bigint *nCopy = deep_copy(n);
    if (two == NULL || nCopy == NULL) {
        delete_many_bigint(2,
            two, nCopy);
        return NULL;
    }

    Bigint *modulus = positive_degree(multiplication_algorithm, two, nCopy);
    delete_many_bigint(2,
        two, nCopy);
    if (modulus == NULL) {
        return NULL;
    }

    Bigint *result = to_base(1);
    Bigint *base   = to_base(115249);
    if (result == NULL || base == NULL) {
        delete_many_bigint(3,
            modulus, result, base);

        return NULL;
    }

    unsigned int exp = 4183;
    while (exp > 0) {
        if (exp & 1) {
            Bigint *baseCopy = deep_copy(base);
            result = multiplication_algorithm(result, baseCopy);
            Bigint *reduced = mod(result, modulus);

            delete_many_bigint(2,
                    baseCopy, result);

            result = reduced;
            if (result == NULL) {
                delete_many_bigint(2,
                    base, modulus);

                return NULL;
            }
        }
        exp >>= 1;
        if (exp > 0) {
            Bigint *base_copy = deep_copy(base);
            base = multiplication_algorithm(base, base_copy);
            Bigint *reduced = mod(base, modulus);

            delete_many_bigint(2,
                base_copy, base);

            base = reduced;
            if (base == NULL) {
                delete_many_bigint(2,
                    result, modulus);

                return NULL;
            }
        }
    }

    delete_many_bigint(2,
        base, modulus);

    return result;
}

#endif
