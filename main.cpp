//
//demon main.cpp
//

#include "define.h"
#include "bitboard.h"
#include "hash.h"
#include "init.h"
#include "options.h"
#include "protocol.h"
#include "util.h"

int main()
    {
    show_version();
    init();
    get_options();
    process_input();
    return 0;
    }
