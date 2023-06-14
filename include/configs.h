// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef MPI_CONFIGS_H
#define MPI_CONFIGS_H

#include <string>
#include <vector>

struct configs {
    double width;
    double height;
    int x_steps;
    int y_steps;
    size_t t_steps;
    size_t display_interval;
    double delta_t;
    std::string init_file;
    std::string pngs_folder;
    double min_temp;
    double max_temp;
    double density;
    double conduction;
    double heat_capacity;
};

#endif //MPI_CONFIGS_H
