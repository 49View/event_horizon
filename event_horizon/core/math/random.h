#pragma once

#include "../util.h"

// Simple and fast pseudo-random number generator, based on a linear congruential generator algorithm.
// http://en.wikipedia.org/wiki/Linear_congruential_generator

class Random {
public:
	Random( uint32_t seed ) {
		setSeed( seed );
	}

	Random() {
		setSeed( 123456789 );
	}

	void setSeed( uint32_t seed ) {
		mSeed = seed;
		mSeed = mSeed * RandMul + RandAdd; // Advance the seed immediately, otherwise it simply gets returned as the first value
	}

	// ================================
	// float methods
	// ================================

	// Returns a random float, in the range 0.0 (inclusive) to 1.0 (exclusive)
	float getFloat() {
		//float value = float(mSeed) * (1.0f / float(uint64_t(1) << 32)); // Note: this can get rounded up to 1.0f due to float precision.
		float value = float( mSeed >> ( 32 - 24 ) ) * ( 1.0f / float( 1 << 24 ) ); // This implementation ensures 0 <= value < 1.0f. Assumes standard float format with 24-bit mantissa (including implicit leading bit).
		mSeed = mSeed * RandMul + RandAdd;
		return value;
	}

	// Returns a random float, in the range min (inclusive) to max (exclusive). (Although note the result may be inclusive of max, due to rounding).
	float getFloat( float min, float max ) {
		return getFloat() * ( max - min ) + min;
	}

	// ================================
	// 32-bit integer methods
	// ================================

	// returns a random signed integer across the entire 32-bit range (INT_MIN to INT_MAX inclusive)
	int getInt() {
		return intGet<int>();
	}
	// returns a random unsigned integer across the entire 32-bit range (0 to 0xffffffff inclusive)
	uint32_t getUInt() {
		return intGet<uint32_t>();
	}
	// returns a random signed integer in the range min to max inclusive
	int getInt( int min, int max ) {
		return intGet( min, max );
	}
	// returns a random unsigned integer in the range min to max inclusive
	uint32_t getUInt( uint32_t min, uint32_t max ) {
		return intGet( min, max );
	}

	// ================================
	// 16-bit integer methods
	// These should be used in preference to masking 32-bit random values, as they provide better randomness (longer period).
	// For example, getUInt32()&0xffff has a period of, at most, 2^16, while getUInt16() has a period of 2^32.
	// In particular, lower bits are greatly affected. For example, getUInt32()&1 has a period of 2, while getUInt16()&1 has a period of 2^17.
	// ================================

	// returns a random signed integer across the entire 16-bit range (-32768 to 32767 inclusive)
	int16_t getInt16() {
		return intGet<int16_t>();
	}
	// returns a random signed integer across the entire 16-bit range (0 to 0xffff inclusive)
	uint16_t getUInt16() {
		return intGet<uint16_t>();
	}
	// returns a random signed integer in the range min to max inclusive
	int16_t getInt16( int16_t min, int16_t max ) {
		return intGet( min, max );
	}
	// returns a random unsigned integer in the range min to max inclusive
	uint16_t getUInt16( uint16_t min, uint16_t max ) {
		return intGet( min, max );
	}

	// ================================
	// 8-bit integer methods
	// These should be used in preference to masking 16- or 32-bit random values, as they provide better randomness (longer period).
	// For example, getUInt32()&0xff has a period of, at most, 256, while getUInt8() has a period of 2^32.
	// In particular, lower bits are greatly affected. For example, getUInt32()&1 has a period of 2, while getUInt8()&1 has a period of 2^25.
	// ================================

	// returns a random signed integer across the entire 8-bit range (-128 to 127 inclusive)
	int8_t getInt8() {
		return intGet<int8_t>();
	}
	// returns a random unsigned integer across the entire 8-bit range (0 to 255 inclusive)
	uint8_t getUInt8() {
		return intGet<uint8_t>();
	}
	// returns a random signed integer in the range min to max inclusive
	int8_t getInt8( int8_t min, int8_t max ) {
		return intGet( min, max );
	}
	// returns a random unsigned integer in the range min to max inclusive
	uint8_t getUInt8( uint8_t min, uint8_t max ) {
		return intGet( min, max );
	}

	// ================================
	// bool methods
	// This method should be used in preference to masking random integer values, as it provide better randomness (longer period).
	// For example, getUInt32()&1 will result in a 0,1,0,1,0,1,... pattern (period of 2), while getBool() has a period of 2^32.
	// ================================

	bool getBool() {
		// Use the top bit, as it is the most random. (Note: if we had used the LSB, it would simply alternate between 0 and 1 on each iteration)
		bool value = ( ( mSeed & 0x80000000u ) != 0 );
		mSeed = mSeed * RandMul + RandAdd;
		return value;
	}

private:
	template<typename T>
	T intGet() {
#ifdef D_LANGUAGE_CXX11
		static_assert( sizeof( T ) == 1 || sizeof( T ) == 2 || sizeof( T ) == 4, "Expected 8-, 16- or 32-bit integer type" );
#endif
		T value = T( mSeed >> ( 32 - sizeof( T ) * 8 ) ); // Use higher bits, as they have better randomness
		mSeed = mSeed * RandMul + RandAdd; // Advance the seed AFTER calculating our value. This reduces latency and can therefore improve performance.
		return value;
	}

	template<typename T>
	T intGet( T min, T max ) {
#ifdef D_LANGUAGE_CXX11
		static_assert( sizeof( T ) == 1 || sizeof( T ) == 2 || sizeof( T ) == 4, "Expected 8-, 16- or 32-bit integer type" );
#endif
		ASSERT( max >= min );
		T value = min + T( ( uint64_t( mSeed ) * ( uint64_t( max - min ) + 1 ) ) >> 32 );
		mSeed = mSeed * RandMul + RandAdd; // Advance the seed AFTER calculating our value. This reduces latency and can therefore improve performance.
		return value;
	}

	static const uint32_t RandMul = 1103515245;
	static const uint32_t RandAdd = 12345;

	uint32_t mSeed;
};
