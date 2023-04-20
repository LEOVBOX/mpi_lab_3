//
// Created by Леонид Шайхутдинов on 05.04.2023.
//

#include <mpi.h>
#include <stdio.h>
#include "mem_ops.h"

#define Y 0
#define X 1
#define ROOT 0

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

void data_broadcast(double* sub_matrix1, double* sub_matrix2, int rank_y, int rank_x, int* matrix1_summands,
		int* matrix2_summands, int* dims, int N2, MPI_Comm* row_comm, MPI_Comm* col_comm)
{
	for (int i = 0; i < dims[0]; i++)
	{
		if (rank_y == i)
		{
			//printf("Try to bcast %d %d: matrix1_summands[rank_y] = %d\n", rank_x, rank_y, matrix1_summands[rank_y]);
			MPI_Bcast(sub_matrix1, matrix1_summands[rank_y] * N2, MPI_DOUBLE, 0, row_comm[i]);
		}
	}
	for (int i = 0; i < dims[1]; ++i)
	{
		if (rank_x == i)
		{
			MPI_Bcast(sub_matrix2, matrix2_summands[rank_x] * N2, MPI_DOUBLE, 0, col_comm[i]);
		}
	}
}

void matrix_partition(double* Matrix, int K, int* lines_per_proc, double* sub_matrix, int rank_y,
		int rank_x, int dim_y, MPI_Comm *comm)
{
	int* send_count = (int*)mallocs(dim_y * sizeof(int));
	int* send_offset = (int*)mallocs(dim_y * sizeof(int));

	for (int i = 0; i < dim_y; i++)
	{
		send_count[i] = lines_per_proc[i] * K;
		if (i > 0)
		{
			send_offset[i] = lines_per_proc[i - 1] * K + send_offset[i-1];
		}
		else
		{
			send_offset[i] = 0;
		}
	}

	// DEBUG BEGIN
//	for (int i = 0; i < dim_y; i++)
//	{
//		printf("%d %d send_offset[%d]: %d\n", rank_x, rank_y, i, send_offset[i]);
//		printf("%d %d send count[%d]: %d\n", rank_x, rank_y, i, send_count[i]);
//	}
//	printf("\n");
	// DEBUG END

	int res = MPI_Scatterv(Matrix, send_count, send_offset, MPI_DOUBLE, sub_matrix,
			send_count[rank_y], MPI_DOUBLE, 0, comm[rank_x]);
	// DEBUG BEGIN
//	if (res == MPI_SUCCESS)
//	{
//		printf("Scatter SUCCESS! %d %d\n", rank_x, rank_y);
//	}
//	else
//	{
//		printf("FAIL %d %d\n", rank_x, rank_y);
//	}
	// DEBUG END

	free(send_count);
	free(send_offset);
}


void collect_res_matrix(int sizeSubmatrixA, int sizeSubmatrixB, int n3, int rank, int numprocs, double *matrixRes,
		double *submatrixRes, int*dims, MPI_Comm gridComm) {
	MPI_Datatype send_submatrix, send_submatrix_resized;
	MPI_Type_vector(sizeSubmatrixA, sizeSubmatrixB, n3, MPI_DOUBLE, &send_submatrix);
	MPI_Type_commit(&send_submatrix);
	MPI_Type_create_resized(send_submatrix, 0, (int)(sizeSubmatrixB * sizeof(double)), &send_submatrix_resized);
	MPI_Type_commit(&send_submatrix_resized);

	int *sendOffset = NULL, *sendNum = NULL;
	if (rank == 0) {
		sendOffset = calloc(numprocs, sizeof(int));
		sendNum = calloc(numprocs, sizeof(int));
		sendOffset[0] = 0;
		for (int i = 0; i < dims[Y]; i++) {
			for (int j = 0; j < dims[X]; j++) {
				sendOffset[i + j * dims[Y]] = (j * sizeSubmatrixB + sizeSubmatrixA * i * n3) / sizeSubmatrixB;
				//printf("%d ",(j * sizeSubmatrixB + sizeSubmatrixA * i * N3) / sizeSubmatrixB);
			}
		}
		for (int i = 0; i < numprocs; ++i) {
			sendNum[i] = 1;
		}
	}

	MPI_Gatherv(submatrixRes, sizeSubmatrixA * sizeSubmatrixB, MPI_DOUBLE, matrixRes,
			sendNum, sendOffset, send_submatrix_resized, ROOT, gridComm);

	MPI_Type_free(&send_submatrix);
	MPI_Type_free(&send_submatrix_resized);
	if (rank == 0) {
		free(sendOffset);
		free(sendNum);
	}
}



