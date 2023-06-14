// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "options_parser.h"
#include "exceptions.h"
#include "common.h"
#include <mpi.h>

int main(int argc, char *argv[]) {
    int commsize, rank, exit_code = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#ifndef NDEBUG
    int len;
    char procname[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(procname, &len);
    printf("Process %d of %d on node %s.\n",
           rank, commsize, procname);
#endif //NDEBUG
    try {
        command_line_options_t cmd_options(argc, argv);
        config_file_options_t configs(cmd_options.config_file);
        start_process(configs.config, rank, commsize);
    } catch (BaseException &ex) {
        std::cerr << ex.what() << std::endl;
        exit_code = ex.get_exit_code();
    }
    MPI_Finalize();
    return exit_code;
}
