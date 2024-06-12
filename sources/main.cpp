#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "MiniGame.hpp"
#include <cstdlib>

using namespace std;

double evaluate(MiniGame (&games)[4], int player) {
	//TODO: make evals return triplets, to do less recalculations
	double scores[3] = {
		games[0].evaluate(0) * games[1].evaluate(0) * games[2].evaluate(0) * games[3].evaluate(0),
		games[0].evaluate(1) * games[1].evaluate(1) * games[2].evaluate(1) * games[3].evaluate(1),
		games[0].evaluate(2) * games[1].evaluate(2) * games[2].evaluate(2) * games[3].evaluate(2),
	};
	double score_v1 = scores[player] - scores[(player + 1) % 3];
	double score_v2 = scores[player] - scores[(player + 2) % 3];
	return std::min(score_v1, score_v2);
	//TODO being 2nd is better than beng 3rd no matter the point difference
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
	std::vector<Key> strategies[10];
	for (int i = 0; i < 10; i++) {
		std::vector<Key> current;
		current.reserve(100);
		for (int j = 0; j < 100; j++) {
			current.push_back((Key)(rand()%4));
		}
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
		for (int i = 0; i < 4; i++)
		{
			games[i].update_state();
			Key keys[3] = {RIGHT, RIGHT, RIGHT};
			games[i].simulateTurn(keys);
			games[i].display_status();
			std::cerr << "===" << std::endl;
		}
		std::cerr << "score: " << evaluate(games, player_idx) << std::endl;
		cout << "RIGHT" << endl;

	}
}
