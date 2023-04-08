//
// Created by Леонид Шайхутдинов on 05.04.2023.
//

#ifndef MPI_LAB_3_PARALLEL_MATRIX_MULT_H
#define MPI_LAB_3_PARALLEL_MATRIX_MULT_H

void create_grid(int procs_num, int* dims, int* size_y, int* size_x, MPI_Comm *comm2d);
MPI_Comm* create_comms(MPI_Comm grid_comm2d, int* dims, int coord);
void matrix_partition(double* Matrix, int N, int K, int* lines_per_proc, double* sub_matrix, int rank_y,
		int rank_x, int dim, MPI_Comm comm);

#endif //MPI_LAB_3_PARALLEL_MATRIX_MULT_H
