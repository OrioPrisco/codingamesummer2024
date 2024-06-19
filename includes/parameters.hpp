#pragma once

// POP_ME >= POP_OPP !!!
#define POP_ME 100
#define POP_OPP 20
// divisor applied to offspring's mutation rate
#define OFFSPRING_MUTATION_DIV 2
// bits to randomly flip on a mutated copy
#define DEFAULT_MUTATION_RATE 10
#define MIN_MUTATION_RATE 3
// Numbers of genration to lowe the mutation rate by 1
#define GEN_TO_LOWER_MUTATION_RATE 10

#include <cstdint>
struct MiniGamesResults {
	uint8_t runner_positions[16]; // position is less than 30, could pack better
	uint8_t runner_stun = 0; // only requires 2 bits
	uint8_t runner_turn = 0; // 0 means not done, anything else is amounf ot turns
	//turn cannot be over 16, requiring 4/5 bits, can pack stuff here

	int16_t archery_distance2 = 0; // distance squared from center in archery
	// -1 means cannot finish
	//probably not worth rooting as it would require floating point number
	//or store it as a fixed point number ?
	//would still require 16 bits anyway

	bool skater_done = 0;
	int8_t skater_pos = 0; // -1 means game cannot be finished
	// max Skater pos is less than 45, requiring 6 or less bit
	// could use the 2 bits for stuff
	int8_t skater_risk = 0;// only requires 4 bits, could pack things

	// -1 means cannot finish
	int8_t diving_score = 0;
};

typedef uint32_t Strat;
#define MOVE_PER_STRAT 16

//turn at which the game ends
#define MAX_TURN 100
