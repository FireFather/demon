//
//demon util.cpp
//
#include "util.h"

#include <conio.h>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>
#include <windows.h>

#include "define.h"
#include "options.h"

FILE* log_file;
int j = 23;
int k = 54;

void show_version()
{
	const std::string startup_banner =
		"" NAME " " VERSION " " PLATFORM "\n"
		"Norman Schmidt\n"
		"" __DATE__ " " __TIME__ "\n";

	std::cout << startup_banner;
}

void open_log()
{
	char file_name[MAX_STRING];
	char buf[MAX_STRING];
	time_t now;
	tm tnow;
	time(&now);
	tnow = *localtime(&now);
	strftime(buf, 32, "%d%b-%H%M", &tnow);
	sprintf(file_name, "%s %s %s %s.txt", NAME, VERSION, PLATFORM, buf);
	log_file = fopen(file_name, "wt");
}

void close_log()
{
	fclose(log_file);
}

void output(const char format[], ...)
{
	va_list arg_list;
	char string[4096];

	va_start(arg_list, format);
	vsprintf(string, format, arg_list);
	va_end(arg_list);

	printf("%s", string);

	if (use_log)
		fprintf(log_file, string, "\n");
}

int InputAvailable()
{
	//	if (stdin->cnt > 0) return 1;

	static HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;

	if (static BOOL console = GetConsoleMode(hInput, &mode); !console)
	{
		DWORD total_bytes_avail;
		if (!PeekNamedPipe(hInput, nullptr, 0, nullptr, &total_bytes_avail, nullptr)) return true;
		return static_cast<int>(total_bytes_avail);
	}
	return _kbhit();
}
