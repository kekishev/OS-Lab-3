#include "../bigint.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * ТЗ номер 3 п.a
 */
static Bigint* af(Bigint* multiplication_algorithm(Bigint* a, Bigint* b), Bigint* n) {
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
static Bigint* count(Bigint* multiplication_algorithm(Bigint* a, Bigint* b), Bigint* n) {
    if (n == NULL) {
        return NULL;
    }

    Bigint* two   = to_base(2);
    Bigint* nCopy = deep_copy(n);
    if (two == NULL || nCopy == NULL) {
        delete_many_bigint(2,
            two, nCopy);
        return NULL;
    }

    Bigint* modulus = positive_degree(multiplication_algorithm, two, nCopy);
    delete_many_bigint(2,
        two, nCopy);
    if (modulus == NULL) {
        return NULL;
    }

    Bigint* result = to_base(1);
    Bigint* base   = to_base(115249);
    if (result == NULL || base == NULL) {
        delete_many_bigint(3,
            modulus, result, base);

        return NULL;
    }

    unsigned int exp = 4183;
    while (exp > 0) {
        if (exp & 1) {
            Bigint* baseCopy = deep_copy(base);
            result = multiplication_algorithm(result, baseCopy);
            Bigint* reduced = mod(result, modulus);

            delete_many_bigint(2, baseCopy, result);

            result = reduced;
            if (result == NULL) {
                delete_many_bigint(2, base, modulus);

                return NULL;
            }
        }
        exp >>= 1;
        if (exp > 0) {
            Bigint* base_copy = deep_copy(base);
            base = multiplication_algorithm(base, base_copy);
            Bigint* reduced = mod(base, modulus);

            delete_many_bigint(2, base_copy, base);

            base = reduced;
            if (base == NULL) {
                delete_many_bigint(2, result, modulus);

                return NULL;
            }
        }
    }

    delete_many_bigint(2, base, modulus);

    return result;
}

int main(void) {
    /**
     * Generating test cases for laboratory work.
     */
    Bigint* a = init();
    Bigint* b = init();
    if (a == NULL || b == NULL) {
        return 1;
    }

    unsigned int* a_tmp = realloc(a->numbers, sizeof(unsigned int) * 3);
    if (a_tmp == NULL) {
        return 1;
    }

    unsigned int* b_tmp = realloc(b->numbers, sizeof(unsigned int) * 3);
    if (b_tmp == NULL) {
        return 1;
    }

    a->numbers = a_tmp;
    b->numbers = b_tmp;

    if (a_tmp == NULL || b_tmp == NULL) {
        delete_many_bigint(2, a, b);
        return 1;
    }

    a->numbers[0] = 3;
    a->numbers[1] = 100;
    a->numbers[2] = 3000;
    a->firstDigit = 13;

    b->numbers[0] = 3;
    b->numbers[1] = 1293;
    b->numbers[2] = 4000;
    b->firstDigit = 120;

    Bigint* aCopy = deep_copy(a);
    Bigint* mySubstraction = substraction(deep_copy(b), aCopy);
    printBigint(mySubstraction); // true
    delete(aCopy);
    delete(mySubstraction);

    Bigint* bCopy = deep_copy(b);
    Bigint* mySummation = summation(deep_copy(a), bCopy);
    printBigint(mySummation); // true
    delete(bCopy);
    delete(mySummation);

    /**
     * For checking multiplication and Karatsuba algorithm more trivial values have been set.
     */
    a->firstDigit = 4000;
    a->numbers[0] = 2;
    a->numbers[1] = 10;

    b->firstDigit = 3000;
    b->numbers[0] = 2;
    b->numbers[1] = 20;

    Bigint* bCopy1 = deep_copy(b);
    Bigint* myMultiplication = multiply(deep_copy(a), bCopy1);
    printBigint(myMultiplication); // true
    delete(myMultiplication);
    delete(bCopy1);

    Bigint* bCopy2 = deep_copy(b);
    Bigint* myKaratsuba = Karatsuba(deep_copy(a), bCopy2);
    printBigint(myKaratsuba); // true
    delete(bCopy2);
    delete(myKaratsuba);

    delete(a);
    delete(b);

    Bigint* cAf = init();
    if (cAf == NULL) {
        return 1;
    }
    cAf->numbers[0] = 1;
    cAf->firstDigit = 151;

    Bigint* cCopy1 = deep_copy(cAf);
    clock_t startDefault = clock();
    Bigint* afByDefault = af(multiply, cCopy1);
    clock_t endDefault = clock();
    printBigint(afByDefault);
    printf("af with multiply for small values:  %.3f s\n", (double) (endDefault - startDefault) / CLOCKS_PER_SEC);
    delete(afByDefault);
    delete(cCopy1);

    Bigint* cCopy2 = deep_copy(cAf);
    clock_t startKaratsuba = clock();
    Bigint* afByKaratsuba = af(Karatsuba, cCopy2);
    clock_t endKaratsuba = clock();
    printBigint(afByKaratsuba);
    printf("af with karatsuba for small values: %.3f s\n", (double) (endKaratsuba - startKaratsuba) / CLOCKS_PER_SEC);
    delete(afByKaratsuba);
    delete(cCopy2);
    delete(cAf);

    Bigint* bigC = init();
    if (bigC == NULL) {
        return 1;
    }

    bigC->numbers[0] = 1;
    bigC->firstDigit = 501;

    Bigint* cCopy3 = deep_copy(bigC);
    clock_t startBigDefault = clock();
    Bigint* bigAfByDefault = af(multiply, cCopy3);
    clock_t bigEndDefault = clock();
    printf("af with multiply for big values:  %.9f s\n", (double) (bigEndDefault - startBigDefault) / CLOCKS_PER_SEC);
    delete(bigAfByDefault);
    delete(cCopy3);

    Bigint* cCopy4 = deep_copy(bigC);
    clock_t startBigKaratsuba = clock();
    Bigint* bigAfByKaratsuba = af(Karatsuba, cCopy4);
    clock_t bigEndKaratsuba = clock();
    printf("af with karatsuba for big values: %.9f s\n", (double) (bigEndKaratsuba - startBigKaratsuba) / CLOCKS_PER_SEC);
    delete(bigAfByKaratsuba);
    delete(cCopy4);
    delete(bigC);

    Bigint* cCount = init();
    if (cCount == NULL) {
        return 1;
    }
    cCount->numbers[0] = 1;
    cCount->firstDigit = 30;

    int reps = 20000;

    clock_t startCountByDefault = clock();
    Bigint* countDefault = NULL;
    for (int k = 0; k < reps; k++) {
        Bigint* cCopy5 = deep_copy(cCount);
        delete(countDefault);
        countDefault = count(multiply, cCopy5);
        delete(cCopy5);
    }
    clock_t endCountByDefault = clock();
    printBigint(countDefault);
    printf("count with multiply  (%d reps): %.3f s\n", reps, (double)(endCountByDefault - startCountByDefault) / CLOCKS_PER_SEC);
    delete(countDefault);

    clock_t startCountByKaratsuba = clock();
    Bigint* countKaratsuba = NULL;
    for (int k = 0; k < reps; k++) {
        Bigint* cCopy6 = deep_copy(cCount);
        delete(countKaratsuba);
        countKaratsuba = count(Karatsuba, cCopy6);
        delete(cCopy6);
    }
    clock_t endCountByKaratsuba = clock();
    printBigint(countKaratsuba);
    printf("count with karatsuba (%d reps): %.3f s\n", reps, (double)(endCountByKaratsuba - startCountByKaratsuba) / CLOCKS_PER_SEC);
    delete(countKaratsuba);
    delete(cCount);

    return 0;
}
