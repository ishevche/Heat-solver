// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "main_process.h"
#include "timer.h"
#include <mpi.h>
#include <png++/png.hpp>

void main_process(const configs &config,
                  int world_size,
                  Array2D<double, uint> &whole_plate) {
    auto start_time = get_current_time_fenced();

    MPI_Request gather_request;
    Array2D<double, uint> plate_buffer(whole_plate);
    Array2D<double, uint> plates[2] = {std::move(whole_plate),
                                       std::move(plate_buffer)};

    size_t shots_count = config.t_steps / config.display_interval;
    std::vector<int> counts(world_size, 0);
    std::vector<int> displs(world_size, 0);
    for (int i = 0; i < world_size - 1; ++i) {
        auto rank_setup = get_process_setup(config, world_size, i);
        auto start_row = rank_setup.first;
        auto row_counts = rank_setup.second;
        counts[i] = row_counts * static_cast<int>(plates[0].cols());
        displs[i] = start_row * static_cast<int>(plates[0].cols());
    }
    for (size_t i = 0; i < shots_count; ++i) {
        if (i != 0)
            MPI_Wait(&gather_request, MPI_STATUS_IGNORE);
        MPI_Igatherv(
                plates[0][0], 0,
                MPI_DOUBLE,
                plates[(i + 1) % 2][1],
                counts.data(),
                displs.data(),
                MPI_DOUBLE,
                world_size - 1,
                MPI_COMM_WORLD,
                &gather_request
        );
        save_image(plates[i % 2], i, config.pngs_folder);
    }
    MPI_Wait(&gather_request, MPI_STATUS_IGNORE);
    save_image(plates[shots_count % 2], shots_count,
               config.pngs_folder);

    auto end_time = get_current_time_fenced();
    auto my_execution_time = to_ms(end_time - start_time);
#ifdef INDIVIDUAL_TIMERS
    std::cout << "Rank " << world_size - 1 << " execution time: " <<
              my_execution_time << std::endl;
#else
    size_t total_execution_time;
    MPI_Reduce(
            &my_execution_time,
            &total_execution_time,
            1,
            MPI_LONG_LONG,
            MPI_SUM,
            world_size - 1,
            MPI_COMM_WORLD
    );
    std::cout << "Total execution time: " <<
              total_execution_time << std::endl;
    std::cout << "Real execution time: " << my_execution_time << std::endl;
#endif //INDIVIDUAL_TIMERS
}

std::pair<int, int> get_process_setup(const configs &config,
                                      int world_size, int rank) {
    int counting_processes = world_size - 1;
    int rows_count = config.y_steps / counting_processes;
    int start_row = rows_count * rank;
    if (rank == counting_processes - 1) {
        rows_count += config.y_steps - counting_processes * rows_count;
    }
    return {start_row, rows_count};
}

void save_image(const Array2D<double, uint> &plate, size_t step,
                const std::string &output_folder) {
    png::image<png::rgb_pixel> image(plate.cols(), plate.rows());
    for (png::uint_32 y = 0; y < plate.rows(); ++y) {
        for (png::uint_32 x = 0; x < plate.cols(); ++x) {
            auto value = png::uint_32(plate[y][x] / 100 * 255);
            image.set_pixel(x, y, png::rgb_pixel(value, value, value));
        }
    }
    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') << step;
    image.write(output_folder + "/step" + ss.str() + ".png");

#ifndef NDEBUG
    std::ofstream out("data/debug/" + std::to_string(step) + ".txt");

    for (size_t y = 0; y < plate.rows(); ++y) {
        for (size_t x = 0; x < plate.cols(); ++x) {
            out << plate[y][x] << " ";
        }
        out << std::endl;
    }
    out.close();
#endif //NDEBUG
}
