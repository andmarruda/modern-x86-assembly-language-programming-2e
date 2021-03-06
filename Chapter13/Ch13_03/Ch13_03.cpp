//------------------------------------------------
//               Ch13_03.cpp
//------------------------------------------------

#include "stdafx.h"
#include <cstdint>
#include <iostream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

extern "C" void Avx512CvtF32ToU32_(uint32_t val_cvt[4], float val);
extern "C" void Avx512CvtF64ToU64_(uint64_t val_cvt[4], double val);
extern "C" void Avx512CvtF64ToF32_(float val_cvt[4], double val);

void ConvertF32ToU32(void)
{
    uint32_t val_cvt[4];
    const float val[] {(float)M_PI, (float)M_SQRT2};
    const int num_vals = sizeof(val) / sizeof(float);

    cout << "\nConvertF32ToU32\n";

    for (int i = 0; i < num_vals; i++)
    {
        Avx512CvtF32ToU32_(val_cvt, val[i]);

        cout << "  Test case #" << i << " val = " << val[i] << '\n';
        cout << "    val_cvt[0] {rn-sae} = " << val_cvt[0] << '\n';
        cout << "    val_cvt[1] {rd-sae} = " << val_cvt[1] << '\n';
        cout << "    val_cvt[2] {ru-sae} = " << val_cvt[2] << '\n';
        cout << "    val_cvt[3] {rz-sae} = " << val_cvt[3] << '\n';
    }
}

void ConvertF64ToU64(void)
{
    uint64_t val_cvt[4];
    const double val[] {(float)M_PI, (float)M_SQRT2};
    const int num_vals = sizeof(val) / sizeof(double);

    cout << "\nConvertF64ToU64\n";

    for (int i = 0; i < num_vals; i++)
    {
        Avx512CvtF64ToU64_(val_cvt, val[i]);

        cout << "  Test case #" << i << " val = " << val[i] << '\n';
        cout << "    val_cvt[0] {rn-sae} = " << val_cvt[0] << '\n';
        cout << "    val_cvt[1] {rd-sae} = " << val_cvt[1] << '\n';
        cout << "    val_cvt[2] {ru-sae} = " << val_cvt[2] << '\n';
        cout << "    val_cvt[3] {rz-sae} = " << val_cvt[3] << '\n';
    }
}

void ConvertF64ToF32(void)
{
    float val_cvt[4];
    const double val[] {M_PI, -M_SQRT2};
    const int num_vals = sizeof(val) / sizeof(double);

    cout << "\nConvertF64ToF32\n";

    for (int i = 0; i < num_vals; i++)
    {
        Avx512CvtF64ToF32_(val_cvt, val[i]);

        cout << fixed << setprecision(7);

        cout << "  Test case #" << i << " val = " << val[i] << '\n';
        cout << "    val_cvt[0] {rn-sae} = " << val_cvt[0] << '\n';
        cout << "    val_cvt[1] {rd-sae} = " << val_cvt[1] << '\n';
        cout << "    val_cvt[2] {ru-sae} = " << val_cvt[2] << '\n';
        cout << "    val_cvt[3] {rz-sae} = " << val_cvt[3] << '\n';
    }
}

int main()
{
    ConvertF32ToU32();
    ConvertF64ToU64();
    ConvertF64ToF32();
    return 0;
}
