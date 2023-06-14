// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef MPI_ARRAY2D_H
#define MPI_ARRAY2D_H

#include <vector>

template<typename T, typename size_type=size_t>
class Array2D {
public:
    Array2D() = delete;

    Array2D(size_type cols, size_type rows)
            : col_num(cols), row_num(rows) {
        data = new T[rows * cols];
    }

    Array2D(size_type cols, size_type rows, T *data_pointer)
            : col_num(cols), row_num(rows) {
        data = new T[rows * cols];
        std::copy(data_pointer, data_pointer + rows * cols, data);
    }

    Array2D(const Array2D &other)
            : Array2D(other.col_num,
                      other.row_num,
                      other.data) {}

    Array2D(Array2D &&other) noexcept
            : col_num(other.col_num),
              row_num(other.row_num),
              data(other.data) {
        other.col_num = 0;
        other.row_num = 0;
        other.data = new T[0];
    }

    Array2D &operator=(const Array2D &other) {
        Array2D tmp(other);
        swap(tmp);
        return *this;
    }

    Array2D &operator=(Array2D &&other) noexcept {
        Array2D tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    void swap(Array2D &other) {
        std::swap(other.row_num, row_num);
        std::swap(other.col_num, col_num);
        std::swap(other.data, data);
    }

    T *operator[](size_type idx) {
        return &data[col_num * idx];
    }

    const T *operator[](size_type idx) const {
        return &data[col_num * idx];
    }

    [[nodiscard]] size_type cols() const noexcept {
        return col_num;
    }

    [[nodiscard]] size_type rows() const noexcept {
        return row_num;
    }

    ~Array2D() {
        delete[]data;
    }


private:
    size_type col_num;
    size_type row_num;
    T *data;
};

#endif //MPI_ARRAY2D_H
