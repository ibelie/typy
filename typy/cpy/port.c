// Copyright 2017 ibelie, Chen Jie, Joungtao. All rights reserved.
// Use of this source code is governed by The MIT License
// that can be found in the LICENSE file.

#define _CRT_SECURE_NO_WARNINGS

#include "port.h"
#include "sys/stat.h"

#ifdef _MSC_VER
#	define fileno _fileno
#	define S_ISREG(m) ((m) & S_IFREG)
#	include "windows.h"
#	include "io.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

FILE* IblFileOpen(const string file) {
	struct stat file_stat = {0};
	stat(file, &file_stat);
	return fopen(file, S_ISREG(file_stat.st_mode) ? "r+b" : "w+b");
}

bool IblFileWriteUint64At(FILE* file, uint64 x, size_t offset) {
	byte buffer[8] = {
		(byte)(x >> 56),
		(byte)(x >> 48),
		(byte)(x >> 40),
		(byte)(x >> 32),
		(byte)(x >> 24),
		(byte)(x >> 16),
		(byte)(x >> 8),
		(byte)(x),
	};
	if (fseek(file, offset, SEEK_SET)) { return false; }
	return fwrite(buffer, sizeof(byte), 8, file) == 8;
}

bool IblFileTruncate(FILE* file, size_t length) {
	if (!file) { return false; }
	if (fflush(file)) { goto file_truncate_error; }

#ifdef _MSC_VER
	/* MS _chsize doesn't work if newsize doesn't fit in 32 bits,
	   so don't even try using it. */
	{
		/* Have to move current pos to desired endpoint on Windows. */
		if (fseek(file, length, SEEK_SET))  { goto file_truncate_error; }
		/* Truncate.  Note that this may grow the file! */
		register HANDLE hFile = (HANDLE)_get_osfhandle(fileno(file));
		if ((int)hFile != -1) {
			if (!SetEndOfFile(hFile)) { goto file_truncate_error; }
		} else {
			goto file_truncate_error;
		}
	}
#else
	if (ftruncate(fileno(file), length)) { goto file_truncate_error; }
#endif /* !_MSC_VER */

	return true;

file_truncate_error:
	clearerr(file);
	return false;
}

#ifdef __cplusplus
}
#endif
