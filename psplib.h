#ifndef PSPLIB_H
#define PSPLIB_H

#include "pspl_input.h"
#include "pspl_graphics.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef  PSPLIB_DEBUG
static void pspl_log(const char* format, ...)
{
	FILE* fp = fopen("pspl_log.txt", "a");
	if (fp)
	{
		va_list args;
		va_start(args, format);

		vfprintf(fp, format, args);
		fprintf(fp, "\n");
		fclose(fp);

		va_end(args);
	}
}
#else
inline void pspl_log(const char* format, ...) {}
#endif


#endif 
