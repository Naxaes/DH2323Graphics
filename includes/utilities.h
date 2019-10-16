#pragma once

constexpr float PI = 3.14159265358979323846264338327950288f;


template <typename T>
struct Array2D
{
    using Type = T;

    Type* data;
    unsigned rows;
    unsigned columns;


    // ---- CONSTRUCTORS ----
    Array2D(unsigned rows, unsigned columns) : rows(rows), columns(columns)
    {
        data = static_cast<T*>(malloc(rows * columns * sizeof(T)));
    }
    Array2D(unsigned rows, unsigned columns, T fill) : rows(rows), columns(columns)
    {
        data = static_cast<T*>(malloc(rows * columns * sizeof(T)));
        for (unsigned i = 0; i < rows * columns; ++i)
            data[i] = fill;
    }

    // ---- COPY/MOVE CONSTRUCTOR ----
    Array2D(const Array2D& other) : rows(other.rows), columns(other.columns)
    {
        Assert(false, "Don't copy! If you really want it, do it yourself!");

        data = static_cast<T*>(malloc(rows * columns * sizeof(T)));

        for (unsigned i = 0; i < rows * columns; ++i)
            data[i] = other.data[i];
    }
    Array2D(Array2D&& other) noexcept : rows(other.rows), columns(other.columns)
    {
        data = other.data;
        other.data = nullptr;
    }

    // ---- DESTRUCTOR ----
    ~Array2D() { delete[] data; }

    // ---- COPY/MOVE ASSIGNMENT ----
    Array2D& operator= (const Array2D& other [[gnu::unused]])
    {
        Assert(false, "Don't copy! If you really want it, do it yourself!");

        data = malloc(rows * columns * sizeof(T));

        for (unsigned i = 0; i < rows * columns; ++i)
            data[i] = other.data[i];

        return *this;
    }
    Array2D& operator= (Array2D&& other) noexcept
    {
        delete[] data;
        data = other.data;
        rows = other.rows;
        columns = other.columns;

        other.data = nullptr;

        return *this;
    }

    inline T& operator() (unsigned row, unsigned column) const noexcept { return data[row * columns + column]; }
    inline T& operator() (int      row, int      column) const noexcept { return data[row * columns + column]; }
};

template <typename T>
void Fill(Array2D<T>& array, T value)
{
    for (int i = 0; i < array.rows * array.columns; ++i)
        array.data[i] = value;
}

template <typename T>
void Clear(Array2D<T>& array)
{
    std::memset(array.data, 0, array.rows * array.columns * sizeof(T));
}