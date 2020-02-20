#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <string.h>
#include <chrono>

#define ARR1_XSIZE 512
#define ARR1_YSIZE 512
#define ARR2_XSIZE 512
#define ARR2_YSIZE 512
#define ABS_MAX_VAL 100

#define DEBUG
// #define FULL_MATRIX

#define THREADS 24

using std::string;
using Clock = std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::cout;
using std::endl;

#define PRINT_ERROR(description) printf("ERROR: %s\n", description)
#define PRINT_INFO(description) printf("INFO: %s\n", description)

void seed_random_generator()
{
    static bool hasSeeded = false;
    if (!hasSeeded)
    {
        srand(time(NULL));
        hasSeeded = true;
    }
}

void make_rand_vector(int **arr, unsigned int size, unsigned int maxNum)
{
    *arr = new int[size];
    for (int i = 0; i < size; i++)
    {
        (*arr)[i] = (rand() % maxNum) - maxNum / 2;
    }
}

void make_rand_matrix(int **arr, unsigned int xSize, unsigned int ySize, unsigned int maxNum)
{
    *arr = new int[xSize * ySize];
    for (int i = 0; i < ySize; i++)
    {
        for (int j = 0; j < xSize; j++)
        {
            (*arr)[i * xSize + j] = (rand() % maxNum) - maxNum / 2;
        }
    }
}

void make_const_matrix(int **arr, unsigned int xSize, unsigned int ySize, unsigned int num)
{
    *arr = new int[xSize * ySize];
    for (int i = 0; i < ySize; i++)
    {
        for (int j = 0; j < xSize; j++)
        {
            (*arr)[i * xSize + j] = num;
        }
    }
}

void print_array(int *arr, unsigned int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d\t", arr[i]);
    }
    printf("\n");
}

void print_matrix_octave(int *arr, unsigned int xSize, unsigned int ySize)
{
    for (int i = 0; i < ySize; i++)
    {
        for (int j = 0; j < xSize; ++j)
        {
            printf("%d ", arr[i*xSize+j]);
        }
        if(i < ySize-1)
        {
            printf(" ; ");
        }
    }
    printf("\n");
}

void print_matrix(int *arr, unsigned int xSize, unsigned int ySize)
{
    for (int i = 0; i < ySize; i++)
    {
        for (int j = 0; j < xSize; ++j)
        {
            printf("%d ", arr[i*xSize+j]);
        }
        printf("\n");
    }
}

int matrix_mult(
    int *arr1, int *arr2,
    unsigned int xSize1, unsigned int ySize1,
    unsigned int xSize2, unsigned int ySize2,
    int **result,
    unsigned int *resXSize, unsigned int *resYSize)
{
    if (xSize1 != ySize2)
    {
        PRINT_ERROR("Matrix Dimensions Not Equal arr1_xSize is not equal to arr2_ySize");
        return -1;
    }

    unsigned int commonSide = xSize1;
    *result = new int[ySize1 * xSize2];
    memset(*result, 0, sizeof(int)*(ySize1 * xSize2));
    int *sumArray[THREADS];

    for (int i = 0; i < THREADS; ++i)
    {
        sumArray[i] = new int[commonSide];
        memset(sumArray[i], 0, sizeof(int)*(commonSide));
    }


    *resXSize = xSize2;
    *resYSize = ySize1;

    register int sum = 0;
    
    #pragma omp parallel for  
    for (unsigned int ii = 0; ii < ySize1; ii++)
    {
        unsigned int tid = omp_get_thread_num();

        // #pragma omp parallel for
        #pragma omp parallel for private(sum)
        for (int jj = 0; jj < xSize2; jj++)
        {
            // #pragma omp parallel for 
            for (unsigned int kk = 0; kk < commonSide; kk++)
            {
                sumArray[tid][kk] = arr1[ii * xSize1 + kk] * arr2[kk * xSize2 + jj];
            }
            sum = 0;
            #pragma omp parallel for reduction(+:sum)
            for (unsigned int kk = 0; kk < commonSide; kk++)
            {
                sum += sumArray[tid][kk];
            }
            (*result)[ii*(*resXSize)+jj] = sum;
        }
        
    }

    for (int i = 0; i < THREADS; ++i)
    {
        delete[] sumArray[i];
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    int *arr1, *arr2, *res;
    unsigned int resXSize, resYSize;
 
    seed_random_generator();
    make_rand_matrix(&arr1, ARR1_XSIZE, ARR1_YSIZE, ABS_MAX_VAL);
    make_rand_matrix(&arr2, ARR2_XSIZE, ARR2_YSIZE, ABS_MAX_VAL);
    omp_set_num_threads(THREADS);

    time_point<Clock> start = Clock::now();

    if(matrix_mult(arr1, arr2, ARR1_XSIZE, ARR1_YSIZE, ARR2_XSIZE, ARR2_YSIZE, &res, &resXSize, &resYSize))
    {
        PRINT_ERROR("Failed to multiply matrices due to invalid sizing, exiting....");
        return -1;
    }

    time_point<Clock> end = Clock::now();
    milliseconds diff = duration_cast<milliseconds>(end - start);


    #ifdef DEBUG

    printf("\n");

    PRINT_INFO("MATRIX A Octave Form");
    print_matrix_octave(arr1, ARR1_XSIZE, ARR1_YSIZE);

    PRINT_INFO("MATRIX B Octave Form");
    print_matrix_octave(arr2, ARR2_XSIZE, ARR2_YSIZE);

    PRINT_INFO("RESULT Octave Form");
    print_matrix_octave(res, resXSize, resYSize);

    printf("\n");

    #ifdef FULL_MATRIX

    PRINT_INFO("MATRIX A Octave Form");
    print_matrix(arr1, ARR1_XSIZE, ARR1_YSIZE);
    printf("\n");

    PRINT_INFO("MATRIX B Octave Form");
    print_matrix(arr2, ARR2_XSIZE, ARR2_YSIZE);
    printf("\n");

    PRINT_INFO("RESULT Octave Form");
    print_matrix(res, resXSize, resYSize);
    printf("\n");

    #endif    
    #endif

    printf("mult time %dms\n", diff.count());

    delete[] arr1;
    delete[] arr2;
    delete[] res;
    return 0;
}
