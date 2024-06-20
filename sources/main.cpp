#include <iostream>
#include <string>
#include <algorithm>
#include "MiniGame.hpp"
#include <cstdlib>
#include <chrono>
#include <map>
#include <unordered_set>
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

Genome genome_from_strat(Strat strat, int player, const MiniGame (&games)[4], int turn) {
	Genome out;
	out.first = strat;
	games[0].runnerDoTurns(out.second, strat, player, turn);
	games[1].archeryDoTurns(out.second, strat, player, turn);
	games[2].skaterDoTurns(out.second, strat, player, turn);
	games[3].divingDoTurns(out.second, strat, player, turn);
	return (out);
}

void medals(MiniGame::Evaluation& out, const int (&scores)[3]) {
	int sec_max = 0;
	int max = INT_MIN;
	for (int i = 0; i < 3; i++) {
		if (scores[i] >= max) {
			sec_max = max;
			max = scores[i];
		} else if (scores[i] > sec_max) {
			sec_max = scores[i];
		}
	}
	for (int i = 0; i < 3; i++)
	{
		if (scores[i] == max) {
			out[i] += 3;
		} else if (scores[i] == sec_max) {
			out[i] += 1;
		} else {
			out[i] += 0;
		}
	}
}

MiniGame::Evaluation skaterResultsEvaluate(const MiniGamesResults& r1, const MiniGamesResults& r2, const MiniGamesResults &r3) {
	bool shares_space[3];
	int risks[3];

	if (r1.skater_pos % 10 == r2.skater_pos %10) {
		shares_space[0] = true;
		shares_space[1] = true;
	}
	if (r2.skater_pos % 10 == r3.skater_pos % 10) {
		shares_space[1] = true;
		shares_space[2] = true;
	}
	if (r3.skater_pos % 10 == r1.skater_pos % 10) {
		shares_space[2] = true;
		shares_space[0] = true;
	}
	MiniGame::skaterDoPlayer2(risks[0], shares_space[0]);
	MiniGame::skaterDoPlayer2(risks[1], shares_space[1]);
	MiniGame::skaterDoPlayer2(risks[2], shares_space[2]);
	return {
		r1.skater_pos + (r1.skater_risk>=0?-r1.skater_risk*0.3:r1.skater_risk*2),
		r2.skater_pos + (r2.skater_risk>=0?-r2.skater_risk*0.3:r2.skater_risk*2),
		r3.skater_pos + (r3.skater_risk>=0?-r3.skater_risk*0.3:r3.skater_risk*2),
	};
}

MiniGame::Evaluation runnerResultsEvaluate(const MiniGamesResults& r1, const MiniGamesResults& r2, const MiniGamesResults &r3) {
	return {
		(double)r1.runner_positions[15] - r1.runner_stun * 2,
		(double)r2.runner_positions[15] - r2.runner_stun * 2,
		(double)r3.runner_positions[15] - r3.runner_stun * 2,
	};
}

void runnerFullResultsEvaluate(MiniGame::Evaluation& eval, const MiniGamesResults& r1, const MiniGamesResults& r2, const MiniGamesResults &r3) {
	int end_turn = r1.runner_turn;
	if (end_turn == 0 || end_turn > r2.runner_turn)
		end_turn = r2.runner_turn;
	if (end_turn == 0 || end_turn > r3.runner_turn)
		end_turn = r3.runner_turn;
	medals(eval, {r1.runner_positions[end_turn], r2.runner_positions[end_turn], r3.runner_positions[end_turn]});
}

void ResultsEvaluate(MiniGame::Evaluation& eval, const MiniGamesResults& r1, const MiniGamesResults& r2, const MiniGamesResults& r3, Type type) {
	if (type == Diving) {
		if (r1.diving_score == -1)
			return;
		return medals(eval, {r1.diving_score, r2.diving_score, r3.diving_score});
	} else if (type == Archery) {
		if (r1.archery_distance2 == -1)
			return;
		return medals(eval, {r1.archery_distance2, r2.archery_distance2, r3.archery_distance2});
	}
	MiniGame::Evaluation partial_scores;
	if (type == Skater) {
		if (r1.skater_pos == -1)
			return;
		if (r1.skater_done)
			return medals(eval, {(int)r1.skater_pos, (int)r2.skater_pos, (int)r3.skater_pos});
		partial_scores = skaterResultsEvaluate(r1, r2, r3);
	}
	else if (type == Runner) {
		if (r1.runner_turn > 0 || r2.runner_turn > 0 || r3.runner_turn > 0)
			return runnerFullResultsEvaluate(eval, r1, r2, r3);
		partial_scores = runnerResultsEvaluate(r1, r2, r3);
	} else {
		throw std::runtime_error("unknwon type");
	}
	int positions[3];
	positions[0] = (partial_scores[0] >= partial_scores[1]) + (partial_scores[0] >= partial_scores[2]);
	positions[1] = (partial_scores[1] >= partial_scores[0]) + (partial_scores[1] >= partial_scores[2]);
	positions[2] = (partial_scores[2] >= partial_scores[0]) + (partial_scores[2] >= partial_scores[1]);
	eval[0] += ((double) positions[0]) /2;
	eval[1] += ((double) positions[1]) /2;
	eval[2] += ((double) positions[2]) /2;
}

MiniGame::Evaluation eval_strat(const MiniGame (&games)[4], const MiniGamesResults& r1, const MiniGamesResults& r2, const MiniGamesResults& r3) {
	MiniGame::Evaluation scores[4];
	scores[0] = games[0].evaluate(true); //get Medal scores
	scores[1] = games[1].evaluate(true);
	scores[2] = games[2].evaluate(true);
	scores[3] = games[3].evaluate(true);

	ResultsEvaluate(scores[0], r1, r2, r3, Runner);
	ResultsEvaluate(scores[1], r1, r2, r3, Archery);
	ResultsEvaluate(scores[2], r1, r2, r3, Skater);
	ResultsEvaluate(scores[3], r1, r2, r3, Diving);

	MiniGame::Evaluation total_score;
	total_score[0] = scores[0][0] * scores[1][0] * scores[2][0] * scores[3][0];
	total_score[1] = scores[0][1] * scores[1][1] * scores[2][1] * scores[3][1];
	total_score[2] = scores[0][2] * scores[1][2] * scores[2][2] * scores[3][2];
	return total_score;
}

MiniGame::Evaluation eval_strat(const MiniGame (&games)[4], Strat p1, Strat p2, Strat p3, int turn) {
	MiniGame games_cpy[4];
	games_cpy[0] = games[0];
	games_cpy[1] = games[1];
	games_cpy[2] = games[2];
	games_cpy[3] = games[3];
	Strat strats[3] = {p1, p2, p3};

	games_cpy[0].runnerDoTurns(strats, turn);
	games_cpy[1].archeryDoTurns(strats, turn);
	games_cpy[2].skaterDoTurns(strats, turn);
	games_cpy[3].divingDoTurns(strats, turn);
	return evaluate(games_cpy);
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

size_t get_tournament_idx(size_t population_size) {
	size_t parent_idx = rand() % population_size;
	uint8_t i = 0;
	while (i < 3) {
		size_t competitor_idx = rand() % population_size;
		if (competitor_idx == parent_idx)
			continue;
		if (competitor_idx < parent_idx)
			parent_idx = competitor_idx;
		i++;
	}
	return parent_idx;
}

typedef Genome Genomes[POP_ME];
void evolve_strats(const MiniGame (&games)[4], Genomes (&genes)[3] , int player, uint8_t bits_to_flip, int population_size, int turn) {
	std::multimap<double, Genome, std::greater<double>> ranked_strats;
	std::unordered_set<Genome> population;

	//mutate each strat once (pretty harshly)
	for (int i = 0; i < population_size; i++) {
		population.insert(genes[player][i]);
		Genome mutant = genome_from_strat(mutate_strat(genes[player][i].first, bits_to_flip), player, games, turn);
		population.insert(mutant); // std::move ?
	}
	//breed strats
	for (int i = 0; i < population_size; i+=2) {
		size_t parent1_idx = get_tournament_idx(population_size);
		size_t parent2_idx = rand() % population_size;
		while(parent2_idx == parent1_idx)
			parent2_idx = rand() % population_size;
		std::pair<Strat,Strat> babies = breed_strats(
			genes[player][parent1_idx].first,
			genes[player][parent2_idx].first, bits_to_flip
		);
		Genome baby1 = genome_from_strat(babies.first, player, games, turn);
		Genome baby2 = genome_from_strat(babies.second, player, games, turn);
		population.insert(baby1);
		population.insert(baby2);
	}
	//evaluate all deduplicated strats
	Genome to_test[3];
	to_test[0] = genes[0][0];
	to_test[1] = genes[1][0];
	to_test[2] = genes[2][0];
	for (Genome gene : population) {
		to_test[player] = gene;
		ranked_strats.insert({eval_of_player(eval_strat(games, to_test[0].second, to_test[1].second, to_test[2].second), player), to_test[player]});
	}

	// keep best pop
	int inserted = 0;
	for (const auto& strat : ranked_strats) {
		genes[player][inserted++] = strat.second;
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

	int turn = 1;
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
			std::cerr << "Turn " << turn << std::endl;
			games[i].simulateTurn(key);
			games[i].display_status();
			games[i].display_medals();
			std::cerr << "===" << std::endl;
		}
		turn++;
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
	Genome strategies[3][POP_ME]; // my strats
	for (int i = 0; i < POP_ME - 2; i++) { // last 2 get erased by runner and diver
		strategies[player_idx][i].first = (rand() ^ (rand() << 1));
	}
	for (int i = 0; i < POP_OPP; i++) {
		//on turn 0 everyone is equal, fine to copy MiniGamesResults
		strategies[opp1_index][i].first = strategies[player_idx][i].first;
		strategies[opp2_index][i].first = strategies[player_idx][i].first;
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
		//recalculate intermediary results that changed because of the update
		for (int i = 0; i < POP_ME; i++) {
			strategies[player_idx][i] = genome_from_strat(strategies[player_idx][i].first , player_idx, games, 0);
		}
		for (int i = 0; i < POP_OPP; i++) {
			//on turn 0 everyone is equal, fine to copy MiniGamesResults
			strategies[opp1_index][i] = genome_from_strat(strategies[opp1_index][i].first, opp1_index, games, 0);
			strategies[opp2_index][i] = genome_from_strat(strategies[opp2_index][i].first, opp2_index, games, 0);
		}
		if (games[3].gpu != "GAME_OVER") {
			Strat optimal_dive = optimal_diving(games[3].gpu);
			strategies[player_idx][POP_ME - 1] = genome_from_strat(optimal_dive, player_idx, games, turn);
			strategies[opp1_index][POP_OPP- 1] = genome_from_strat(optimal_dive, opp1_index, games, turn);
			strategies[opp2_index][POP_OPP- 1] = genome_from_strat(optimal_dive, opp2_index, games, turn);
		}
		if (games[0].gpu != "GAME_OVER") {
			strategies[player_idx][POP_ME - 2] = genome_from_strat(optimal_runner(games[0].gpu, games[0].regs[player_idx], games[0].regs[player_idx + 3]),
				player_idx, games, turn);
			strategies[opp1_index][POP_OPP- 2] = genome_from_strat(optimal_runner(games[0].gpu, games[0].regs[opp1_index], games[0].regs[opp1_index + 3]),
				opp1_index, games, turn);
			strategies[opp2_index][POP_OPP- 2] = genome_from_strat(optimal_runner(games[0].gpu, games[0].regs[opp2_index], games[0].regs[opp2_index + 3]),
				opp2_index, games, turn);
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
		cout << KeyStrs[strategies[player_idx][0].first & 3] << endl;
		std::cerr << "Took " << millis.count() << "ms" << std::endl;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < (i==player_idx?POP_ME:POP_OPP); j++) {
				strategies[i][j].first >>= 2;
			}
		}
		turn++;
	}
}
