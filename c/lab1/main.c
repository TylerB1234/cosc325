#include <stdio.h>

/*
 * C Imperative Paradigm Lab
 * Parts 1–4 are implemented in this file.
 *
 * Do NOT change function signatures.
 * 
 * 1/15/26 - Brian Toone
 */

/* ===== Part 2 ===== */
int sum_to(int n) {
    int s = 0; // Local variable to hold the sum
    for (int i = 1; i <= n; i++) {
        s += i; // Update the sum
    }
    return s; // Return the computed sum
}

/* ===== Part 3 ===== */
double avg_1_to_n(int n) {
    return (double)sum_to(n) / n; // Avoid integer division
}

/* ===== Part 4 ===== */
void increment_bad(int x) {
    x++;
}

void increment(int *x) {
    (*x)++; // Increment the value at the pointer address
}

int main() {

    /* ===== Part 1 ===== */
    printf("--- Part 1: State and Sequence ---\n");

    int x = 2;
    x = x + 3;
    x = x * 4;
    printf("x=%d\n", x); // Expected output: x=20

    // Reflection 1.1: The value of x changed over time, and intermediate values lived in memory.


    /* ===== Part 2 ===== */
    printf("\n--- Part 2: Control Flow ---\n");
    printf("sum_to(5) = %d (expected 15)\n", sum_to(5));
    printf("sum_to(1) = %d (expected 1)\n", sum_to(1));

    // Reflection 2.1: Control happens in the for loop, and state changes in the local variable s.


    /* ===== Part 3 ===== */
    printf("\n--- Part 3: Functions as Procedures ---\n");
    printf("avg_1_to_n(5) = %.1f (expected 3.0)\n", avg_1_to_n(5));

    // Reflection 3.1: C requires explicit type conversions that Java/Python often infer automatically.


    /* ===== Part 4 ===== */
    printf("\n--- Part 4: Pass-by-Value vs Pointers ---\n");

    int a = 5;
    increment_bad(a);
    printf("after increment_bad: a=%d (expected 5)\n", a);

    increment(&a);
    printf("after increment:     a=%d (expected 6)\n", a);

    // Reflection 4.1: a didn’t change because of pass-by-value.
    // Reflection 4.2: A pointer allows shared state between functions.

    return 0;
}
