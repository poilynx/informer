#include "log.h"
#include <stdio.h>
#include <stdarg.h>
void Log(char *fmt, ...) {
	FILE *	f;
	va_list	vl;
	int		n;

	f = fopen("c:\\testsvc.log","a+");
	if(f == NULL)
		return;

	va_start(vl,fmt);
	n = vfprintf(f, fmt, vl);
	fclose(f);
	va_end(vl);
	return ;
}


