// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef MPI_COMMON_H
#define MPI_COMMON_H

#include "configs.h"
#include "array2D.h"

void start_process(const configs &config, int rank, int world_size);

bool is_stable(const configs &configs);

Array2D<double, uint> read_initial_state(const configs &configs);

#endif //MPI_COMMON_H
