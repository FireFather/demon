//
//demon protocol.cpp
//

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "bitboard.h"
#include "define.h"
#include "draw.h"
#include "hash.h"
#include "init.h"
#include "move.h"
#include "options.h"
#include "search.h"
#include "protocol.h"
#include "strings.h"
#include "timeman.h"
#include "util.h"

int moves = 0;
double winc = 0;
double binc = 0;
double wtime = 0;
double btime = 0;
int hash_size = 128;
int move_number = 0;
int moves_to_go = 0;
bool ponder = false;
clock_t start_time;
double move_time = 0;
int search_depth = 0;
int nodes_per_check = 300000;
double infinite = 1000000000000000;
double move_time_in_cps = move_time * CLOCKS_PER_SEC;

int process_input()
{
	const auto input = static_cast<char*>(malloc(16384));
	int d = 6;

	srand(time(nullptr));
	setbuf(stdout, nullptr);
	setbuf(stdin, nullptr);
	signal(SIGINT, SIG_IGN);

	while (true)
	{
		get(input, 65536);

		if (use_log)
			fprintf(log_file, "%s%c", input, '\n');

		else if (!strcmp(input, "uci"))
		{
			output("id name %s %s %s\n", NAME, VERSION, PLATFORM);
			output("id author %s\n", AUTHOR);
			print_options();
			output("uciok\n");
		}

		else if (!strcmp(input, "isready"))
			output("%s\n", "readyok");

		else if (!strcmp(input, "ucinewgame"))
			new_game();

		else if (string_starts(input, "perft"))
		{
			const char* token = strtok(input, " ");
			if (token)
			{
				token = strtok(nullptr, " ");
				if (token)
					d = atoi(token);
			}
			perft(d);
		}

		else if (string_starts(input, "divide"))
		{
			const char* token = strtok(input, " ");
			if (token)
			{
				token = strtok(nullptr, " ");
				if (token)
					d = atoi(token);
			}
			divide(d);
		}

		else if (string_starts(input, "position"))
			parse_position(input);

		else if (string_starts(input, "setoption"))
			parse_option(input);

		else if (string_starts(input, "go"))
		{
			const char* token = strtok(input, " ");

			while (token)
			{
				if (!strcmp(token, "btime"))
				{
					token = strtok(nullptr, " ");

					if (token)
						btime = atoi(token) / 1000;
				}

				else if (!strcmp(token, "wtime"))
				{
					token = strtok(nullptr, " ");

					if (token)
						wtime = atoi(token) / 1000;
				}

				else if (!strcmp(token, "binc"))
				{
					token = strtok(nullptr, " ");

					if (token)
						binc = atoi(token) / 1000;
				}

				else if (!strcmp(token, "winc"))
				{
					token = strtok(nullptr, " ");

					if (token)
						winc = atoi(token) / 1000;
				}

				else if (!strcmp(token, "depth"))
				{
					token = strtok(nullptr, " ");

					if (token)
						search_depth = atoi(token);
				}

				else if (!strcmp(token, "infinite"))
					move_time = infinite;

				else if (!strcmp(token, "movestogo"))
				{
					token = strtok(nullptr, " ");

					if (token)
						moves_to_go = atoi(token);
				}

				else if (!strcmp(token, "movetime"))
				{
					token = strtok(nullptr, " ");

					if (token)
						move_time = atoi(token) / 1000;
				}

				token = strtok(nullptr, " ");
			}

			start_search();
		}
		else if (!strcmp(input, "quit"))
		{
			free_hash();
			free(input);
			if (use_log)
				close_log();
			break;
		}
	}
	return 0;
}

void parse_option(char string[])
{
	const char* name = strstr(string, "name ");
	char* value = strstr(string, "value ");

	if (name == nullptr || value == nullptr || name >= value)
		return;

	value[-1] = '\0';
	name += 5;
	value += 6;

	set_option(name, value);

	if (!strcmp(name, "Hash"))
	{
		hash_size = get_option_int("Hash");
		clear_hash();
		init_hash();
	}

	if (!strcmp(name, "Log"))
	{
		if (get_option_bool("Log"))
		{
			use_log = true;
			open_log();
		}
		else
			use_log = false;
	}

	if (!strcmp(name, "Ponder"))
	{
		if (get_option_bool("Ponder"))
			ponder = true;
		else
			ponder = false;
	}

	if (!strcmp(name, "Pawn"))
	{
		pawn_value = get_option_int("Pawn");
	}

	if (!strcmp(name, "Knight"))
	{
		knight_value = get_option_int("Knight");
	}

	if (!strcmp(name, "Bishop"))
	{
		bishop_value = get_option_int("Bishop");
	}

	if (!strcmp(name, "Rook"))
	{
		rook_value = get_option_int("Rook");
	}

	if (!strcmp(name, "Queen"))
	{
		queen_value = get_option_int("Queen");
	}
}

void start_search()
{
	const int side = board->turn ? 1 : -1;

	if (move_time != infinite && move_time == 0)
		move_time = get_move_time();

	move_time_in_cps = move_time * CLOCKS_PER_SEC;

	nodes = 0;
	start_time = clock();

	if (search_depth == 0)
		iterate(side, MAX_ITERATION);
	else
		iterate(side, search_depth);

	if (best_move)
	{
		make_move(best_move, 0);
		move_number++;
		position_list[move_number] = board->key;

		const auto coord_move = to_long_algebraic(board, best_move);
		output("bestmove %s\n", coord_move);

		if (ponder)
		{
		}
		free(coord_move);
	}
}

void new_game()
{
	int i;
	int j;

	set_start_position();

	for (i = 0; i < MAX_DEPTH; i++)
		for (j = 0; j < MAX_DEPTH; j++)
		{
			pv[i][j] = 0;
		}

	for (i = 0; i < MAX_DEPTH; i++)
		pv_length[i] = 0;

	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++)
		{
			history_white[i][j] = 0;
			history_black[i][j] = 0;
		}

	for (i = 0; i < hash_entries; i++)
	{
		smart_hash[i].old = 1;
	}
	move_number = 0;
}
