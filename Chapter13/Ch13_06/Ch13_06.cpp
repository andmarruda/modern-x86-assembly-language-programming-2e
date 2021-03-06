//------------------------------------------------
//               Ch13_06.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <memory>

using namespace std;

// Test size limits to illustrate argument checking
extern "C" size_t c_NumRowsMax = 1000000;
extern "C" size_t c_NumColsMax = 1000000;

extern "C" bool Avx512CalcColumnMeans_(const double* x, size_t nrows, size_t ncols, double* col_means, size_t* col_counts, double x_min);

void Init(double* x, size_t n, int rng_min, int rng_max, unsigned int seed)
{
    uniform_int_distribution<> ui_dist {rng_min, rng_max};
    default_random_engine rng {seed};

    for (size_t i = 0; i < n; i++)
        x[i] = (double)ui_dist(rng);
}

bool Avx512CalcColumnMeansCpp(const double* x, size_t nrows, size_t ncols, double* col_means, size_t* col_counts, double x_min)
{
    // Make sure nrows and ncols are valid
    if (nrows == 0 || nrows > c_NumRowsMax)
        return false;
    if (ncols == 0 || ncols > c_NumColsMax)
        return false;

    // Initialize column means and column counts to zero
    for (size_t i = 0; i < ncols; i++)
    {
        col_means[i] = 0.0;
        col_counts[i] = 0;
    }

    // Calculate column means
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            double val = x[i * ncols + j];

            if (val >= x_min)
            {
                col_means[j] += val;
                col_counts[j]++;
            }
        }
    }

    for (size_t j = 0; j < ncols; j++)
        col_means[j] /= col_counts[j];

    return true;
}

void Avx512CalcColumnMeans(void)
{
    const size_t nrows = 20000;
    const size_t ncols = 23;
    const int rng_min = 1;
    const int rng_max = 999;
    const unsigned int rng_seed = 47;
    const double x_min = 75.0;

    unique_ptr<double[]> x {new double[nrows * ncols]};
    unique_ptr<double[]> col_means1 {new double[ncols]};
    unique_ptr<double[]> col_means2 {new double[ncols]};
    unique_ptr<size_t[]> col_counts1 {new size_t[ncols]};
    unique_ptr<size_t[]> col_counts2 {new size_t[ncols]};

    Init(x.get(), nrows * ncols, rng_min, rng_max, rng_seed);

    bool rc1 = Avx512CalcColumnMeansCpp(x.get(), nrows, ncols, col_means1.get(), col_counts1.get(), x_min);
    bool rc2 = Avx512CalcColumnMeans_(x.get(), nrows, ncols, col_means2.get(), col_counts2.get(), x_min);

    cout << "Results for Avx512CalcColumnMeans\n";

    if (!rc1 || !rc2)
    {
        cout << "Invalid return code: ";
        cout << "rc1 = " << boolalpha << rc1 << ", ";
        cout << "rc2 = " << boolalpha << rc2 << '\n';
        return;
    }

    cout << "Test Matrix (nrows = " << nrows << ", ncols = " << ncols << ")\n";
    cout << "\nColumn Means\n";
    cout << fixed << setprecision(4);

    for (size_t j = 0; j < ncols; j++)
    {
        cout << setw(4) << j << ": ";
        cout << "col_means = ";
        cout << setw(10) << col_means1[j] << ", ";
        cout << setw(10) << col_means2[j] << "    ";

        cout << "col_counts = ";
        cout << setw(6) << col_counts1[j] << ", ";
        cout << setw(6) << col_counts2[j] << '\n';

        if (col_means1[j] != col_means2[j])
            cout << "col_means compare error\n";

        if (col_counts1[j] != col_counts2[j])
            cout << "col_counts compare error\n";
    }
}

int main()
{
    Avx512CalcColumnMeans();
    return 0;
}
