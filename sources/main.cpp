#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "MiniGame.hpp"
#include <cstdlib>
#include <chrono>

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
	}
	return evaluate(games_cpy);
}

void mutate_strat(vector<Key>& keys, int percent_mutation) {
	for (size_t i = 0; i < keys.size(); i++) {
		if (rand() % 100 < percent_mutation)
			keys[i] = (Key)(rand()%4);
	}
}

int main()
{
	int player_idx;
	cin >> player_idx; cin.ignore();
	int nb_games;
	cin >> nb_games; cin.ignore();

	MiniGame games[4];
	games[0].type = Runner;
	games[1].type = Archery;
	games[2].type = Skater;
	games[3].type = Diving;
	// game loop
	std::vector<Key> strategies[10]; // my strarts
	std::vector<Key> strategies_v1[10]; // my opponent 1 strats
	std::vector<Key> strategies_v2[10]; // my opponent 2 strats
	for (int i = 0; i < 10; i++) {
		std::vector<Key> current;
		current.reserve(100);
		for (int j = 0; j < 100; j++) {
			current.push_back((Key)(rand()%4));
		}
		strategies[i] = current;
		strategies_v1[i] = current;
		strategies_v2[i] = current;
	}
	int glob_scores[3];
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

		while (millis.count() < 45)
		{
			//mutate stuff here
			time_now = std::chrono::system_clock::now();
			millis = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_start);
		}
		for (int i = 0; i < 4; i++)
			games[i].display_status();
		std::cerr << "score: " << eval_of_player(evaluate(games), player_idx) << std::endl;
		std::cerr << "Took " << millis.count() << "ms" << std::endl;
		//cout << KeyStrs[strategies[0][0]] << endl;
		cout << "RIGHT" << endl;

	}
}
