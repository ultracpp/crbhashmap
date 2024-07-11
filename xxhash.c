#include "xxhash.h"

#define PRIME64_1 0x9E3779B185EBCA87ULL
#define PRIME64_2 0xC2B2AE3D27D4EB4FULL
#define PRIME64_3 0x165667B19E3779F9ULL
#define PRIME64_4 0x85EBCA77C2B2AE63ULL
#define PRIME64_5 0x27D4EB2F165667C5ULL

static uint64_t XXH_rotl64(uint64_t x, int r) {
	return (x << r) | (x >> (64 - r));
}

static uint64_t XXH64_finalize(uint64_t h, const uint8_t* ptr, size_t len) {
	while (len >= 8) {
		uint64_t k1 = *(const uint64_t*)ptr;
		k1 *= PRIME64_2;
		k1 = XXH_rotl64(k1, 31);
		k1 *= PRIME64_1;
		h ^= k1;
		h = XXH_rotl64(h, 27) * PRIME64_1 + PRIME64_4;
		ptr += 8;
		len -= 8;
	}

	if (len >= 4) {
		h ^= (*(const uint32_t*)ptr) * PRIME64_1;
		h = XXH_rotl64(h, 23) * PRIME64_2 + PRIME64_3;
		ptr += 4;
		len -= 4;
	}

	while (len > 0) {
		h ^= (*ptr++) * PRIME64_5;
		h = XXH_rotl64(h, 11) * PRIME64_1;
		len--;
	}

	h ^= h >> 33;
	h *= PRIME64_2;
	h ^= h >> 29;
	h *= PRIME64_3;
	h ^= h >> 32;

	return h;
}

uint64_t XXH64(const void* input, size_t length, uint64_t seed) {
	const uint8_t* p = (const uint8_t*)input;
	const uint8_t* bEnd = p + length;
	uint64_t h64;

	if (length >= 32) {
		const uint8_t* const limit = bEnd - 32;
		uint64_t v1 = seed + PRIME64_1 + PRIME64_2;
		uint64_t v2 = seed + PRIME64_2;
		uint64_t v3 = seed + 0;
		uint64_t v4 = seed - PRIME64_1;

		do {
			v1 += (*(const uint64_t*)p) * PRIME64_2;
			v1 = XXH_rotl64(v1, 31);
			v1 *= PRIME64_1;
			p += 8;
			v2 += (*(const uint64_t*)p) * PRIME64_2;
			v2 = XXH_rotl64(v2, 31);
			v2 *= PRIME64_1;
			p += 8;
			v3 += (*(const uint64_t*)p) * PRIME64_2;
			v3 = XXH_rotl64(v3, 31);
			v3 *= PRIME64_1;
			p += 8;
			v4 += (*(const uint64_t*)p) * PRIME64_2;
			v4 = XXH_rotl64(v4, 31);
			v4 *= PRIME64_1;
			p += 8;
		} while (p <= limit);

		h64 = XXH_rotl64(v1, 1) + XXH_rotl64(v2, 7) + XXH_rotl64(v3, 12) + XXH_rotl64(v4, 18);

		v1 *= PRIME64_2;
		v1 = XXH_rotl64(v1, 31);
		v1 *= PRIME64_1;
		h64 ^= v1;
		h64 = h64 * PRIME64_1 + PRIME64_4;

		v2 *= PRIME64_2;
		v2 = XXH_rotl64(v2, 31);
		v2 *= PRIME64_1;
		h64 ^= v2;
		h64 = h64 * PRIME64_1 + PRIME64_4;

		v3 *= PRIME64_2;
		v3 = XXH_rotl64(v3, 31);
		v3 *= PRIME64_1;
		h64 ^= v3;
		h64 = h64 * PRIME64_1 + PRIME64_4;

		v4 *= PRIME64_2;
		v4 = XXH_rotl64(v4, 31);
		v4 *= PRIME64_1;
		h64 ^= v4;
		h64 = h64 * PRIME64_1 + PRIME64_4;
	} else {
		h64 = seed + PRIME64_5;
	}

	h64 += (uint64_t)length;

	return XXH64_finalize(h64, p, length);
}

