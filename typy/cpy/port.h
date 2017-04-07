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
#	define true  1
#endif
#ifndef false
#	define false 0
#endif
#ifndef bool
#	define bool byte
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

typedef unsigned char byte;

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
#	define Ibl_LONGLONG(x)  x##I64
#	define Ibl_ULONGLONG(x) x##UI64
#	define Ibl_LL_FORMAT "I64"  // As in printf("%I64d", ...)
#else
#	define Ibl_LONGLONG(x)  x##LL
#	define Ibl_ULONGLONG(x) x##ULL
#	define Ibl_LL_FORMAT "ll"  // As in "%lld". Note that "q" is poor form also.
#endif

#ifdef _MSC_VER
#	define Ibl_LITTLE_ENDIAN 1
#else
#	include <sys/param.h> // __BYTE_ORDER
#	if ((defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN))
#		define Ibl_LITTLE_ENDIAN 1
#endif
#endif

#define INT32_MAX 0x7FFFFFFF
#define INT32_MIN (-INT32_MAX - 1)
#define INT64_MAX Ibl_LONGLONG(0x7FFFFFFFFFFFFFFF)
#define INT64_MIN (-INT64_MAX - 1)
#define UINT32_MAX 0xFFFFFFFFu
#define UINT64_MAX Ibl_ULONGLONG(0xFFFFFFFFFFFFFFFF)

#define Ibl_Max(a, b) ((a) > (b) ? (a) : (b))
#define Ibl_Min(a, b) ((a) < (b) ? (a) : (b))

typedef struct _bytes {
	byte*  data;
	size_t length;
} *bytes;

IblAPI(size_t) IblBytesHash    (bytes);
IblAPI(int)    IblBytesCompare (bytes, bytes);

typedef char* string;

IblAPI(size_t) IblStringHash    (string*);
IblAPI(int)    IblStringCompare (string*, string*);

IblAPI(uint64) IblUint64    (byte*);
IblAPI(void)   IblPutUint64 (byte*, uint64);

#define MaxVarintLen 10

IblAPI(int)    IblUvarint    (byte*, size_t, uint64*);
IblAPI(size_t) IblPutUvarint (byte*, uint64);
IblAPI(size_t) IblSizeVarint (uint64);

IblAPI(FILE*)  IblFileOpen          (const string);
IblAPI(bool)   IblFileWriteUint64At (FILE*, uint64, size_t);
IblAPI(bool)   IblFileTruncate      (FILE*, size_t);

#ifdef __cplusplus
}
#endif

#endif /* IBELIE_UTILS_H__ */
