//
//demon options.cpp
//

#include <cstdio>
#include <cstdlib>
#include <windows.h>

#include "options.h"
#include "protocol.h"
#include "util.h"

bool use_log = false;

int pawn_value = 100;
int knight_value = 320;
int bishop_value = 330;
int rook_value = 500;
int queen_value = 1000;

int total_value = 16 * pawn_value + 4 * knight_value
	+ 4 * bishop_value + 4 * rook_value
	+ 2 * queen_value;

int number_of_options = 0;

enum option_type_t
{
	UCI_SPIN,
	UCI_COMBO,
	UCI_CHECK,
	UCI_STRING,
	UCI_BUTTON
};

struct option_t
{
	char name[64];
	char default_value[128];
	char value[128];
	int type;
	int min;
	int max;
	char combo_vars[16][32];
};

option_t uci_options[64];

void init_options()
{
	define_option("Hash", "128", UCI_SPIN, 8, 1024);
	define_option("Log", "false", UCI_CHECK, 0, 0);
	define_option("Pawn", "100", UCI_SPIN, 0, 200);
	define_option("Knight", "320", UCI_SPIN, 0, 650);
	define_option("Bishop", "330", UCI_SPIN, 0, 650);
	define_option("Rook", "500", UCI_SPIN, 0, 1000);
	define_option("Queen", "1000", UCI_SPIN, 0, 2000);
}

void get_options()
{
	hash_size = get_option_int("Hash");

	if (get_option_bool("Log"))
		use_log = true;
	else
		use_log = false;

	pawn_value = get_option_int("Pawn");
	knight_value = get_option_int("Knight");
	bishop_value = get_option_int("Bishop");
	rook_value = get_option_int("Rook");
	queen_value = get_option_int("Queen");
}

void define_option(const char* name, const char* default_value, const int type, const int min, int max, ...)
{
	option_t* new_option = uci_options + number_of_options;

	strcpy(new_option->name, name);
	strcpy(new_option->default_value, default_value);
	strcpy(new_option->value, new_option->default_value);
	new_option->type = type;
	new_option->min = min;
	new_option->max = max;

	if (new_option->type == UCI_COMBO)
	{
		int i = 0;
		va_list arglist;
		va_start(arglist, max);

		do
		{
			if (const char* str = va_arg(arglist, char *); str != nullptr)
				strcpy(new_option->combo_vars[i++], str);
		}
		while (i < max);
		new_option->combo_vars[i][0] = '\0';
		va_end(arglist);
	}
	number_of_options++;
}

void print_option(option_t* this_option)
{
	int i;
	char option_type_name[UCI_BUTTON + 1][16] =
	{
		"spin", "combo", "check", "string", "button"
	};

	output("option name %s ", this_option->name);
	output("type %s ", option_type_name[this_option->type]);

	if (this_option->type != UCI_BUTTON)
		output("default %s ", this_option->default_value);

	switch (this_option->type)
	{
	case UCI_SPIN:
		output("min %d max %d", this_option->min, this_option->max);
		break;

	case UCI_COMBO:
		for (i = 0; strlen(this_option->combo_vars[i]) > 0; i++)
			output("var %s ", this_option->combo_vars[i]);
		break;

	default:
		break;
	}
	output("\n");
}

void print_options()
{
	for (int i = 0; i < number_of_options; i++)
		print_option(uci_options + i);
}

option_t* get_option_name(const char* option_name)
{
	for (int i = 0; i < number_of_options; i++)
		if (strcmp(option_name, uci_options[i].name) == 0)
			return uci_options + i;
	return nullptr;
}

int get_option_bool(const char* option_name)
{
	const option_t* this_option = get_option_name(option_name);
	return this_option->value[0] == 't' ? 1 : 0;
}

int get_option_int(const char* option_name)
{
	const option_t* this_option = get_option_name(option_name);
	return atoi(this_option->value);
}

char* get_option_string(const char* option_name, char* str)
{
	const option_t* this_option = get_option_name(option_name);
	strcpy(str, this_option->value);
	return str;
}

void set_option(const char* option_name, const char* value)
{
	if (option_t* this_option = get_option_name(option_name); this_option != nullptr)
	{
		strcpy(this_option->value, value);

		if (char* c = strchr(this_option->value, '\n'); c != nullptr)
			*c = '\0';
	}
}
