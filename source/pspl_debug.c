#include "pspl_debug.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

void pspl_log_file(const char* format, ...)
{
	static bool init = false;
	if (!init)
	{
		remove(PSPL_DEBUG_FILE);
		init = true;
	}

	FILE* fp = fopen(PSPL_DEBUG_FILE, "a");
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