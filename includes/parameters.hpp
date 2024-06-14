#pragma once

// POP_ME >= POP_OPP !!!
#define POP_ME 50
#define POP_OPP 10
// 100 is 100%
#define PERCENT_OFFSPRING_MUTATION_RATE 3
// 100 is 100%
#define DEFAULT_MUTATION_RATE 30
// 100 is 100%
#define MIN_MUTATION_RATE 3
// Numbers of genration to lowe the mutation rate by 1%
#define GEN_TO_LOWER_MUTATION_RATE 3

#include <cstdint>
typedef uint64_t Strat;
#define MOVE_PER_STRAT 32

//turn at which the game ends
#define MAX_TURN 100
