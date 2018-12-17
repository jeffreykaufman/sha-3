#ifndef sha_3_h
#define sha_3_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define ROL(bitstring, offset) ((((uint64_t) bitstring) << offset) ^ (((uint64_t) bitstring) >> (sizeof(uint64_t) * 8 - offset)))
#define i(x, y) ((5 * y) + x)

void print_state_array(uint64_t *A);
void theta(uint64_t *A);
void rho(uint64_t *A, int w);
void pi(uint64_t *A);
void chi(uint64_t *A);
uint64_t rc(int t);
void iota(uint64_t *A, int l, int i_r);
void keccak_p_permute(int b, int n_r, uint64_t *A);
void keccak_f_permute(int b, uint64_t *A);
uint64_t *pad10_1();
uint64_t *uint8_to_uint64(uint8_t *X, int X_count);
void keccak(int b, int r, uint8_t *M, int M_length, uint64_t suffix, uint8_t *d, int d_length);

#endif
