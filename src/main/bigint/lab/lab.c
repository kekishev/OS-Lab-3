#include "../bigint.h"
#include "lab.h"
#include <time.h>

int main(void) {
    /**
     * Generating test cases for laboratory work.
     */
    Bigint* a = init();
    Bigint* b = init();
    if (a == NULL || b == NULL) {
        return 1;
    }

    realloc(a->numbers, sizeof(unsigned int) * 3);
    realloc(b->numbers, sizeof(unsigned int) * 3);

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
