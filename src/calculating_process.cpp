// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "calculating_process.h"
#include "timer.h"

void calculating_process(const configs &config,
                         int rank, int world_size,
                         Array2D<double, uint> &plate_state) {
    auto start_time = get_current_time_fenced();

    Array2D<double, uint> next_state(plate_state);
    Array2D<double, uint> send_copy(plate_state.cols(),
                                    plate_state.rows() - 2);
    MPI_Request gather_request;
    MPI_Request sr_requests[4] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL,
                                  MPI_REQUEST_NULL, MPI_REQUEST_NULL};
    for (size_t t = 0; t < config.t_steps; ++t) {
        if (t != 0)
            receive_rows(rank, world_size, next_state,
                         &sr_requests[2]);
        calculate_next_state(config, plate_state, next_state);
        if (t != 0)
            MPI_Waitall(4, sr_requests,
                        MPI_STATUSES_IGNORE);
        plate_state.swap(next_state);
        if (t != config.t_steps - 1)
            send_rows(rank, world_size, plate_state,
                      &sr_requests[0]);

        if (t % config.display_interval == config.display_interval - 1) {
            std::copy(plate_state[1], plate_state[send_copy.rows() + 1],
                      send_copy[0]);
            if (t != config.display_interval - 1)
                MPI_Wait(&gather_request, MPI_STATUS_IGNORE);
            MPI_Igatherv(
                    send_copy[0],
                    static_cast<int>(send_copy.rows() * send_copy.cols()),
                    MPI_DOUBLE,
                    nullptr,
                    nullptr,
                    nullptr,
                    MPI_DOUBLE,
                    world_size - 1,
                    MPI_COMM_WORLD,
                    &gather_request
            );
        }
    }
    MPI_Wait(&gather_request, MPI_STATUS_IGNORE);
    auto end_time = get_current_time_fenced();
    auto my_execution_time = to_ms(end_time - start_time);
#ifdef INDIVIDUAL_TIMERS
    std::cout << "Rank " << rank << " execution time: " <<
             my_execution_time << std::endl;
#else
    MPI_Reduce(
            &my_execution_time,
            nullptr,
            1,
            MPI_LONG_LONG,
            MPI_SUM,
            static_cast<int>(world_size - 1),
            MPI_COMM_WORLD
    );
#endif //INDIVIDUAL_TIMERS
}

void calculate_next_state(const configs &config,
                          const Array2D<double, uint> &my_part,
                          Array2D<double, uint> &next_step) {
    double delta_x = config.width / static_cast<double>(config.x_steps);
    double delta_y = config.height / static_cast<double>(config.y_steps);
    double delta_t = config.delta_t;
    double alpha = config.conduction /
                   (config.density * config.heat_capacity);

    for (uint y = 0; y < my_part.rows() - 2; ++y) {
        for (int x = 0; x < config.x_steps; ++x) {
            double prev_value = my_part[y + 1][x + 1];
            double x_addend =
                    my_part[y + 1][x] -
                    2 * prev_value +
                    my_part[y + 1][x + 2];
            x_addend /= delta_x * delta_x;
            double y_addend =
                    my_part[y][x + 1] -
                    2 * prev_value +
                    my_part[y + 2][x + 1];
            y_addend /= delta_y * delta_y;
            next_step[y + 1][x + 1] =
                    prev_value + delta_t * alpha * (x_addend + y_addend);
        }
    }
}

void receive_rows(int rank, int world_size,
                  Array2D<double, uint> &receive_plate,
                  MPI_Request *requests) {
    if (rank != 0)
        MPI_Irecv(receive_plate[0],
                  static_cast<int>(receive_plate.cols()),
                  MPI_DOUBLE,
                  rank - 1, 0,
                  MPI_COMM_WORLD,
                  &requests[0]);
    if (rank != world_size - 2)
        MPI_Irecv(receive_plate[receive_plate.rows() - 1],
                  static_cast<int>(receive_plate.cols()),
                  MPI_DOUBLE,
                  rank + 1, 0,
                  MPI_COMM_WORLD,
                  &requests[1]);
}

void send_rows(int rank, int world_size,
               const Array2D<double, uint> &send_plate,
               MPI_Request *requests) {
    if (rank != 0)
        MPI_Isend(send_plate[1],
                  static_cast<int>(send_plate.cols()),
                  MPI_DOUBLE,
                  rank - 1, 0,
                  MPI_COMM_WORLD,
                  &requests[0]);
    if (rank != world_size - 2)
        MPI_Isend(send_plate[send_plate.rows() - 2],
                  static_cast<int>(send_plate.cols()),
                  MPI_DOUBLE,
                  rank + 1, 0,
                  MPI_COMM_WORLD,
                  &requests[1]);
}
