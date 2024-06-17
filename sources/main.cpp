#include <iostream>
#include <string>
#include <algorithm>
#include "MiniGame.hpp"
#include <cstdlib>
#include <chrono>
#include <map>
#include <set>
#include "parameters.hpp"

using namespace std;

MiniGame::Evaluation evaluate(const MiniGame (&games)[4], int turn) {
	MiniGame::Evaluation scores[4];
	scores[0] = games[0].evaluate(turn);
	scores[1] = games[1].evaluate(turn);
	scores[2] = games[2].evaluate(turn);
	scores[3] = games[3].evaluate(turn);
	MiniGame::Evaluation total_score;
	total_score[0] = scores[0][0] * scores[1][0] * scores[2][0] * scores[3][0];
	total_score[1] = scores[0][1] * scores[1][1] * scores[2][1] * scores[3][1];
	total_score[2] = scores[0][2] * scores[1][2] * scores[2][2] * scores[3][2];
	return total_score;
}

double eval_of_player(const MiniGame::Evaluation& total_score, int player) {
	return std::min(total_score[player] - total_score[(player + 1) % 3], total_score[player] - total_score[(player + 2)%3]);
	//TODO being 2nd is better than beng 3rd no matter the point difference
}

MiniGame::Evaluation eval_strat(const MiniGame (&games)[4], Strat p1, Strat p2, Strat p3, int turn) {
	MiniGame games_cpy[4];
	games_cpy[0] = games[0];
	games_cpy[1] = games[1];
	games_cpy[2] = games[2];
	games_cpy[3] = games[3];

	for (size_t i = 0; i < MOVE_PER_STRAT && turn < MAX_TURN; i++) {
		Key keys[3] = {(Key)(p1 & 3), (Key)(p2 & 3), (Key)(p3 & 3)};
		games_cpy[0].simulateTurn(keys);
		games_cpy[1].simulateTurn(keys);
		games_cpy[2].simulateTurn(keys);
		games_cpy[3].simulateTurn(keys);
		p1 >>= 2;
		p2 >>= 2;
		p3 >>= 2;
		turn++;
	}
	return evaluate(games_cpy, turn);
}

Strat mutate_strat(Strat keys, uint8_t bits_to_flip) {
	for (uint8_t i = 0; i < bits_to_flip; i++) {
		keys ^= (1 << (rand() % MOVE_PER_STRAT));
	}
	return keys;
}

//crossover method
std::pair<Strat,Strat> breed_strats(Strat parent1, Strat parent2, uint8_t bits_to_flip) {
	Strat bitmask;
	unsigned int crossover = rand() % MOVE_PER_STRAT;
	bitmask = (1 << (crossover * 2)) - 1;
	return {
		mutate_strat((parent1 & bitmask) | (parent2 & ~bitmask), bits_to_flip / OFFSPRING_MUTATION_DIV),
		mutate_strat((parent1 & ~bitmask) | (parent2 & bitmask), bits_to_flip / OFFSPRING_MUTATION_DIV),
	};
}

Strat optimal_diving(const std::string& gpu) {
	Strat out = 0;
	for (uint8_t i = 0; i < gpu.size(); i++) {
		out |= charToKey[(int)gpu[i]] << (i * 2);
	}
	return out;
}

/*
 * dist to # (from pos + 1)
 * npos: RIGHT
 * 0:    UP
 * 1:    LEFT
 * 2:    DOWN
 * 3:    RIGHT
 * 3+:   RIGHT
 */

Strat optimal_runner(const std::string& gpu, size_t pos, int stun) {
	Strat out = 0;
	for (uint8_t i = 0; pos + 1 < gpu.size() && i < MOVE_PER_STRAT; i++) {
		Key key = UP;
		if (stun) {
			stun--;
			continue;
		}
		uint8_t dist_to_hedge = gpu.find('#', pos + 1) - (pos + 1);
		switch (dist_to_hedge) {
			case 0:
				key = UP;
				pos += 2;
				break;
			case 1:
				key = LEFT;
				pos += 1;
				break;
			case 2:
				pos += 2;
				key = DOWN;
				break;
			default: // 3 , 3+, npos
				pos += 3;
				key = RIGHT;
		}
		out |= (int)key << (i * 2);
	}
	return out;
}

typedef Strat Strats[POP_ME];
void evolve_strats(const MiniGame (&games)[4], Strats (&strats)[3] , int player, uint8_t bits_to_flip, int population_size, int turn) {
	std::multimap<double, Strat, std::greater<double>> ranked_strats;
	std::set<Strat> population;

	//mutate each strat once (pretty harshly)
	for (int i = 0; i < population_size; i++) {
		population.insert(strats[player][i]);
		population.insert(mutate_strat(strats[player][i], bits_to_flip));
	}
	//breed strats
	for (int i = 0; i < population_size; i+=2) {
		size_t parent1_idx = rand() % population_size;
		size_t parent2_idx = rand() % population_size;
		while(parent2_idx == parent1_idx)
			parent2_idx = rand() % population_size;
		std::pair<Strat,Strat> babies = breed_strats(strats[player][parent1_idx], strats[player][parent2_idx], bits_to_flip);
		population.insert(babies.first);
		population.insert(babies.second);
	}
	//evaluate all deduplicated strats
	Strat to_test[3];
	to_test[0] = strats[0][0];
	to_test[1] = strats[1][0];
	to_test[2] = strats[2][0];
	for (Strat strat : population) {
		to_test[player] = strat;
		ranked_strats.insert({eval_of_player(eval_strat(games, to_test[0], to_test[1], to_test[2], turn), player), to_test[player]});
	}

	// keep best pop
	int inserted = 0;
	for (const auto& strat : ranked_strats) {
		strats[player][inserted++] = strat.second;
		if (inserted == population_size)
			break;
	}
}

void dump_turn1(int player_idx, int nb_games, int glob_scores[3], MiniGame (&games)[4]) {
	std::cerr << player_idx << std::endl;
	std::cerr << nb_games << std::endl;
	for (int i = 0; i < 3; i++) {
		std::cerr << glob_scores[i]
			<<" "<< games[0].medals[i].gold <<" "<< games[0].medals[i].silver <<" "<< games[0].medals[i].bronze
			<<" "<< games[1].medals[i].gold <<" "<< games[1].medals[i].silver <<" "<< games[1].medals[i].bronze
			<<" "<< games[2].medals[i].gold <<" "<< games[2].medals[i].silver <<" "<< games[2].medals[i].bronze
			<<" "<< games[3].medals[i].gold <<" "<< games[3].medals[i].silver <<" "<< games[3].medals[i].bronze
			<< std::endl;
	}
	for (int i = 0; i < 4; i++) {
		std::cerr << games[i].gpu << " ";
		for (int j = 0; j < 7; j++)
			std::cerr << games[i].regs[j] << " ";
		std::cerr << std::endl;
	}
}

void manual_step_test(MiniGame (&games)[4]) {
	std::cerr << "stepping" << std::endl;
	std::map<std::string, Key> keys = {
		{"UP", UP},
		{"DOWN", DOWN},
		{"LEFT", LEFT},
		{"RIGHT", RIGHT},
	};

	for(;;) {
		std::string moves[3];
		std::cin.ignore();
		std::getline(cin, moves[0]);
		std::cerr << "move 1" << moves[0] << std::endl;
		std::getline(cin, moves[1]);
		std::cerr << "move 2" << moves[1] << std::endl;
		std::getline(cin, moves[2]);
		std::cerr << "move 3" << moves[2] << std::endl;
		Key key[3];
		key[0] = keys[moves[0]];
		key[1] = keys[moves[1]];
		key[2] = keys[moves[2]];
		for (int i = 0; i < 4; i++) {
			games[i].simulateTurn(key);
			games[i].display_status();
			std::cerr << "===" << std::endl;
		}
	}
}

int main()
{
	int player_idx, opp1_index, opp2_index;
	cin >> player_idx; cin.ignore();
	opp1_index = (player_idx + 1) % 3;
	opp2_index = (player_idx + 2) % 3;
	int nb_games;
	cin >> nb_games; cin.ignore();

	MiniGame games[4];
	games[0].type = Runner;
	games[1].type = Archery;
	games[2].type = Skater;
	games[3].type = Diving;
	// game loop
	Strat strategies[3][POP_ME]; // my strats
	for (int i = 0; i < POP_ME; i++) {
		strategies[player_idx][i] = rand() ^ (rand() << 1);
	}
	for (int i = 0; i < POP_OPP; i++) {
		strategies[opp1_index][i] = strategies[player_idx][i];
		strategies[opp2_index][i] = strategies[player_idx][i];
	}
	int glob_scores[3];
	int turn = 0;
	while (1) {
		for (int i = 0; i < 3; i++) {
			cin >> glob_scores[i]
				>> games[0].medals[i].gold >> games[0].medals[i].silver >> games[0].medals[i].bronze
				>> games[1].medals[i].gold >> games[1].medals[i].silver >> games[1].medals[i].bronze
				>> games[2].medals[i].gold >> games[2].medals[i].silver >> games[2].medals[i].bronze
				>> games[3].medals[i].gold >> games[3].medals[i].silver >> games[3].medals[i].bronze
			;
			std::cin.ignore();
		}
		auto time_start = std::chrono::system_clock::now(); // get the current time
		auto time_now = time_start;
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_start);
		for (int i = 0; i < 4; i++)
			games[i].update_state();
		if (games[3].gpu != "GAME_OVER") {
			Strat optimal_dive = optimal_diving(games[3].gpu);
			strategies[player_idx][POP_ME - 1] = optimal_dive;
			strategies[opp1_index][POP_OPP - 1] = optimal_dive;
			strategies[opp2_index][POP_OPP - 1] = optimal_dive;
		}
		if (games[0].gpu != "GAME_OVER") {
			strategies[player_idx][POP_ME - 2] = optimal_runner(games[0].gpu, games[0].regs[player_idx], games[0].regs[player_idx + 3]);
			strategies[opp1_index][POP_OPP- 2] = optimal_runner(games[0].gpu, games[0].regs[opp1_index], games[0].regs[opp1_index + 3]);
			strategies[opp2_index][POP_OPP- 2] = optimal_runner(games[0].gpu, games[0].regs[opp2_index], games[0].regs[opp2_index + 3]);
		}
		if (turn == 0)
			dump_turn1(player_idx, nb_games, glob_scores, games);
		//manual_step_test(games);
		size_t cycle = 0;
		int mutation_rate = DEFAULT_MUTATION_RATE;
		while (millis.count() < (turn?45:995))
		{
			//mutate stuff here
			evolve_strats(games, strategies, cycle%3, mutation_rate, cycle%3==(unsigned int)player_idx?POP_ME:POP_OPP, turn);
			time_now = std::chrono::system_clock::now();
			millis = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_start);
			cycle++;
			if (mutation_rate > MIN_MUTATION_RATE && cycle % GEN_TO_LOWER_MUTATION_RATE == 0)
				mutation_rate--;
		}
		/*
		for (int i = 0; i < 4; i++)
		{
			games[i].display_status();
			std::cerr << "===" << std::endl;
		}
		*/
		std::cerr << "Did " << cycle << " cycles (" << cycle / 3 <<" generations)" << std::endl;
		cout << KeyStrs[strategies[player_idx][0] & 3] << endl;
		std::cerr << "Took " << millis.count() << "ms" << std::endl;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < (i==player_idx?POP_ME:POP_OPP); j++) {
				strategies[i][j] >>= 2;
			}
		}
		turn++;
	}
}
