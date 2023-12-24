/*
author: Riley Durbin
email: ridurbin0@gmail.com

brief description: 
    recursive implementation of merge sort, with pre -allocated left and right array optimization.
*/


#include "mergesort.h"
#include <cstring>
#include <iostream>

// Helper functions
void PrintArray(int* array, int size)
{
    for (int i = 0; i < size; i++)
    {
        std::cout << array[i];
    }
}

// Merges the given left and right arrays, using the parameter indexes to define sizes and positions to use in those arrays
void merge(int * a, int * left, int * right, unsigned leftIndex, unsigned midIndex, unsigned rightIndex)
{
    // Get array sized
    unsigned leftSize = midIndex - leftIndex + 1;
    unsigned rightSize = rightIndex - midIndex;

    // Copy memory into scratch arrays
    memcpy(left, a + leftIndex, sizeof(int) * leftSize);
    memcpy(right, a + midIndex + 1, sizeof(int) * rightSize);

    unsigned i1 = 0; // For iterating through the left array
    unsigned i2 = 0; // For iterating through the right array

    unsigned mergedIndex = leftIndex;

    while (i1 < leftSize && i2 < rightSize) // While both arrays are unsorted
    {
        // Sort left and right arrays into return array
        if (left[i1] < right[i2])
        {
            a[mergedIndex] = left[i1]; 
            ++i1;
        }
        else
        {
            a[mergedIndex] = right[i2]; 
            ++i2;
        }
        mergedIndex++; // Advance merged index
    }

    // Copy over any remaining data from left array
    while (i1 < leftSize)
    {
        a[mergedIndex] = left[i1];
        ++i1;
        mergedIndex++;
    }

    // Copy over any remaining data from right array
    while (i2 < rightSize)
    {
        a[mergedIndex] = right[i2];
        ++i2;
        mergedIndex++;
    }

}

void mergesort_rec(int * a, int * left, int * right, unsigned start, unsigned end)
{
    if (start >= end)
    {
        return;
    }


    // Splits given array in half
    unsigned mid = start + (end - start) / 2;

    // Mergesort on each half
    mergesort_rec(a, left, right, start, mid);
    mergesort_rec(a, left, right, mid + 1, end);

    // Once splitting is done, merges back
    merge(a, left, right, start, mid, end);
}

// ///////////////////////////////////////////////////////
// Main usage function 
void mergesort(int *a, unsigned p, unsigned r)
{
    // Scratch arrays for merging
    int * left = new int[r/2];
    int * right = new int[r/2];

    mergesort_rec(a, left, right, p, r - 1);

    //PrintArray(a, r); // Debug print of array

    // Deletes scratch arrays
    delete [] left;
    delete [] right;

    // No return array, as given array was sorted through the passed pointer

}

