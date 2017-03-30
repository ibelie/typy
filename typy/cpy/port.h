// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef IBELIE_UTILS_H__
#define IBELIE_UTILS_H__

#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "string.h"

#define SINGLE_ARG(...) __VA_ARGS__

#ifndef true
#define true  1
#endif
#ifndef false
#define false 0
#endif
#ifndef bool
#define bool byte
#endif

#undef NULL
#ifdef __cplusplus
#	define NULL 0
#else
#	define NULL ((void *)0)
#endif

#ifndef IblAPI
#	define IblAPI(RTYPE) extern RTYPE
#endif

#ifdef _MSC_VER
#	define IBL_ALIGN(X, FIELD) __declspec(align(X)) FIELD
#else
#	define IBL_ALIGN(X, FIELD) FIELD __attribute__((aligned(X)))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER

typedef unsigned __int8 byte;

typedef signed __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

#else

typedef unsigned char byte

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

#endif

// long long macros to be used because gcc and vc++ use different suffixes,
// and different size specifiers in format strings
#ifdef _MSC_VER
#define Ibl_LONGLONG(x)  x##I64
#define Ibl_ULONGLONG(x) x##UI64
#define Ibl_LL_FORMAT "I64"  // As in printf("%I64d", ...)
#else
#define Ibl_LONGLONG(x)  x##LL
#define Ibl_ULONGLONG(x) x##ULL
#define Ibl_LL_FORMAT "ll"  // As in "%lld". Note that "q" is poor form also.
#endif

#define INT32_MAX 0x7FFFFFFF
#define INT32_MIN (-INT32_MAX - 1)
#define INT64_MAX Ibl_LONGLONG(0x7FFFFFFFFFFFFFFF)
#define INT64_MIN (-INT64_MAX - 1)
#define UINT32_MAX 0xFFFFFFFFu
#define UINT64_MAX Ibl_ULONGLONG(0xFFFFFFFFFFFFFFFF)

typedef struct _bytes {
	byte*  data;
	size_t length;
} *bytes;

inline IblAPI(size_t) IblBytesHash(bytes buffer) {
	register size_t hash = 0;
	register byte *i, *n = buffer->data + buffer->length;
	for (i = buffer->data; i < n; i++) {
		hash = (hash << 5) - hash + *i;
	}
	return hash;
}

inline IblAPI(int) IblBytesCompare(bytes k1, bytes k2) {
	if (k1->length == k2->length) {
		return memcmp(k1->data, k2->data, k1->length);
	} else {
		return k1->length - k2->length;
	}
}

typedef char* string;

inline IblAPI(size_t) IblStringHash(string* str) {
	register size_t hash = 0;
	register char* i;
	for (i = *str; *i != '\0'; i++) {
		hash = (hash << 5) - hash + *i;
	}
	return hash;
}

inline IblAPI(int) IblStringCompare(string* s1, string* s2) {
	return strcmp(*s1, *s2);
}

inline IblAPI(uint64) IblUint64(byte* b) {
	return ((uint64)(b[7])) | ((uint64)(b[6])<<8) | ((uint64)(b[5])<<16) | ((uint64)(b[4])<<24) |
		((uint64)(b[3])<<32) | ((uint64)(b[2])<<40) | ((uint64)(b[1])<<48) | ((uint64)(b[0])<<56);
}

inline IblAPI(void) IblPutUint64(byte* b, uint64 v) {
	b[0] = (byte)(v >> 56);
	b[1] = (byte)(v >> 48);
	b[2] = (byte)(v >> 40);
	b[3] = (byte)(v >> 32);
	b[4] = (byte)(v >> 24);
	b[5] = (byte)(v >> 16);
	b[6] = (byte)(v >> 8);
	b[7] = (byte)(v);
}

#define MaxVarintLen 10

inline IblAPI(size_t) IblSizeVarint(uint64 x) {
	register size_t n = 0;
	do { n++; x >>= 7; } while (x);
	return n;
}

inline IblAPI(int) IblUvarint(byte* buffer, size_t buf_len, uint64* x) {
	register uint64 y = 0;
	register size_t s = 0, i;
	if (buf_len > MaxVarintLen) { buf_len = MaxVarintLen; }
	for (i = 0; i < buf_len; i++) {
		register byte b = buffer[i];
		if (b < 0x80) {
			if (i > 9 || i == 9 && b > 1) {
				/* overflow */
				*x = 0;
				return -((int)i + 1);
			}
			*x = y | (uint64)(b) << s;
			return i + 1;
		}
		y |= (uint64)(b & 0x7f) << s;
		s += 7;
	}
	*x = 0;
	return 0;
}

inline IblAPI(size_t) IblPutUvarint(byte* buffer, uint64 x) {
	register size_t i = 0;
	while (x >= 0x80) {
		buffer[i] = (byte)x | 0x80;
		x >>= 7;
		i++;
	}
	buffer[i] = (byte)x;
	return i + 1;
}

IblAPI(FILE*) IblFileOpen(const string);
IblAPI(bool)  IblFileWriteUint64At(FILE*, uint64, size_t);
IblAPI(bool)  IblFileTruncate(FILE*, size_t);

#ifdef __cplusplus
}
#endif

#endif /* IBELIE_UTILS_H__ */
