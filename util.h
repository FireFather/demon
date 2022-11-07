//
//demon util.h
//

#pragma once

#include <cstdint>
#include <cstdio>

//function headers
uint64_t get_random_64();
void show_version();
void open_log();
void close_log();
void output(const char format[], ...);
int InputAvailable();
//extern
extern FILE* log_file;
