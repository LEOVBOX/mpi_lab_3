#include <stdio.h>
#include "linear_matrix.h"
#include "mem_ops.h"
#include <mpi.h>
#include <string.h>
#include "mm_malloc.h"
#include <math.h>

#define ROOT 0
#define X 1
#define Y 0

int char_to_int(char a)
{
	if ((a > 57) || (a < 48))
		return 0;
	return a - '0';
}

int string_to_int(char* str)
{
	int result = 0;
	int multiplier = 1;
	int len = (int)strlen(str);
	for (int i = len-1; i > -1; i--)
	{
		result += char_to_int(str[i]) * multiplier;
		multiplier *= 10;
	}
	return result;
}

int read_input_file(char* file_path, double* matrix1, double* matrix2, int n1, int n2, int n3)
{
	FILE* fd = fopen(file_path, "r");
	if (fd == NULL)
	{
		printf("fopen(%s) failed\n", file_path);
		return -1;
	}

	fscan_linear_matrix(fd, matrix1, n1, n2);
	fgetc(fd);
	fscan_linear_matrix(fd, matrix2, n2, n3);

	fclose(fd);
	return 0;
}

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



MPI_Comm* create_comms(MPI_Comm grid_comm2d, int* dims)
{
	MPI_Comm* comms = (MPI_Comm*)mallocs(sizeof(MPI_Comm) * (dims[0] + dims[1]));
	MPI_Group grid_group;
	if (MPI_Comm_group(grid_comm2d, &grid_group) != MPI_SUCCESS)
	{
		printf("MPI_Comm_group failed\n");
	}

	MPI_Group buffer;

	int* ranks = (int*)mallocs(sizeof(int) * dims[Y]);

	int cur_proc_rank;
	int cur_coord[2] = {0, 0};

	// Создание вертикальных коммуникаторов
	for (int x = 0; x < dims[X]; x++)
	{
		for (int i = 0; i < dims[Y]; i++)
		{
			cur_coord[X] = x;
			cur_coord[Y] = i;
			MPI_Cart_rank(grid_comm2d, cur_coord, &cur_proc_rank);
			printf("cur coords: %d %d\ncur_proc_rank = %d\n", cur_coord[1], cur_coord[0], cur_proc_rank);
			ranks[i] = cur_proc_rank;
		}
		MPI_Group_incl(grid_group, dims[Y], ranks, &buffer);
		MPI_Comm_create(grid_comm2d, buffer, &comms[x]);
	}

	ranks = reallocs(ranks, sizeof(int) * dims[X]);

	// Создание горизонтальных коммуникаторов
	for (int y = 0; y < dims[Y]; y++)
	{
		for (int i = 0; i < dims[X]; i++)
		{
			cur_coord[Y] = y;
			cur_coord[X] = i;
			MPI_Cart_rank(grid_comm2d, cur_coord, &cur_proc_rank);
			printf("cur coords: %d %d\ncur_proc_rank = %d\n", cur_coord[X], cur_coord[Y], cur_proc_rank);
			ranks[i] = cur_proc_rank;
		}
		MPI_Group_incl(grid_group, dims[X], ranks, &buffer);
		MPI_Comm_create(grid_comm2d, buffer, &comms[y]);
	}

	free(ranks);

	return comms;
}

int main(int argc, char* argv[])
{

	int procs_num = 0;
	int rank = -1;
	MPI_Status status;

	// Инициализация MPI. Указываем сколько процессов должно создаться.
	int rc = MPI_Init(&argc, &argv);
	if (rc != MPI_SUCCESS)
	{
		printf("Mpi Initialisation error\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	// Получение числа инициализированных процессов.
	MPI_Comm_size(MPI_COMM_WORLD, &procs_num);
	// Получение номера данного процесса.
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	// Чтение параметров матриц
	int n1 = string_to_int(argv[argc - 3]);
	//printf("n1 = %d\n", n1);
	int n2 = string_to_int(argv[argc - 2]);
	//printf("n2 = %d\n", n2);
	int n3 = string_to_int(argv[argc - 1]);
	//printf("n3 = %d\n", n3);


	// Параметры решетки
	int size, size_x, size_y, rank_x, rank_y, comm2d_rank;
	int prev_x, prev_y, next_x, next_y;
	int coords[2];
	int dims[2] = {0, 0};

	// Создание решетки
	MPI_Comm grid_comm2d;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	create_grid(procs_num, dims, &size_y, &size_x, &grid_comm2d);
	MPI_Comm_rank(grid_comm2d, &comm2d_rank);
	int grid_comm_size = 0;
	MPI_Comm_size(grid_comm2d, &grid_comm_size);
	//printf("grid_comm_size = %d\n", grid_comm_size);
	MPI_Cart_coords(grid_comm2d, comm2d_rank, 2, coords);

	printf("rank %d coords: %d %d\n", rank, coords[0], coords[1]);

	MPI_Comm* comms = create_comms(grid_comm2d, dims);


	if (rank == ROOT)
	{
		if (argc < 3)
		{
			printf("Few args\n");
			return 1;
		}


		double* matrix1 = (double*)mallocs(n1 * n2 * sizeof(double));
		double* matrix2 = (double*)mallocs(n2 * n3 * sizeof(double));

		printf("malloced\n");

		read_input_file("input.txt", matrix1, matrix2, n1, n2, n3);

		printf("file read\n");


		free(matrix1);
		free(matrix2);
	}

	free(comms);
	MPI_Finalize();

	return 0;
}
