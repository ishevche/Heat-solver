// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef MPI_MAIN_PROCESS_H
#define MPI_MAIN_PROCESS_H

#include "configs.h"
#include "array2D.h"

void main_process(const configs &config,
                  int world_size,
                  Array2D<double, uint> &whole_plate);

std::pair<int, int> get_process_setup(const configs &config,
                                      int world_size, int rank);

void save_image(const Array2D<double, uint> &plate, size_t step,
                const std::string &output_folder);

#endif //MPI_MAIN_PROCESS_H
