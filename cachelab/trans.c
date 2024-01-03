/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */

char transpose_submit_desc[] = "Transpose submission";
// void transpose_submit(int M, int N, int A[N][M], int B[M][N])
// {
// }
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (M == 48) {
        int temp[12];
        for (int bi = 0; bi < M; bi+=12) {
            for (int bj = 0; bj < N; bj+=12) {
                for (int k = 0; k < 12; k++) {
                    for(int i = 0;i <12;i++){
                        temp[i] = A[bi + k][bj +i];
                    }
                    for(int i = 0;i <12;i++){
                        B[bj+i][bi + k] = temp[i];
                    }                    
                }
            }
        }
    }
    else if(M==96){
        int a[12] = {0}; // 创建一个大小为12的整型数组a，用于临时存储数据
        for(int i=0;i<M;i+=12){ // 外层循环，以12为步长遍历A的行
            for(int j=0;j<N;j+=12){ // 内层循环，以12为步长遍历A的列
                for(int k = i;k < i+6;k++){ // 遍历A的左上部分的行
                    for(int m = 0;m < 12;m++){ // 将A的左上部分的数据存储到数组a中
                        a[m] = A[k][j+m];
                    }
                    for(int m = 0;m < 6;m++){ // 将数组a的前6个元素复制到B的对应位置
                        B[j+m][k] = a[m];
                    }
                    for(int m = 0;m < 6;m++){ // 将数组a的后6个元素复制到B的对应位置
                        B[j+m][k+6] = a[m+6];
                    }                                        
                }

                for(int k =j;k < j +6;k++){ // 遍历B的右上部分的列
                    for(int m = 0;m < 6;m++){ // 将B的右上部分的数据存储到数组a中
                        a[m] = B[k][i+6+m];
                    } 
                    for(int m = 0;m < 6;m++){ // 将A的左下部分的数据存储到数组a的后6个元素中
                        a[m+6] = A[i+6+m][k];
                    }

                    for(int m = 0;m < 6;m++){ // 将数组a的后6个元素复制到B的左下部分对应位置
                        B[k][i+6+m] = a[m+6];
                    } 
                    for(int m = 0;m < 6;m++){ // 将数组a的前6个元素复制到B的右下部分对应位置
                        B[k+6][i+m] = a[m];
                    }                                                      
                }

                for(int k = i+6; k < i+12 ; k++){ // 遍历A的右下部分的行
                    for(int m = 0;m < 6;m++){ // 将A的右下部分的数据存储到数组a的后6个元素中
                        a[m+6] = A[k][j+m+6];
                    }                      
                    for(int m = 0;m < 6;m++){ // 将数组a的后6个元素复制到B的右下部分对应位置
                        B[j+m+6][k] = a[m+6];
                    }   
                }
            }
        }
    }
    else if(M == 93){
        for (int i = 0; i < N; i += 32)
            for (int j = 0; j < M; j += 24)
                for (int k = i; k < i + 32 && k < N; k++)
                    for (int l = j; l < j + 24 && l < M; l++)
                        B[l][k] = A[k][l];
    }
    else{
        int i, j, tmp;

        for (i = 0; i < N; i++)
        {
            for (j = 0; j < M; j++)
            {
                tmp = A[i][j];
                B[j][i] = tmp;
            }

        }
    }
}


/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
