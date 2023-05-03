//
//demon util.h
//

#pragma once

#include <cstdio>

//function headers
void show_version();
void open_log();
void close_log();
void output(const char format[], ...);
int InputAvailable();
//extern
extern FILE* log_file;
