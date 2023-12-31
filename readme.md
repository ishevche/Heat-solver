# Lab work 10: MPI

Authors (team): [Shevchenko Ivan](https://github.com/ishevche)

## Prerequisites

- cmake
- g++
- libopenmpi-dev
- libboost-all-dev
- libpng++-dev

### Compilation

```shell
./compile.sh
```

```
Usage: ./compile.sh [options]
  Options:
    -h      --help                  Show help message.
    -O      --optimize-build        Compile with optimization before executing.
    -o      --no-optimize-build     Compile without optimization before executing.
    -D      --debug-build           Compile with debug options.
    -d      --no-debug-build        Compile without debug options.
    -I      --install_prefix        Installation path.
    -R      --remove-build-dirs     Remove build dirs after the install.
```

### Installation

```shell
xargs sudo apt-get install < dependencies/apt.txt
```

### Usage

#### C++

After compiling the program from the root directory of the project run the
following, where `<amount>` is number of processes to start:

```shell
mpirun -np <amount> bin/mpi data/config.cfg
```

#### Python

`src/initial_setup.py` is program is used for generate an init configuration 
file. As an input it receives `<conf>` - path to a config file, the same 
that is passed to a `c++` executable and also `<init-conf>` - path to 
initial config configuration file.

```shell
src/initial_setup.py data/config.cfg data/init_config.cfg
```

### Important!

File `config.cfg` - configuration file for `c++` program. In this file next 
values specified:
 - the overall length and division rank for every out of three dimensions: 
   width, height and time.
 - the attributes of the plate: its density, conduction and heat_capacity
 - min and max available temperature
 - path to initial config file
 - path to directory in which intermediate results in form of `pngs` will be 
   saved

File `init_config.cfg` - configuration file for generating initial state. It 
contains rules to calculate values on the edges of the plate and initial 
temperature of the plate. Each one of the borders can depend on the `x` that 
increases from top-left to bottom-right, and plate initial state may depend 
on `x` and `y`, where `x` is horizontal axis and `y` - vertical.

Each `display_interval` number of steps in time, starting from the first, 
program saves a snapshot of current plate into a `png` files into configured 
folder. Afterward they can be combined into `gif` using the following script:
```shell
convert -delay 10 -loop 0 data/pngs/*.png data/hot_borders.gif
```

In `DEBUG` mode, code apart from `png` files also generates `txt` files with 
the same information but in numerical format and stores it in folder 
`data/debug`.

### Results

As a result I would like to present the following animation.
This animation was created using configuration files specified in `data` folder.

![hot_borders](data/hot_borders.gif)

This animation shows temperature speeding from the hot borders onto the cool 
plate. I assume that temperature on the borders are not changing at all, so 
however temperature of the plate changes, it does not affect environment at all.

Initial state of this system can be seen in file `hot_borders.cfg`

The other result is spreading temperature from the hot point in the center 
of the plate:

![hot_center](data/hot_center.gif)

Initial state of this system can be seen in file `hot_center.cfg`

Also, I have launched my program on MPI cluster formed of 2 computers 
connected via Guest network. On the cluster I have started bigger task then 
one presented earlier. Configs of the task are the following:
```toml
width=1000
x_steps=1000
height=1000
y_steps=1000
```
Other configs are the same (initial state is generated by `initial_setup.py` 
and `init.cfg` file). The only thing that changed is the size_type of the plate.

The resulting animation of this start is the following gif:

   ![big plate](data/big_plate.gif)

Launch of this task purely on local machine with `8` processes took `14 
secs` to complete and launch of the same task on the cluster described 
earlier also with `8` processes distributed evenly between computers took 
`3 mins`. Such result can be easily explained, as two neighbour processes on 
different machines have to exchange `20 000` rows through slow network. This 
exchange also implemented in a synchronous way in my code, so no operations 
are done while transition takes place.

Overall the results are expected, clusters are designed to perform better as 
more computers are connected to the cluster. If the task is runnable on a 
single computer it is always better to run it on a single computer rather 
than on cluster, as there is a huge overtime on communication between nodes, 
and I have demonstrated this doing this lab. For achieving benefit from 
using the cluster the number of processes have to be much bigger and the 
task itself also should also be bigger. 
