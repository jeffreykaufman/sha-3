#include "sha-3.h"

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
    for(int i_r = 0; i_r < n_r; i_r++)
    {
        theta(A);
        rho(A, b / 25);
        pi(A);
        chi(A);
        iota(A, log2(b / 25), i_r);
    }
}

void keccak_f_permute(int b, uint64_t *A)
{
    keccak_p_permute(b, 12 + (2 * log2(b / 25)), A);
}

void keccak(int b, int r, uint8_t *M, int M_length)
{
    if((b % 8 != 0) || (r % 8 != 0))
    {
        return;
    }

    uint8_t A[b / 8];
    memset(A, 0, sizeof(A));

    int c = b - r;
    int r_byte_length = r / 8;
    int M_byte_length = (M_length / 8) + (M_length % 8 != 0);

    while(M_byte_length > 0)
    {
        int block_byte_size = (M_byte_length < r_byte_length ? M_byte_length : r_byte_length);

        for(int i = 0; i < block_byte_size; i++)
        {

        }

        if()
        {
            keccak_f_permute(b, A);
        }
    }


}

int main(int argc, const char *argv[])
{
    uint64_t M[25] = {
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
    };

    keccak(1600, 576, M, 1600);

    return 0;
}
