//
// Created by Леонид Шайхутдинов on 13.03.2023.
//

#include "mm_malloc.h"
#include "stdio.h"

void* mallocs(size_t size)
{
	void* result = malloc(size);
	if (result == NULL)
	{
		printf("Malloc failed\n");
		exit(2);
	}
	return result;
}

void* reallocs(void* ptr, size_t size)
{
	void* res = realloc(ptr, size);
	if (res == NULL)
	{
		printf("Realloc failed");
		exit(3);
	}
	return res;
}