#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "MiniGame.hpp"

using namespace std;

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
	while (1) {
		for (int i = 0; i < 3; i++) {
			string score_info;
			getline(cin, score_info);
			// updates medals in games
		}
		for (int i = 0; i < 4; i++)
		{
			games[i].update_state();
			Key keys[3] = {RIGHT, RIGHT, RIGHT};
			games[i].simulateTurn(keys);
			games[i].display_status();
			std::cerr << "===" << std::endl;
		}
		cout << "RIGHT" << endl;

	}
}
