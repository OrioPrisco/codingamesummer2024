#pragma once

// POP_ME >= POP_OPP !!!
#define POP_ME 100
#define POP_OPP 20
// 100 is 100%
// divisor applied to offspring's mutation rate
#define OFFSPRING_MUTATION_DIV 2
// bits to randomly flip on a mutated copy
#define DEFAULT_MUTATION_RATE 10
#define MIN_MUTATION_RATE 3
// Numbers of genration to lowe the mutation rate by 1
#define GEN_TO_LOWER_MUTATION_RATE 10

#include <cstdint>
typedef uint32_t Strat;
#define MOVE_PER_STRAT 16

//turn at which the game ends
#define MAX_TURN 100
