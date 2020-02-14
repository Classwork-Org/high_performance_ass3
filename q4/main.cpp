#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <string.h>

using std::string;

#define PRINT_ERROR(description) printf("ERROR: %s\n", description)

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

void print_matrix(int *arr, unsigned int xSize, unsigned int ySize)
{
    for (int i = 0; i < ySize; i++)
    {
        print_array(&arr[i], xSize);
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
    int *sumArray = new int[commonSide];
    memset(sumArray, 0, sizeof(int)*(commonSide));

    *resXSize = xSize2;
    *resYSize = ySize1;

    unsigned int res_jj = 0;
    register int sum = 0;
    for (unsigned int ii = 0; ii < ySize1; ii++)
    {
        for (int jj = 0; jj < xSize2; jj++)
        {
            for (unsigned int kk = 0; kk < commonSide; kk++)
            {
                sumArray[kk] = arr1[ii * xSize1 + kk] * arr2[kk * xSize2 + jj];
            }
            sum = 0;
            for (unsigned int kk = 0; kk < commonSide; kk++)
            {
                sum += sumArray[kk];
            }
            (*result)[res_jj++] = sum;
        }
    }
    delete[] sumArray;
    return 0;
}

int main(int argc, char const *argv[])
{
    int *arr1, *arr2, *res;
    unsigned int resXSize, resYSize;
    seed_random_generator();
    make_const_matrix(&arr1, 10, 10, 2);
    make_const_matrix(&arr2, 10, 10, 2);
    if(matrix_mult(arr1, arr2, 10, 10, 10, 10, &res, &resXSize, &resYSize))
    {
        PRINT_ERROR("Failed to multiply matrices due to invalid sizing, exiting....");
        return -1;
    }
    print_matrix(res, resXSize, resYSize);

    delete[] arr1;
    delete[] arr2;
    delete[] res;
    return 0;
}
