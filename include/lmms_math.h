/*
 * lmms_math.h - defines math functions
 *
 * Copyright (c) 2004-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * 
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef LMMS_MATH_H
#define LMMS_MATH_H

#include <QtGlobal>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <numbers>
#include <concepts>

#include "lmms_constants.h"

namespace lmms
{

// TODO C++23: Make constexpr since std::abs() will be constexpr
inline bool approximatelyEqual(float x, float y) noexcept
{
	return x == y || std::abs(x - y) < F_EPSILON;
}

// TODO C++23: Make constexpr since std::trunc() will be constexpr
/*!
 * @brief Returns the fractional part of a float, a value between -1.0f and 1.0f.
 *
 * fraction( 2.3) =>  0.3
 * fraction(-2.3) => -0.3
 *
 * Note that if the return value is used as a phase of an oscillator, that the oscillator must support
 * negative phases.
 */
inline auto fraction(std::floating_point auto x) noexcept
{
	return x - std::trunc(x);
}


// TODO C++23: Make constexpr since std::floor() will be constexpr
/*!
 * @brief Returns the wrapped fractional part of a float, a value between 0.0f and 1.0f.
 *
 * absFraction( 2.3) =>  0.3
 * absFraction(-2.3) =>  0.7
 *
 * Note that this not the same as the absolute value of the fraction (as the function name suggests).
 * If the result is interpreted as a phase of an oscillator, it makes that negative phases are
 * converted to positive phases.
 */
inline auto absFraction(std::floating_point auto x) noexcept
{
	return x - std::floor(x);
}

inline auto fastRand() noexcept
{
	static unsigned long next = 1;
	next = next * 1103515245 + 12345;
	return next / 65536 % 32768;
}

template<std::floating_point T>
inline auto fastRand(T range) noexcept
{
	constexpr T FAST_RAND_RATIO = static_cast<T>(1.0 / 32767);
	return fastRand() * range * FAST_RAND_RATIO;
}

template<std::floating_point T>
inline auto fastRand(T from, T to) noexcept
{
	return from + fastRand(to - from);
}

//! Round `value` to `where` depending on step size
template<class T>
static void roundAt(T& value, const T& where, const T& stepSize)
{
	if (std::abs(value - where) < F_EPSILON * std::abs(stepSize))
	{
		value = where;
	}
}

//! Source: http://martin.ankerl.com/2007/10/04/optimized-pow-approximation-for-java-and-c-c/
inline double fastPow(double a, double b)
{
	double d;
	std::int32_t x[2];

	std::memcpy(x, &a, sizeof(x));
	x[1] = static_cast<std::int32_t>(b * (x[1] - 1072632447) + 1072632447);
	x[0] = 0;

	std::memcpy(&d, x, sizeof(d));
	return d;
}


//! returns +1 if val >= 0, else -1
template<typename T>
constexpr T sign(T val) noexcept
{ 
	return val >= 0 ? 1 : -1; 
}


//! if val >= 0.0f, returns sqrt(val), else: -sqrt(-val)
inline float sqrt_neg(float val) 
{
	return std::sqrt(std::abs(val)) * sign(val);
}

//! @brief Exponential function that deals with negative bases
inline float signedPowf(float v, float e)
{
	return std::pow(std::abs(v), e) * sign(v);
}


//! @brief Scales @value from linear to logarithmic.
//! Value should be within [0,1]
inline float logToLinearScale(float min, float max, float value)
{
	using namespace std::numbers;
	if (min < 0)
	{
		const float mmax = std::max(std::abs(min), std::abs(max));
		const float val = value * (max - min) + min;
		float result = signedPowf(val / mmax, e_v<float>) * mmax;
		return std::isnan(result) ? 0 : result;
	}
	float result = std::pow(value, e_v<float>) * (max - min) + min;
	return std::isnan(result) ? 0 : result;
}


//! @brief Scales value from logarithmic to linear. Value should be in min-max range.
inline float linearToLogScale(float min, float max, float value)
{
	constexpr auto inv_e = static_cast<float>(1.0 / std::numbers::e);
	const float valueLimited = std::clamp(value, min, max);
	const float val = (valueLimited - min) / (max - min);
	if (min < 0)
	{
		const float mmax = std::max(std::abs(min), std::abs(max));
		float result = signedPowf(valueLimited / mmax, inv_e) * mmax;
		return std::isnan(result) ? 0 : result;
	}
	float result = std::pow(val, inv_e) * (max - min) + min;
	return std::isnan(result) ? 0 : result;
}

// TODO C++26: Make constexpr since std::exp() will be constexpr
template<std::floating_point T>
inline auto fastPow10f(T x)
{
	return std::exp(std::numbers::ln10_v<T> * x);
}

// TODO C++26: Make constexpr since std::exp() will be constexpr
inline auto fastPow10f(std::integral auto x)
{
	return std::exp(std::numbers::ln10_v<float> * x);
}

// TODO C++26: Make constexpr since std::log() will be constexpr
inline auto fastLog10f(float x)
{
	constexpr auto inv_ln10 = static_cast<float>(1.0 / std::numbers::ln10);
	return std::log(x) * inv_ln10;
}

//! @brief Converts linear amplitude (>0-1.0) to dBFS scale. 
//! @param amp Linear amplitude, where 1.0 = 0dBFS. ** Must be larger than zero! **
//! @return Amplitude in dBFS. 
inline float ampToDbfs(float amp)
{
	return fastLog10f(amp) * 20.0f;
}


//! @brief Converts dBFS-scale to linear amplitude with 0dBFS = 1.0
//! @param dbfs The dBFS value to convert. ** Must be a real number - not inf/nan! **
//! @return Linear amplitude
inline float dbfsToAmp(float dbfs)
{
	return fastPow10f(dbfs * 0.05f);
}


//! @brief Converts linear amplitude (0-1.0) to dBFS scale. Handles zeroes as -inf.
//! @param amp Linear amplitude, where 1.0 = 0dBFS. 
//! @return Amplitude in dBFS. -inf for 0 amplitude.
inline float safeAmpToDbfs(float amp)
{
	return amp == 0.0f ? -INFINITY : ampToDbfs(amp);
}


//! @brief Converts dBFS-scale to linear amplitude with 0dBFS = 1.0. Handles infinity as zero.
//! @param dbfs The dBFS value to convert: all infinites are treated as -inf and result in 0
//! @return Linear amplitude
inline float safeDbfsToAmp(float dbfs)
{
	return std::isinf(dbfs) ? 0.0f : dbfsToAmp(dbfs);
}


// TODO C++20: use std::formatted_size
// @brief Calculate number of digits which LcdSpinBox would show for a given number
inline int numDigitsAsInt(float f)
{
	// use rounding:
	// LcdSpinBox sometimes uses std::round(), sometimes cast rounding
	// we use rounding to be on the "safe side"
	int asInt = static_cast<int>(std::round(f));
	int digits = 1; // always at least 1
	if(asInt < 0)
	{
		++digits;
		asInt = -asInt;
	}
	// "asInt" is positive from now
	int power = 1;
	for (int i = 1; i < 10; ++i)
	{
		power *= 10;
		if (asInt >= power) { ++digits; } // 2 digits for >=10, 3 for >=100
		else { break; }
	}
	return digits;
}

template <typename T>
class LinearMap
{
public:
	LinearMap(T x1, T y1, T x2, T y2)
	{
		T const dx = x2 - x1;
		assert (dx != T(0));

		m_a = (y2 - y1) / dx;
		m_b = y1 - m_a * x1;
	}

	T map(T x) const
	{
		return m_a * x + m_b;
	}

private:
	T m_a;
	T m_b;
};

} // namespace lmms

#endif // LMMS_MATH_H
