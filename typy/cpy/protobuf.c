// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#include "protobuf.h"

#ifdef __cplusplus
extern "C" {
#endif

bool Typy_ReadByte(byte** buffer, size_t* buf_len, byte* x) {
	if (*buf_len < 1) { return false; }
	*x = ((*buffer)++)[0];
	(*buf_len)--;
	return true;
}

bool Typy_ReadVarint32(byte** buffer, size_t* buf_len, uint32* x) {
	uint64 y;
	register int size = IblUvarint(*buffer, *buf_len, &y);
	if (size >= 0 && (size_t)size <= *buf_len) {
		*buffer += size;
		*buf_len -= size;
		*x = (uint32)y;
		return true;
	} else {
		return false;
	}
}

bool Typy_ReadVarint64(byte** buffer, size_t* buf_len, uint64* x) {
	register int size = IblUvarint(*buffer, *buf_len, x);
	if (size >= 0 && (size_t)size <= *buf_len) {
		*buffer += size;
		*buf_len -= size;
		return true;
	} else {
		return false;
	}
}

bool Typy_Read64(byte** buffer, size_t* buf_len, uint64* x) {
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

bool Typy_Read32(byte** buffer, size_t* buf_len, uint32* x) {
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

size_t Typy_WriteByte(byte* buffer, byte* value) {
	buffer[0] = *value;
	return sizeof(*value);
}

size_t Typy_Write32(byte* buffer, uint32* value) {
#ifdef Ibl_LITTLE_ENDIAN
	memcpy(buffer, value, sizeof(*value));
#else
	buffer[0] = (byte)(*value);
	buffer[1] = (byte)(*value >>  8);
	buffer[2] = (byte)(*value >> 16);
	buffer[3] = (byte)(*value >> 24);
#endif
	return sizeof(*value);
}

size_t Typy_Write64(byte* buffer, uint64* value) {
#ifdef Ibl_LITTLE_ENDIAN
	memcpy(buffer, value, sizeof(*value));
#else
	uint32 part0 = (uint32)(*value);
	uint32 part1 = (uint32)(*value >> 32);

	buffer[0] = (byte)(part0);
	buffer[1] = (byte)(part0 >>  8);
	buffer[2] = (byte)(part0 >> 16);
	buffer[3] = (byte)(part0 >> 24);
	buffer[4] = (byte)(part1);
	buffer[5] = (byte)(part1 >>  8);
	buffer[6] = (byte)(part1 >> 16);
	buffer[7] = (byte)(part1 >> 24);
#endif
	return sizeof(*value);
}

size_t Typy_WriteTag(byte* buffer, uint32 tag) {
	register size_t i = 0;
	while (tag >= 0x80) {
		buffer[i] = (byte)tag | 0x80;
		tag >>= 7;
		i++;
	}
	buffer[i] = (byte)tag;
	return i + 1;
}

static inline bool _Typy_ReadTag(byte** buffer, size_t* buf_len, uint32* tag) {
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
		register int i;
		for (i = 0; i < MaxVarintLen - MaxVarint32Len; i++) {
		  b = *(ptr++); if (!(b & 0x80)) { goto done; }
		}

		// We have overrun the maximum size of a varint (10 bytes).  Assume
		// the data is corrupt.
		*tag = 0;
		return false;

	done:
		if ((size_t)(ptr - *buffer) > *buf_len) {
			*tag = 0;
			return false;
		}
		*tag = result;
		*buf_len -= (size_t)(ptr - *buffer);
		*buffer = ptr;
		return true;
	} else {
		uint64 temp;
		register int size = IblUvarint(*buffer, *buf_len, &temp);
		if (size < 0 || (size_t)size > *buf_len) {
			*tag = 0;
			return false;
		}
		*tag = (uint32)temp;
		*buffer += size;
		*buf_len -= size;
		return true;
	}
}


bool Typy_ReadTag(byte** buffer, size_t* buf_len, uint32* tag, uint32 cutoff) {
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
	} else if (buf_len <= 0) {
		return false;
	}
	// Slow path
	return _Typy_ReadTag(buffer, buf_len, tag);
}

bool Typy_SkipField(byte** buffer, size_t* buf_len, uint32 tag) {
	uint64 value;
	switch (TAG_WIRETYPE(tag)) {
		case WIRETYPE_VARINT:
			return Typy_ReadVarint64(buffer, buf_len, &value);
		case WIRETYPE_FIXED64:
			return Typy_Read64(buffer, buf_len, &value);
		case WIRETYPE_FIXED32:
			return Typy_Read32(buffer, buf_len, (uint32*)&value);
		case WIRETYPE_LENGTH_DELIMITED:
			if (!Typy_ReadVarint64(buffer, buf_len, &value)) { return false; }
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
