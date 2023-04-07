//
// Created by Леонид Шайхутдинов on 07.04.2023.
//
#include "stdio.h"
#include "mm_malloc.h"


void print_int_array(int* array, int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%d ", array[i]);
	}
	printf("\n");
}

int* generate_summands(int n, int k)
{
	if (n == 0)
	{
		printf("Невозможно разложить на 0 слагаемых\n");\
		return NULL;
	}
	int* summands = (int*)malloc(sizeof(int) * k);
	if (k == 1)
	{
		summands[0] = n;
		printf("%d\n", n);
		return summands;
	}

	// Заполняем массив слагаемых единицами
	for (int i = 0; i < k; i++)
	{
		summands[i] = 1;
	}

	int sum = k;
	while (sum != n)
	{
		for (int i = 0; i < k; i++)
		{
			summands[i] += 1;
			sum++;
			if (sum == n)
				break;
		}
	}
	print_int_array(summands, k);
	return summands;
}
