#pragma once

#define BASE (1ULL << sizeof(unsigned int) * 8)

typedef struct Bigint {
    int firstDigit;
    unsigned int* numbers;
} Bigint;

/**
 * Initialize a Bigint instance with 2 assigned memory cells in numbers
 */
Bigint* init();

/**
 * Safely delete a Bigint number.
 * It will work correctly even with passed nullptr as argument.
 */
void delete(Bigint* number);

/**
 * Rough input in BASE number system with little endian (LE) format.
 *
 * Actually, I don't use it in my program due to lack of necessity to do that.
 */
int inputBigint(Bigint* number);

/**
 * Summation of two Bigint numbers.
 * @param a Bigint number, which becomes a return value.
 * @param b Bigint number
 * @return a+b
 */
Bigint* summation(Bigint* a, Bigint* b);

/**
 * Substraction of two Bigint numbers.
 * @param a Bigint number, which becomes a return value.
 */
Bigint* substraction(Bigint* a, Bigint* b);

/**
 * Multiplication of two Bigint numbers.
 * @param a - a Bigint number, which becomes a return value.
 */
Bigint* multiply(Bigint* a, Bigint* b);

/**
 * Deletes an arbitrary number of Bigint pointers.
 * @param count - how many Bigint* arguments follow.
 */
void delete_many_bigint(int count, ...);

/**
 * It is the implementation of karatsuba algorithm for two Bigint numbers.
 * @param a - a Bigint number, which becomes a return value.
 */
Bigint* Karatsuba(Bigint* a, Bigint* b);

/**
 * It is the implementation of increment function for Bigint numbers.
 */
void increment(Bigint* n);

/**
 * It is the implementation of decrement function for Bigint numbers.
 */
void decrement(Bigint* n);

/**
 * Implementation of making a deep copy for Bigint numbers.
 */
Bigint* deep_copy(Bigint* n);

/**
 * Compares two Bigint numbers by module.
 * @return -1 if |a| < |b|, 0 if |a| == |b|, 1 if |a| > |b|
 */
int compareBigint(Bigint* a, Bigint* b);

/**
 * Factorial function for Bigint numbers.
 */
Bigint* factorial(Bigint* multiplication_algorithm(Bigint* a, Bigint* b), Bigint* n);

/**
 * It is the function of casting an unsigned int number to a Bigint number.
 */
Bigint *to_base(unsigned int n);

/**
 * It is the implementation of exponentiation to a positive degree.
 * Returns base^degree as a new Bigint. Destroys degree (decrements to 0).
 */
Bigint* positive_degree(Bigint *multiplication_algorithm(Bigint* a, Bigint* b), Bigint* base, Bigint* degree);

/**
 * Returns n if a == 2^n (n >= 0), otherwise returns -1.
 */
int get_pow2_exponent(Bigint* a);

/**
 * Computes a mod b.
 * Fast O(N) path when b is a power of 2 (uses a bit masking).
 * Falls back to repeated subtraction otherwise.
 */
Bigint* mod(Bigint* a, Bigint* b);

/**
 * It is the function of printing a Bigint number.
 */
void printBigint(Bigint* n);