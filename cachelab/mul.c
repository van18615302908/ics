/*
 * mul.c - Matrix multiply C = A * B
 *
 * Each multiply function must have a prototype of the form:
 * void mul(int M, int N, int A[N][M], int B[M][N], int C[N][N]);
 *
 * A multiply function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_mul(int M, int N, int A[N][M], int B[M][N], int C[N][N]);

/*
 * multiply_submit - This is the solution multiply function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "multiply submission", as the driver
 *     searches for that string to identify the multiply function to
 *     be graded.
 */

char mul_submit_desc[] = "multiply submission";

#define block_size 8
void mul_submit(int M, int N, int A[N][M], int B[M][N], int C[N][N]) {
    int tmp[16]; 
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            C[i][j] = 0;
    }

    // 外层循环，每次增加8列（bk表示列偏移）
    for (int bk = 0; bk < M; bk += 8) {
        // 主循环，遍历A矩阵的行
        for (int i = 0; i < N; i++) {
            for (int i = 0; i < 16; i++){
                tmp[i] = 0;
            }
            // 将A矩阵的一行复制到tmp数组中
            for (int j = 0; j < 8; j++){
                tmp[j] = A[i][bk + j];
            }

            // 内层循环，每次增加8列（bj表示列偏移）
            for (int bj = 0; bj < 32; bj += 8) {
                // 执行矩阵乘法的主循环
                for (int j = 0; j < 8; j++) {
                    // 通过累加计算C矩阵的一列元素
                    for (int k = 0; k < 8; k++){
                        tmp[j + 8] += tmp[k] * B[bk + k][bj + j];
                    }
                }
                // 将中间结果写回到C的一列
                for (int k = 0; k < 8; k++){
                    C[i][bj + k] += tmp[k + 8];
                }
                // 初始化tmp数组的后8个元素为0！！！！！
                for (int i = 0; i < 8; i++){
                    tmp[i + 8] = 0;
                }
            }
        }
    }
}



/*
 * mul - A simple multiply function, not optimized for the cache.
 */
char mul_desc[] = "multiply submission";
void mul(int M, int N, int A[N][M], int B[M][N], int C[N][N])
{
    int i, j, k, tmp;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            tmp = 0;
            for (k = 0; k < M; k++)
            {
                tmp += A[i][k] * B[k][j];
            }
            C[i][j] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your multiply
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     multiply strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerMulFunction(mul_submit, mul_submit_desc);

    /* Register any additional multiply functions */
    // registerMulFunction(mul, mul_desc);
}

/*
 * is_multiply - This helper function checks if C is the multiply of
 *     A and B. You can check the correctness of your multiply by calling
 *     it before returning from the multiply function.
 */
int is_mul(int M, int N, int A[N][M], int B[M][N], int C[N][N])
{
    int i, j, k;
    int num = 0;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            num = 0;
            for (k = 0; k < M; k++)
            {
                num += A[i][k] * B[k][j];
            }
            if (num != C[i][j])
            {
                return 0;
            }
        }
    }
    return 1;
}
