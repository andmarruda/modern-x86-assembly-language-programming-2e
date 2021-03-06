//------------------------------------------------
//               Ch07_06.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <random>
#include "AlignedMem.h"

using namespace std;

// Ch07_06_Misc.cpp
extern uint32_t ConvertImgVerify(const float* src1, const float* src2, uint32_t num_pixels);
extern uint32_t ConvertImgVerify(const uint8_t* src1, const uint8_t* src2, uint32_t num_pixels);

// Ch07_06_.asm
extern "C" bool ConvertImgU8ToF32_(float* des, const uint8_t* src, uint32_t num_pixels);
extern "C" bool ConvertImgF32ToU8_(uint8_t* des, const float* src, uint32_t num_pixels);

extern "C" uint32_t c_NumPixelsMax = 16777216;

template <typename T> void Init(T* x, size_t n, unsigned int seed, T scale)
{
    uniform_int_distribution<> ui_dist {0, 255};
    default_random_engine rng {seed};

    for (size_t i = 0; i < n; i++)
    {
        T temp = (T)ui_dist(rng);
        x[i] = (scale == 1) ? temp : temp / scale;
    }
}

bool ConvertImgU8ToF32Cpp(float* des, const uint8_t* src, uint32_t num_pixels)
{
    // Make sure num_pixels is valid
    if ((num_pixels == 0) || (num_pixels > c_NumPixelsMax))
        return false;
    if ((num_pixels % 32) != 0)
        return false;

    // Make sure src and des are aligned to a 16-byte boundary
    if (!AlignedMem::IsAligned(src, 16))
        return false;
    if (!AlignedMem::IsAligned(des, 16))
        return false;

    // Convert the image
    for (uint32_t i = 0; i < num_pixels; i++)
        des[i] = src[i] / 255.0f;

    return true;
}

bool ConvertImgF32ToU8Cpp(uint8_t* des, const float* src, uint32_t num_pixels)
{
    // Make sure num_pixels is valid
    if ((num_pixels == 0) || (num_pixels > c_NumPixelsMax))
        return false;
    if ((num_pixels % 32) != 0)
        return false;

    // Make sure src and des are aligned to a 16-byte boundary
    if (!AlignedMem::IsAligned(src, 16))
        return false;
    if (!AlignedMem::IsAligned(des, 16))
        return false;

    for (uint32_t i = 0; i < num_pixels; i++)
    {
        if (src[i] > 1.0f)
            des[i] = 255;
        else if (src[i] < 0.0)
            des[i] = 0;
        else
            des[i] = (uint8_t)(src[i] * 255.0f);
    }

    return true;
}

void ConvertImgU8ToF32(void)
{
    const uint32_t num_pixels = 1024;
    AlignedArray<uint8_t> src_aa(num_pixels, 16);
    AlignedArray<float> des1_aa(num_pixels, 16);
    AlignedArray<float> des2_aa(num_pixels, 16);
    uint8_t* src = src_aa.Data();
    float* des1 = des1_aa.Data();
    float* des2 = des2_aa.Data();

    Init(src, num_pixels, 12, (uint8_t)1);

    bool rc1 = ConvertImgU8ToF32Cpp(des1, src, num_pixels);
    bool rc2 = ConvertImgU8ToF32_(des2, src, num_pixels);

    if (!rc1 || !rc2)
    {
        cout << "Invalid return code - ";
        cout << "rc1 = " << boolalpha << rc1 << ", ";
        cout << "rc2 = " << boolalpha << rc2 << '\n';
        return;
    }

    uint32_t num_diff = ConvertImgVerify(des1, des2, num_pixels);
    cout << "\nResults for ConvertImgU8ToF32\n";
    cout << "  num_pixels = " << num_pixels << '\n';
    cout << "  num_diff = " << num_diff << '\n';
}

void ConvertImgF32ToU8(void)
{
    const uint32_t num_pixels = 1024;
    AlignedArray<float> src_aa(num_pixels, 16);
    AlignedArray<uint8_t> des1_aa(num_pixels, 16);
    AlignedArray<uint8_t> des2_aa(num_pixels, 16);
    float* src = src_aa.Data();
    uint8_t* des1 = des1_aa.Data();
    uint8_t* des2 = des2_aa.Data();

    // Initialize the src pixel buffer. The first few entries in src
    // are set to known values for test purposes.

    Init(src, num_pixels, 20, 1.0f);

    src[0] = 0.125f;        src[8] = 0.01f;
    src[1] = 0.75f;         src[9] = 0.99f;
    src[2] = -4.0f;         src[10] = 1.1f;
    src[3] = 3.0f;          src[11] = -1.1f;
    src[4] = 0.0f;          src[12] = 0.99999f;
    src[5] = 1.0f;          src[13] = 0.5f;
    src[6] = -0.01f;        src[14] = -0.0;
    src[7] = 1.01f;         src[15] = .333333f;

    bool rc1 = ConvertImgF32ToU8Cpp(des1, src, num_pixels);
    bool rc2 = ConvertImgF32ToU8_(des2, src, num_pixels);

    if (!rc1 || !rc2)
    {
        cout << "Invalid return code - ";
        cout << "rc1 = " << boolalpha << rc1 << ", ";
        cout << "rc2 = " << boolalpha << rc2 << '\n';
        return;
    }

    uint32_t num_diff = ConvertImgVerify(des1, des2, num_pixels);
    cout << "\nResults for ConvertImgF32ToU8\n";
    cout << "  num_pixels = " << num_pixels << '\n';
    cout << "  num_diff = " << num_diff << '\n';
}

int main()
{
    ConvertImgU8ToF32();
    ConvertImgF32ToU8();
    return 0;
}
