#include <iostream>
#include <string>
#include <vector>
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

MiniGame::Evaluation eval_strat(const MiniGame (&games)[4], const std::vector<Key>& p1, const std::vector<Key>& p2, const std::vector<Key>& p3) {
	MiniGame games_cpy[4];
	games_cpy[0] = games[0];
	games_cpy[1] = games[1];
	games_cpy[2] = games[2];
	games_cpy[3] = games[3];

	//TODO : stop loop once all say game over
	for (size_t i = 0; i < p1.size(); i++) {
		Key keys[3] = {p1[i], p2[i], p3[i]};
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
	}
	return evaluate(games_cpy);
}

void mutate_strat(vector<Key>& keys, int percent_mutation) {
	for (size_t i = 0; i < keys.size(); i++) {
		if (rand() % 100 < percent_mutation)
			keys[i] = (Key)(rand()%4);
	}
}

typedef std::vector<Key> Strats[POP_ME];
void evolve_strats(const MiniGame (&games)[4], Strats (&strats)[3] , int player, int percent_mutation, int population_size) {
	std::multimap<double, std::vector<Key>, std::greater<double>> ranked_strats;
	int opp1 = (player + 1) % 3;
	int opp2 = (player + 2) % 3;

	//mutate each strat once
	for (int i = 0; i < population_size; i++) {
		const std::vector<Key>& strat = strats[player][i];
		ranked_strats.insert({eval_of_player(eval_strat(games, strat, strats[opp1][0], strats[opp2][0]), player), strat});
		std::vector<Key> mutated = strat;
		mutate_strat(mutated, percent_mutation);
		ranked_strats.insert({eval_of_player(eval_strat(games, mutated, strats[opp1][0], strats[opp2][0]), player), mutated});
	}
	int inserted = 0;
	// keep best pop
	for (auto& strat : ranked_strats) {
		strats[player][inserted++] = std::move(strat.second);
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
	std::vector<Key> strategies[3][POP_ME]; // my strats
	for (int i = 0; i < POP_ME; i++) {
		std::vector<Key> current;
		current.reserve(100);
		for (int j = 0; j < 100; j++) {
			current.push_back((Key)(rand()%4));
		}
		strategies[player_idx][i] = current;
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
		if (first_turn)
			dump_turn1(player_idx, nb_games, glob_scores, games);
		//manual_step_test(games);
		size_t cycle = 0;
		int mutation_rate = DEFAULT_MUTATION_RATE;
		while (millis.count() < (first_turn?995:45))
		{
			//mutate stuff here
			evolve_strats(games, strategies, cycle%3, mutation_rate, cycle%3==(unsigned int)player_idx?100:10);
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
		cout << KeyStrs[strategies[player_idx][0][0]] << endl;
		std::cerr << "Took " << millis.count() << "ms" << std::endl;
		//cout << "RIGHT" << endl;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < (i==player_idx?POP_ME:POP_OPP); j++) {
				std::vector<Key>& strat = strategies[i][j];
				strat.erase(strat.begin());
			}
		}
		first_turn = false;
	}
}
