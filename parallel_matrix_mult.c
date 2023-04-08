//
// Created by Леонид Шайхутдинов on 05.04.2023.
//

#include <mpi.h>
#include <stdio.h>
#include "mem_ops.h"


#define Y 0
#define X 1

void create_grid(int procs_num, int* dims, int* size_y, int* size_x, MPI_Comm *comm2d)
{
	// Определение размеров решетки: dims
	MPI_Dims_create(procs_num, 2, dims);
	*size_y = dims[Y];
	*size_x = dims[X];

	// Создание коммуникатора: comm2d
	int periods[2] = {0, 0};
	int reorder = 0;


	if (MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, comm2d) != MPI_SUCCESS)
	{
		printf("MPI_Cart_create failed\n");
	}
	//printf("dims: %d %d\n", dims[0], dims[1]);
}

MPI_Comm* create_comms(MPI_Comm grid_comm2d, int* dims, int coord)
{
	MPI_Comm* comms = (MPI_Comm*)mallocs(sizeof(MPI_Comm) * dims[coord]);
	MPI_Group grid_group;
	if (MPI_Comm_group(grid_comm2d, &grid_group) != MPI_SUCCESS)
	{
		printf("MPI_Comm_group failed\n");
	}

	MPI_Group buffer;

	int* ranks = (int*)mallocs(sizeof(int) * dims[!coord]);

	int cur_proc_rank;
	int cur_coord[2] = {0, 0};


	for (int i = 0; i < dims[coord]; i++)
	{
		for (int j = 0; j < dims[!coord]; j++)
		{
			cur_coord[coord] = i;
			cur_coord[!coord] = j;
			MPI_Cart_rank(grid_comm2d, cur_coord, &cur_proc_rank);
			//printf("comm %d num %d creating\ncur coords: %d %d; cur_proc_rank = %d\n", coord, i, cur_coord[X], cur_coord[Y], cur_proc_rank);
			ranks[j] = cur_proc_rank;
		}
		MPI_Group_incl(grid_group, dims[!coord], ranks, &buffer);
		MPI_Comm_create(grid_comm2d, buffer, &comms[i]);
	}

	free(ranks);

	return comms;
}

void matrix_partition(double* Matrix, int N, int K, int* lines_per_proc, double* sub_matrix, int rank_y,
		int rank_x, int dim, MPI_Comm comm)
{
	int* send_count = (int*)mallocs(dim * sizeof(int));
	int* send_offset = (int*)mallocs(dim * sizeof(int));


	for (int i = 0; i < dim; i++)
	{
		send_count[i] = lines_per_proc[i] * N;
		send_offset[i] = i * lines_per_proc[i] * K;

	}

	/*for (int i = 0; i < dim; i++)
	{
		printf("%d %d send_offset[%d]: %d\n", rank_x, rank_y, i, send_offset[i]);
		printf("%d %d send count[%d]: %d\n", rank_x, rank_y, i, send_count[i]);
	}*/
	printf("\n");
	if (MPI_Scatterv(Matrix, send_count, send_offset, MPI_DOUBLE, sub_matrix,
			send_count[rank_x], MPI_DOUBLE, 0, comm) == MPI_SUCCESS)
	{
		printf("SUCCESS! %d %d\n", rank_x, rank_y);
	}
	else
	{
		printf("FAIL %d %d\n", rank_x, rank_y);
	}
	free(send_count);
	free(send_offset);
}
