#include <iostream>
#include <string>
#include <algorithm>
#include "MiniGame.hpp"
#include <cstdlib>
#include <chrono>
#include <map>
#include "parameters.hpp"

using namespace std;

MiniGame::Evaluation evaluate(const MiniGame (&games)[4]) {
	MiniGame::Evaluation scores[4];
	scores[0] = games[0].evaluate();
	scores[1] = games[1].evaluate();
	scores[2] = games[2].evaluate();
	scores[3] = games[3].evaluate();
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

MiniGame::Evaluation eval_strat(const MiniGame (&games)[4], Strat p1, Strat p2, Strat p3) {
	MiniGame games_cpy[4];
	games_cpy[0] = games[0];
	games_cpy[1] = games[1];
	games_cpy[2] = games[2];
	games_cpy[3] = games[3];

	//TODO : stop loop once all say game over
	for (size_t i = 0; i < MOVE_PER_STRAT; i++) {
		Key keys[3] = {(Key)(p1 & 3), (Key)(p2 & 3), (Key)(p3 & 3)};
		games_cpy[0].simulateTurn(keys);
		games_cpy[1].simulateTurn(keys);
		games_cpy[2].simulateTurn(keys);
		games_cpy[3].simulateTurn(keys);
		if (
			( (games_cpy[0].gpu == "GAME_OVER")
			+ (games_cpy[1].gpu == "GAME_OVER")
			+ (games_cpy[1].gpu == "GAME_OVER")
			+ (games_cpy[3].gpu == "GAME_OVER")
			) >= 3)
			break;
		p1 >>= 2;
		p2 >>= 2;
		p3 >>= 2;
	}
	return evaluate(games_cpy);
}

Strat mutate_strat(Strat keys, int percent_mutation) {
	for (size_t i = 0; i < MOVE_PER_STRAT; i++) {
		if (rand() % 100 < percent_mutation)
			keys ^= (rand()%3 | 1) >> (i * 2);
	}
	return keys;
}

Strat optimal_diving(const std::string& gpu) {
	Strat out = 0;
	for (size_t i = 0; i < gpu.size(); i++) {
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
	for (size_t i = 0; pos + 1 < gpu.size() && i < MOVE_PER_STRAT; i++) {
		Key key = UP;
		if (stun) {
			stun--;
			continue;
		}
		size_t dist_to_hedge = gpu.find('#', pos + 1) - (pos + 1);
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
void evolve_strats(const MiniGame (&games)[4], Strats (&strats)[3] , int player, int percent_mutation, int population_size) {
	std::multimap<double, uint64_t, std::greater<double>> ranked_strats;
	int opp1 = (player + 1) % 3;
	int opp2 = (player + 2) % 3;

	//mutate each strat once
	for (int i = 0; i < population_size; i++) {
		Strat strat = strats[player][i];
		ranked_strats.insert({eval_of_player(eval_strat(games, strat, strats[opp1][0], strats[opp2][0]), player), strat});
		Strat mutated = mutate_strat(strat, percent_mutation);
		ranked_strats.insert({eval_of_player(eval_strat(games, mutated, strats[opp1][0], strats[opp2][0]), player), mutated});
	}
	int inserted = 0;
	// keep best pop
	for (auto& strat : ranked_strats) {
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
		strategies[player_idx][i] = rand() | (rand() << 16);
	}
	for (int i = 0; i < POP_OPP; i++) {
		strategies[opp1_index][i] = strategies[player_idx][i];
		strategies[opp2_index][i] = strategies[player_idx][i];
	}
	int glob_scores[3];
	bool first_turn = true;
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
		if (first_turn)
			dump_turn1(player_idx, nb_games, glob_scores, games);
		//manual_step_test(games);
		size_t cycle = 0;
		int mutation_rate = DEFAULT_MUTATION_RATE;
		while (millis.count() < (first_turn?995:45))
		{
			//mutate stuff here
			evolve_strats(games, strategies, cycle%3, mutation_rate, cycle%3==(unsigned int)player_idx?POP_ME:POP_OPP);
			time_now = std::chrono::system_clock::now();
			millis = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_start);
			cycle++;
			if (mutation_rate > MIN_MUTATION_RATE && cycle % GEN_TO_LOWER_MUTATION_RATE == 0)
				mutation_rate--;
		}
		for (int i = 0; i < 4; i++)
		{
			games[i].display_status();
			std::cerr << "===" << std::endl;
		}
		std::cerr << "Did " << cycle << " cycles" << std::endl;
		//std::cerr << "score: " << eval_of_player(evaluate(games), player_idx) << std::endl;
		cout << KeyStrs[strategies[player_idx][0] & 3] << endl;
		std::cerr << "Took " << millis.count() << "ms" << std::endl;
		//cout << "RIGHT" << endl;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < (i==player_idx?POP_ME:POP_OPP); j++) {
				strategies[i][j] >>= 2;
			}
		}
		first_turn = false;
	}
}
