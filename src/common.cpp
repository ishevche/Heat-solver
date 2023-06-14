// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "common.h"
#include "exceptions.h"
#include "main_process.h"
#include "calculating_process.h"

#include <fstream>

void start_process(const configs &config, int rank, int world_size) {
    if (!is_stable(config)) {
        throw NotStableSystem("Von Neumann criteria is not satisfied");
    }
    if (world_size <= 1) {
        throw NotEnoughProcesses("World size_type have to be at least 2");
    }
    auto plate = read_initial_state(config);

    if (rank == world_size - 1) {
        main_process(config, world_size, plate);
    } else {
        auto setup = get_process_setup(config, world_size, rank);
        Array2D<double, uint> plate_part(config.x_steps + 2,
                                         setup.second + 2,
                                         plate[setup.first]);
        calculating_process(config, rank,
                            world_size, plate_part);
    }
}

bool is_stable(const configs &configs) {
    double delta_x = configs.width / static_cast<double>(configs.x_steps);
    double delta_y = configs.height / static_cast<double>(configs.y_steps);
    double alpha = configs.conduction /
                   (configs.density * configs.heat_capacity);
    double max_delta = std::max(delta_x, delta_y);
    return configs.delta_t <= max_delta * max_delta / (4 * alpha);
}

Array2D<double, uint> read_initial_state(const configs &configs) {
    Array2D<double, uint> plate(configs.x_steps + 2,
                                configs.y_steps + 2);
    std::ifstream raw_file(configs.init_file);
    for (int i = 0; i < configs.x_steps + 2; ++i) {
        for (int j = 0; j < configs.y_steps + 2; ++j) {
            raw_file >> plate[i][j];
        }
    }
    return plate;
}
