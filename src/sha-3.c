#include "sha-3.h"

void print_uint64_array(uint64_t *A, int length)
{
    for(int i = 0; i < length; i++)
    {
        printf("%016llX (%d)\n", A[i], i);
    }
}

void print_state_array(uint64_t *A)
{
    for(int j = 0; j < 5; j++)
    {
        for(int k = 0; k < 5; k++)
        {
            printf("%016llX ", A[i(k, j)]);
        }

        printf("\n");
    }
}

void print_uint8_array(uint8_t *A, int length)
{
    for(int i = 0; i < length; i++)
    {
        printf("%02hhX (%d)\n", A[i], i);
    }
}

void theta(uint64_t *A)
{
    uint64_t C[5], D[5];

    for(int x = 0; x < 5; x++)
    {
        C[x] = A[i(x, 0)];

        for(int y = 1; y < 5; y++)
        {
            C[x] ^= A[i(x, y)];
        }
    }

    for(int x = 0; x < 5; x++)
    {
        D[x] = C[(x + 4) % 5] ^ ROL(C[(x + 1) % 5], 1);

        for(int y = 0; y < 5; y++)
        {
            A[i(x, y)] ^= D[x];
        }
    }
}

void rho(uint64_t *A, int w)
{
    int x = 1;
    int y = 0;

    for(int t = 0; t < 24; t++)
    {
        int X = y % 5;
        int Y = ((2 * x) + (3 * y)) % 5;

        A[i(x, y)] = ROL(A[i(x, y)], (((t + 1) * (t + 2)) / 2) % w);

        x = X;
        y = Y;
    }
}

void pi(uint64_t *A)
{
    uint64_t tmpA[25];

    for(int x = 0; x < 5; x++)
    {
        for(int y = 0; y < 5; y++)
        {
            int X = y % 5;
            int Y = ((2 * x) + (3 * y)) % 5;

            tmpA[i(X, Y)] = A[i(x, y)];
        }
    }

    for(int i = 0; i < 25; i++)
    {
        A[i] = tmpA[i];
    }
}

void chi(uint64_t *A)
{
    uint64_t tmpA[5];

    for(int y = 0; y < 5; y++)
    {
        for(int x = 0; x < 5; x++)
        {
            tmpA[x] = A[i(x, y)];
        }

        for(int x = 0; x < 5; x++)
        {
            A[i(x, y)] ^= ((~tmpA[(x + 1) % 5]) & tmpA[(x + 2) % 5]);
        }
    }
}

uint64_t rc(int t)
{
    if(t % 255 == 0)
    {
        return (uint64_t) 0x1;
    }

    uint64_t R = (uint64_t) 0x1;

    for(int i = 0; i < t % 255; i++)
    {
        R <<= (uint64_t) 0x1;

        if(R & (uint64_t) 0x100)
        {
            R ^= (uint64_t) 0x71;
        }
    }

    return R & (uint64_t) 0x1;
}

void iota(uint64_t *A, int l, int i_r)
{
    uint64_t RC = (uint64_t) 0x0;

    for(int j = 0; j < l + 1; j++)
    {
        RC |= rc(j + (7 * i_r)) << ((1 << j) - 1);
    }

    A[i(0, 0)] ^= RC;
}

void keccak_p_permute(int b, int n_r, uint64_t *A)
{
    printf("Input of Permutation\n");
    print_state_array(A);
    printf("\n");

    for(int i_r = 0; i_r < n_r; i_r++)
    {
        theta(A);
        //printf("After Theta Step (%d)\n", i_r);
        //print_state_array(A);
        //printf("\n");
        rho(A, b / 25);
        //printf("After Rho Step (%d)\n", i_r);
        //print_state_array(A);
        //printf("\n");
        pi(A);
        //printf("After Pi Step (%d)\n", i_r);
        //print_state_array(A);
        //printf("\n");
        chi(A);
        //printf("After Chi Step (%d)\n", i_r);
        //print_state_array(A);
        //printf("\n");
        iota(A, log2(b / 25), i_r);
        //printf("After Iota Step (%d)\n", i_r);
        //print_state_array(A);
        //printf("\n");
    }

    printf("State After Permutation\n");
    print_state_array(A);
    printf("\n");
}

void keccak_f_permute(int b, uint64_t *A)
{
    keccak_p_permute(b, 12 + (2 * log2(b / 25)), A);
}

uint64_t *uint8_to_uint64(uint8_t *X, int X_count)
{
    int n = (X_count / sizeof(uint8_t)) + (X_count % sizeof(uint8_t) != 0);
    uint64_t *Y = malloc((sizeof(uint64_t) / sizeof(uint8_t)) * n);
    memset(Y, 0, (sizeof(uint64_t) / sizeof(uint8_t)) * n);

    for(int j = 0; j < n; j++)
    {
        for(int k = 0; k < 8; k++)
        {
            Y[j] |= ((uint64_t) ((8 * j + k) < X_count ? X[(8 * j + k)] : 0x0) << (8 * k));
        }
    }

    return Y;
}

void keccak(int b, int r, uint8_t *M_8, int M_length, uint8_t *d, int d_length)
{
    if((b % 8 != 0) || (r % 8 != 0))
    {
        return;
    }

    int r_byte_length = r / 8;
    int M_byte_length = (M_length / 8) + (M_length % 8 != 0);
    int d_byte_length = (d_length / 8) + (d_length % 8 != 0);

    uint64_t *M_64 = uint8_to_uint64(M_8, M_byte_length);
    int M_64_count = (M_length / 64) + (M_length % 64 != 0);

    printf("Length (in bits) of message: %d\n", M_length);
    printf("Length (in bytes) of message: %d\n", M_byte_length);
    printf("# of 64-bit integers in message: %d\n", M_64_count);
    print_uint64_array(M_64, M_64_count);
    printf("Currently, the message %s divisible by r. (M_length / r = %d)\n", M_length % r == 0 ? "is" : "is not", M_length / r);

    int n = 1;

    while(M_length + 2 > n * r)
    {
        n++;
    }

    printf("In the end, r should divide the message %d times.\n", n);

    int new_count = M_64_count;

    printf("The initial new count of 64-bit integers is %d.\n", new_count);

    while(n * r > (8 * sizeof(uint64_t)) * new_count)
    {
        new_count += 1;
    }

    printf("%d 64-bit integers will be required due to padding.\n", new_count);

    uint64_t *M = malloc(8 * new_count * sizeof(uint64_t));
    memcpy(M, M_64, sizeof(uint64_t) * M_64_count);
    memset(M + M_64_count, 0, r_byte_length - M_byte_length);

    M[(M_length + 1) / 64] |= ((uint64_t) 0x1) << (M_length % 64);
    M[new_count - 1] |= ((uint64_t) 0x1) << 63;

    print_uint64_array(M, new_count);

    // Must be modified to be dynamic rather than static.
    uint64_t *A = malloc((b / 64) * 8);
    memset(A, 0, b / 64 * 8);

    for(int j = 0; j < n; j++)
    {
        for(int k = 0; k < r / 64; k++)
        {
            A[k] ^= M[(25 * j) + k];
        }

        keccak_f_permute(b, A);
    }

    memset(d, 0, d_byte_length);
    print_uint8_array(d, d_byte_length);

    int x = 0;

    for(int i = 0; i < d_byte_length; i++)
    {
        printf("Byte #%d of the digest is from 64-bit interger with index %d. It is the byte with index %d inside the 64-bit interger.\n", i, i / 8, x);

        d[i] = (A[i / 8] >> (8 * x)) & 0xFF;

        x++;

        if((i != 0) && (i % r_byte_length == 0))
        {
            keccak_f_permute(b, A);
            x = 0;
        }
    }

    print_uint8_array(d, d_byte_length);

    // n is wrong
    // int n = (M_length / r) + (M_length % r != 0);
    // if(((8 * sizeof(uint64_t)) * M_64_count) - M_length < 2)
    // {
    //     M_byte_length += sizeof(uint64_t);
    //     M_64_count++;
    // }
    // uint64_t *M = malloc(M_byte_length);
    // memcpy(M, M_64, sizeof(*M_64) * M_64_count);
    // memset(M + (sizeof(*M_64) * M_64_count), 0, r / 8);
    //print_uint64_array(M, (n * r) / sizeof(*M_64));
    // uint64_t *M = M_64;
    // printf("%d\n", (n * r) - M_length);
    // if((n * r) - M_length < 2)
    // {
    //     printf("%lu\n", (sizeof(*M_64) * M_64_count) + (r / 8));
    //     M = malloc((sizeof(*M_64) * M_64_count) + (r / 8));
    //     memcpy(M, M_64, ;
    //     memset();
    //     n++;
    // }
    // M[(M_length + 1) / 64] |= ((uint64_t) 0x1) << (M_length % 64);
    // M[M_64_count - 1] |= ((uint64_t) 0x1) << 64;
    // print_uint64_array(M, n);
    // uint64_t A[b / 64];
    // memset(A, 0, sizeof(A));
    // while(n > 0)
    // {
    //     for(int i = 0; i < r / 64; i++)
    //     {
    //         A[i] ^= M_64[(r / 64 * n) + i];
    //     }
    //     keccak_f_permute(b, A);
    //     n--;
    // }
    // while(d_length / 8 > 0)
    // {
    //     int block_byte_size = (d_length / 8 < r / 8 ? d_length / 8 : r / 8);
    //     memcpy(d, A, block_byte_size);
    //     d += block_byte_size;
    //     keccak_f_permute(b, A);
    //     d_length -= block_byte_size * 8;
    // }
}

// void keccak(int b, int r, uint8_t *M, int M_length, uint64_t suffix, uint8_t *d, int d_length)
// {
//     if((b % 8 != 0) || (r % 8 != 0))
//     {
//         return;
//     }

//     uint8_t A[b / 8];
//     memset(A, 0, sizeof(A));

//     int c = b - r;
//     int r_byte_length = r / 8;
//     int M_byte_length = (M_length / 8) + (M_length % 8 != 0);
//     int d_byte_length = (d_length / 8) + (d_length % 8 != 0);
//     int block_byte_size;

//     printf("Input Message (M)\n");

//     for(int i = 0; i < M_byte_length; i++) {
//         printf("%02hhX ", M[i]);
//     }

//     printf("\n\n");

//     printf("Suffix\n");
//     printf("%02hhX\n\n", suffix);

//     while(M_byte_length > 0)
//     {
//         block_byte_size = (M_byte_length < r_byte_length ? M_byte_length : r_byte_length);

//         for(int i = 0; i < block_byte_size; i++)
//         {
//             A[i] ^= M[i];
//         }

//         M += block_byte_size;
//         M_byte_length -= block_byte_size;

//         if(block_byte_size == r_byte_length)
//         {
//             keccak_f_permute(b, A);
//             block_byte_size = 0;
//         }
//     }

//     A[block_byte_size] ^= suffix;

//     printf("Input Message with Suffix Appended\n");

//     for(int i = 0; i < (M_length / 8) + (M_length % 8 != 0); i++) {
//         printf("%02hhX ", M[i]);
//     }

//     printf("\n\n");

//     if (((suffix & 0x80) != 0) && (block_byte_size == (r_byte_length - 1)))
//     {
//         keccak_f_permute(b, A);
//     }

//     A[r_byte_length - 1] ^= 0x80;
//     keccak_f_permute(b, A);

//     while(d_byte_length > 0) {
//         block_byte_size = (d_byte_length < r_byte_length ? d_byte_length : r_byte_length);
//         memcpy(d, A, block_byte_size);
//         d += block_byte_size;
//         d_byte_length -= block_byte_size;

//         if(d_byte_length > 0)
//         {
//             keccak_f_permute(b, A);
//         }
//     }
// }

int main(int argc, const char *argv[])
{
    int b = 1600;
    int r = 576;
    int d_length = 512;

    uint8_t M[4] = {
        0x53, 0x58, 0x7B, 0x99
    };

    uint8_t *d = malloc(d_length / 8);

    keccak(b, r, M, 32, d, 512);

    for(int i = 0; i < d_length / 8; i++)
    {
        printf("%02hhX", d[i]);
    }

    return 0;
}
