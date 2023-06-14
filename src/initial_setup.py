#! /usr/bin/env python3
import argparse
import tomllib


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("config", help="path to config file", type=str)
    parser.add_argument("initial_config", help="path to config file with initial state",
                        type=str)
    return parser.parse_args()


def load_toml(path):
    with open(path, 'rb') as file:
        data = tomllib.load(file)
    return data


def trunc_eval(function, min_val, max_val, x=0, y=0):
    value = eval(function)
    return max(min_val, min(max_val, value))


def main(config, init_config):
    config_data = load_toml(config)
    init_data = load_toml(init_config)

    width = config_data['x_steps']
    height = config_data['y_steps']
    out_file = config_data['init_file']
    min_temp = config_data['min_temp']
    max_temp = config_data['max_temp']

    top_func = init_data['top']
    right_func = init_data['right']
    bottom_func = init_data['bottom']
    left_func = init_data['left']
    plate_func = init_data['plate']

    initial_conditions = [[0. for _ in range(width + 2)]
                          for _ in range(height + 2)]

    for x in range(1, width + 1):
        initial_conditions[0][x] = trunc_eval(top_func, min_temp, max_temp, x=x - 1)
    for x in range(1, width + 1):
        initial_conditions[height + 1][x] = trunc_eval(bottom_func, min_temp, max_temp, x=x - 1)
    for y in range(1, height + 1):
        initial_conditions[y][0] = trunc_eval(left_func, min_temp, max_temp, x=y - 1)
    for y in range(1, height + 1):
        initial_conditions[y][width + 1] = trunc_eval(right_func, min_temp, max_temp, x=y - 1)
    for x in range(1, width + 1):
        for y in range(1, height + 1):
            initial_conditions[y][x] = trunc_eval(plate_func, min_temp, max_temp, x=x - 1, y=y - 1)

    with open(out_file, 'w') as result_file:
        for row in initial_conditions:
            str_row = " ".join(map(str, row))
            result_file.write(str_row + '\n')


if __name__ == '__main__':
    args = get_args()
    main(args.config, args.initial_config)
