// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef MPI_CALCULATING_PROCESS_H
#define MPI_CALCULATING_PROCESS_H

#include "configs.h"
#include "array2D.h"
#include <mpi.h>

void calculating_process(const configs &config,
                         int rank, int world_size,
                         Array2D<double, uint> &plate_state);

void calculate_next_state(const configs &config,
                          const Array2D<double, uint> &plate_state,
                          Array2D<double, uint> &next_state);

void receive_rows(int rank, int world_size,
                  Array2D<double, uint> &receive_plate,
                  MPI_Request *requests);

void send_rows(int rank, int world_size,
               const Array2D<double, uint> &send_plate,
               MPI_Request *requests);

#endif //MPI_CALCULATING_PROCESS_H
