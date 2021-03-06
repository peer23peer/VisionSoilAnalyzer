/* Copyright (C) Jelle Spijker - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * and only allowed with the written consent of the author (Jelle Spijker)
 * This software is proprietary and confidential
 * Written by Jelle Spijker <spijker.jelle@gmail.com>, 2015
 */

#pragma once
#define COMMONOPERATIONS_VERSION 1

#include <algorithm>
#include <stdint.h>
#include <math.h>
#include <vector>

namespace SoilMath {
inline uint16_t MinNotZero(uint16_t a, uint16_t b) {
  if (a != 0 && b != 0) {
    return (a < b) ? a : b;
  } else {
    return (a > b) ? a : b;
  }
}

inline uint16_t Max(uint16_t a, uint16_t b) { return (a > b) ? a : b; }

inline uint16_t Max(uint16_t a, uint16_t b, uint16_t c, uint16_t d) {
  return (Max(a, b) > Max(c, d)) ? Max(a, b) : Max(c, d);
}

inline uint16_t Min(uint16_t a, uint16_t b) { return (a < b) ? a : b; }

inline uint16_t Min(uint16_t a, uint16_t b, uint16_t c, uint16_t d) {
  return (Min(a, b) > Min(c, d)) ? Min(a, b) : Min(c, d);
}

static inline double quick_pow10(int n) {
  static double pow10[19] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000,
                             100000000, 1000000000, 10000000000, 100000000000,
                             1000000000000, 10000000000000, 100000000000000,
                             1000000000000000, 10000000000000000,
                             100000000000000000, 1000000000000000000};
  return pow10[(n >= 0) ? n : -n];
}


// Source:
// http://martin.ankerl.com/2012/01/25/optimized-approximative-pow-in-c-and-cpp/
static inline double fastPow(double a, double b) {
  union {
    double d;
    int x[2];
  } u = {a};
  u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
}

static inline double quick_pow2(int n) {
  static double pow2[256] = {
      0,     1,     4,     9,     16,    25,    36,    49,    64,    81,
      100,   121,   144,   169,   196,   225,   256,   289,   324,   361,
      400,   441,   484,   529,   576,   625,   676,   729,   784,   841,
      900,   961,   1024,  1089,  1156,  1225,  1296,  1369,  1444,  1521,
      1600,  1681,  1764,  1849,  1936,  2025,  2116,  2209,  2304,  2401,
      2500,  2601,  2704,  2809,  2916,  3025,  3136,  3249,  3364,  3481,
      3600,  3721,  3844,  3969,  4096,  4225,  4356,  4489,  4624,  4761,
      4900,  5041,  5184,  5329,  5476,  5625,  5776,  5929,  6084,  6241,
      6400,  6561,  6724,  6889,  7056,  7225,  7396,  7569,  7744,  7921,
      8100,  8281,  8464,  8649,  8836,  9025,  9216,  9409,  9604,  9801,
      10000, 10201, 10404, 10609, 10816, 11025, 11236, 11449, 11664, 11881,
      12100, 12321, 12544, 12769, 12996, 13225, 13456, 13689, 13924, 14161,
      14400, 14641, 14884, 15129, 15376, 15625, 15876, 16129, 16384, 16641,
      16900, 17161, 17424, 17689, 17956, 18225, 18496, 18769, 19044, 19321,
      19600, 19881, 20164, 20449, 20736, 21025, 21316, 21609, 21904, 22201,
      22500, 22801, 23104, 23409, 23716, 24025, 24336, 24649, 24964, 25281,
      25600, 25921, 26244, 26569, 26896, 27225, 27556, 27889, 28224, 28561,
      28900, 29241, 29584, 29929, 30276, 30625, 30976, 31329, 31684, 32041,
      32400, 32761, 33124, 33489, 33856, 34225, 34596, 34969, 35344, 35721,
      36100, 36481, 36864, 37249, 37636, 38025, 38416, 38809, 39204, 39601,
      40000, 40401, 40804, 41209, 41616, 42025, 42436, 42849, 43264, 43681,
      44100, 44521, 44944, 45369, 45796, 46225, 46656, 47089, 47524, 47961,
      48400, 48841, 49284, 49729, 50176, 50625, 51076, 51529, 51984, 52441,
      52900, 53361, 53824, 54289, 54756, 55225, 55696, 56169, 56644, 57121,
      57600, 58081, 58564, 59049, 59536, 60025, 60516, 61009, 61504, 62001,
      62500, 63001, 63504, 64009, 64516, 65025};
  return pow2[(n >= 0) ? n : -n];
}

static inline long float2intRound(double d) {
  d += 6755399441055744.0;
  return reinterpret_cast<int &>(d);
}

/*!
 * \brief calcVolume according to ISO 9276-6
 * \param A
 * \return
 */
static inline float calcVolume(float A) {
  return (pow(A, 1.5)) / 10.6347f;
}

static inline std::vector<float> makeOutput(uint8_t value, uint32_t noNeurons) {
  std::vector<float> retVal(noNeurons, -1);
  retVal[value - 1] = 1;
  return retVal;
}

/*!
 * \brief calcDiameter according to ISO 9276-6
 * \param A
 * \return
 */
static inline float calcDiameter(float A) {
  //return sqrt((4 * A) / M_PI);
  return 1.1283791670955 * sqrt(A);
}
}
