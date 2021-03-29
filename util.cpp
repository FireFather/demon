//
//demon util.cpp
//

#include <cstdarg>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "define.h"
#include "bitboard.h"
#include "options.h"
#include "util.h"

FILE *log_file;
int j = 23;
int k = 54;

void show_version( void )
    {
    char *startup_banner = "\n"
        "" NAME " " VERSION "\n\n"
        "Kranium \n"
        "" __DATE__ " " __TIME__ "\n\n";

    output(startup_banner);
    }

void open_log( void )
    {
    char file_name[MAX_STRING];
    char buf[MAX_STRING];
    time_t now;
    struct tm tnow;
    time(&now);
    tnow = *localtime(&now);
    strftime(buf, 32, "%d%b-%H%M", &tnow);
    sprintf(file_name, "%s %s %s.txt", NAME, VERSION, buf);
    log_file = fopen(file_name, "wt");
    }

void close_log( void )
    {
    fclose(log_file);
    }

void output( const char format [], ... )
    {
    va_list arg_list;
    char string[4096];

    va_start(arg_list, format);
    vsprintf(string, format, arg_list);
    va_end(arg_list);

    printf("%s", string);

    if( use_log )
        fprintf(log_file, string);
    }

unsigned int Y[55] =
    {
    1217451684UL, 2816712964UL, 2334535705UL, 3950095883UL, 2265551681UL, 1770752961UL, 2185226958UL, 2388687450UL,
        1916922145UL, 2392106166UL, 2943953050UL, 3742610289UL, 3609265228UL, 3522957869UL, 3171739406UL, 2752878121UL,
        2921639267UL, 2875042281UL, 2211563587UL, 2884710826UL, 1010629021UL, 1431560701UL, 3627189000UL, 2880283151UL,
        3587109110UL, 2816164222UL, 2603598036UL, 2302718091UL, 1323074514UL, 2234352819UL, 3833870150UL, 3384436478UL,
        2882322585UL, 1786923814UL, 1241886172UL, 2079638548UL, 4263711305UL, 3003510415UL, 2224366730UL, 2082791478UL,
        3415655460UL, 2876658493UL, 3821049720UL, 3230944277UL, 4139787478UL, 2375474592UL, 1158011394UL, 2673463024UL,
        1724129812UL, 1478156632UL, 2645137599UL, 1206899692UL, 4256603317UL, 1307629121UL, 3140483415UL
    };

bitboard get_random_64()
    {
    bitboard lower32;
    bitboard upper32;

    Y[k] = (unsigned int)(Y[k] + Y[j]);
    lower32 = Y[k];

    j--;
    k--;

    if( j < 0 )
        j = 54;

    if( k < 0 )
        k = 54;

    Y[k] = (unsigned int)(Y[k] + Y[j]);
    upper32 = Y[k];

    j--;
    k--;

    if( j < 0 )
        j = 54;

    if( k < 0 )
        k = 54;

    return lower32 | (upper32 << 32);
    }
