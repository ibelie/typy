// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#ifndef TYPY_PROTOBUF_H__
#define TYPY_PROTOBUF_H__

#include "port.h"

#define MaxVarint32Len 5

#define TAG_TYPE_BITS 3
#define TAG_TYPE_MASK ((1 << TAG_TYPE_BITS) - 1)
#define MAKE_TAG(FIELD_NUMBER, TYPE) ((uint32)(((FIELD_NUMBER) << TAG_TYPE_BITS) | (TYPE)))
#define MAX_TAG(TAG) MAKE_TAG(TAG, TAG_TYPE_MASK)
#define TAG_CUTOFF(TAG) (MAX_TAG(TAG) <= 0x7F ? 0x7F : (MAX_TAG(TAG) <= 0x3FFF ? 0x3FFF : MAX_TAG(TAG)))
#define TAG_INDEX(TAG) ((size_t)(((TAG) >> TAG_TYPE_BITS) - 1))
#define TAG_WIRETYPE(TAG) ((WireType)((TAG) & TAG_TYPE_MASK))

#ifdef __cplusplus
extern "C" {
#endif

inline bool Typy_ReadVarint(byte** buffer, size_t* buf_len, uint64* x) {
	register int size = IblUvarint(*buffer, *buf_len, x);
	*buffer += size >= 0 ? size : 0;
	*buf_len -= size >= 0 ? size : 0;
	return size >= 0;
}

inline bool Typy_Read64(byte** buffer, size_t* buf_len, uint64* x) {
	register size_t size = *buf_len;
#ifdef Ibl_LITTLE_ENDIAN
	if (size >= sizeof(*x)) {
		size = sizeof(*x);
	} else {
		*x = 0;
	}
	memcpy(x, *buffer, size);
#else
	byte bytes[sizeof(*x)] = {0};
	if (size >= sizeof(*x)) {
		size = sizeof(*x);
	}
	memcpy(bytes, *buffer, size);
	register uint32 part0 = ((uint32)(bytes[0])      ) |
							((uint32)(bytes[1]) <<  8) |
							((uint32)(bytes[2]) << 16) |
							((uint32)(bytes[3]) << 24);
	register uint32 part1 = ((uint32)(bytes[4])      ) |
							((uint32)(bytes[5]) <<  8) |
							((uint32)(bytes[6]) << 16) |
							((uint32)(bytes[7]) << 24);
	*x = (uint64)(part0) | ((uint64)(part1) << 32);
#endif
	*buffer += size;
	*buf_len -= size;
	return true;
}

inline bool Typy_Read32(byte** buffer, size_t* buf_len, uint32* x) {
	register size_t size = *buf_len;
#ifdef Ibl_LITTLE_ENDIAN
	if (size >= sizeof(*x)) {
		size = sizeof(*x);
	} else {
		*x = 0;
	}
	memcpy(x, *buffer, size);
#else
	byte bytes[sizeof(*x)] = {0};
	if (size >= sizeof(*x)) {
		size = sizeof(*x);
	}
	memcpy(bytes, *buffer, size);
	*x = (uint32)(bytes[0])        |
		((uint32)(bytes[1]) <<  8) |
		((uint32)(bytes[2]) << 16) |
		((uint32)(bytes[3]) << 24);
#endif
	*buffer += size;
	*buf_len -= size;
	return true;
}

inline size_t Typy_WriteTag(byte* buffer, uint32 tag) {
	register size_t i = 0;
	while (tag >= 0x80) {
		buffer[i] = (byte)tag | 0x80;
		tag >>= 7;
		i++;
	}
	buffer[i] = (byte)tag;
	return i + 1;
}

inline bool _Typy_ReadTag(byte** buffer, size_t* buf_len, uint32* tag) {
	if ((*buf_len) >= MaxVarintLen || !((*buffer)[(*buf_len) - 1] & 0x80)) {
		// Optimization:  We're also safe if the buffer is non-empty and it ends
		// with a byte that would terminate a varint.
		register byte* ptr = *buffer;
		register uint32 b;
		// We just processed the first byte.  Move on to the second.
		register uint32 result = *(ptr++) - 0x80;
		b = *(ptr++); result += b <<  7; if (!(b & 0x80)) { goto done; }
		result -= 0x80 << 7;
		b = *(ptr++); result += b << 14; if (!(b & 0x80)) { goto done; }
		result -= 0x80 << 14;
		b = *(ptr++); result += b << 21; if (!(b & 0x80)) { goto done; }
		result -= 0x80 << 21;
		b = *(ptr++); result += b << 28; if (!(b & 0x80)) { goto done; }
		// "result -= 0x80 << 28" is irrevelant.

		// If the input is larger than 32 bits, we still need to read it all
		// and discard the high-order bits.
		for (int i = 0; i < MaxVarintLen - MaxVarint32Len; i++) {
		  b = *(ptr++); if (!(b & 0x80)) { goto done; }
		}

		// We have overrun the maximum size of a varint (10 bytes).  Assume
		// the data is corrupt.
		*tag = 0;
		return false;

	done:
		*tag = result;
		*buf_len -= (size_t)(ptr - *buffer);
		*buffer = ptr;
		return true;
	} else {
		uint64 temp;
		register int size = IblUvarint(*buffer, *buf_len, &temp);
		*tag = (uint32)temp;
		*buffer += size >= 0 ? size : 0;
		*buf_len -= size >= 0 ? size : 0;
		return size >= 0;
	}
}


inline bool Typy_ReadTag(byte** buffer, size_t* buf_len, uint32* tag, uint32 cutoff) {
	// In performance-sensitive code we can expect cutoff to be a compile-time
	// constant, and things like "cutoff >= 0x7F" to be evaluated at
	// compile time.
	if ((*buf_len) > 0) {
		register uint32 b1, b2;
		register byte* ptr = *buffer;
		// Hot case: buffer non_empty, buffer[0] in [1, 128).
		if (!((b1 = *(ptr++)) & 0x80)) {
			if (cutoff >= 0x7F || b1 <= cutoff) {
				*tag = b1;
				*buffer = ptr;
				*buf_len -= 1;
				return true;
			} else {
				*tag = 0;
				return false;
			}
		}
		// Other hot case: cutoff >= 0x80, buffer has at least two bytes available,
		// and tag is two bytes.  The latter is tested by bitwise-and-not of the
		// first byte and the second byte.
		if (cutoff >= 0x80 && (*buf_len) > 1 && !((b2 = *(ptr++)) & 0x80)) {
			b1 = (b2 << 7) + (b1 - 0x80);
			if (cutoff >= 0x3FFF || b1 <= cutoff) {
				*tag = b1;
				*buffer = ptr;
				*buf_len -= 2;
				return true;
			} else {
				*tag = 0;
				return false;
			}
		}
	}
	// Slow path
	return _Typy_ReadTag(buffer, buf_len, tag);
}

typedef enum {
	WIRETYPE_VARINT           = 0,
	WIRETYPE_FIXED64          = 1,
	WIRETYPE_LENGTH_DELIMITED = 2,
	WIRETYPE_START_GROUP      = 3,
	WIRETYPE_END_GROUP        = 4,
	WIRETYPE_FIXED32          = 5,
} WireType;

// Lite alternative to FieldDescriptor::Type.  Must be kept in sync.
typedef enum {
	TYPE_DOUBLE         = 1,
	TYPE_FLOAT          = 2,
	TYPE_INT64          = 3,
	TYPE_UINT64         = 4,
	TYPE_INT32          = 5,
	TYPE_FIXED64        = 6,
	TYPE_FIXED32        = 7,
	TYPE_BOOL           = 8,
	TYPE_STRING         = 9,
	TYPE_GROUP          = 10,
	TYPE_MESSAGE        = 11,
	TYPE_BYTES          = 12,
	TYPE_UINT32         = 13,
	TYPE_ENUM           = 14,
	TYPE_SFIXED32       = 15,
	TYPE_SFIXED64       = 16,
	TYPE_SINT32         = 17,
	TYPE_SINT64         = 18,
	MAX_FIELD_TYPE      = 18,
} FieldType;

inline bool Typy_SkipField(byte** buffer, size_t* buf_len, uint32 tag) {
	uint64 value;
	switch (TAG_WIRETYPE(tag)) {
		case WIRETYPE_VARINT:
			return Typy_ReadVarint(buffer, buf_len, &value);
		case WIRETYPE_FIXED64:
			return Typy_Read64(buffer, buf_len, &value);
		case WIRETYPE_FIXED32:
			return Typy_Read32(buffer, buf_len, (uint32*)&value);
		case WIRETYPE_LENGTH_DELIMITED:
			if (!Typy_ReadVarint(buffer, buf_len, &value)) { return false; }
			register uint32 length = (uint32)value;
			if (*buf_len < length) { return false; }
			*buffer += length;
			*buf_len -= length;
			return true;
		default:
			return false;
	}
}

#ifdef __cplusplus
}
#endif

#endif // TYPY_PROTOBUF_H__
