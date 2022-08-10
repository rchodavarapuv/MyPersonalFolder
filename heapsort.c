# -*- coding: utf-8 -*-
"""HeapSort.c

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1jGtEJGJXpA7GBhrkZrVNU-QuHsqWEgTE
"""

#include <stdio.h>
#include <stdlib.h>

#define MAX 6
void heapify(int*, int);

void swap(int *x, int *y)
{
    int temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

void heapify(int *a, int MAX_v)
{
    int i=0;
    
    for(i=(MAX_v-1)/2;i>=0;i--)
    {
        if(*(a+i) < *(a+(2*i)) && ((2*i) < MAX_v))
        {
            swap((a+i),(a+(2*i)));
        }
        if(*(a+i) < *(a+(2*i)+1) && ((2*i+1) < MAX_v))
        {
            swap((a+i), (a+((2*i)+1)));
        }
    }
}


int main()
{
    int arr[MAX] = {1, 12, 9, 5, 6, 10};
    int i=0;
    heapify(arr, MAX);
    for(i=0;i<MAX;i++)
    printf("%d\t",arr[i]);
    heapify(arr+1, MAX-1);
    for(i=0;i<MAX;i++)
    printf("%d\t",arr[i]);
    return 0;
}