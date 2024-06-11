#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

struct RunnerField {
		char data[30];
};

struct RunnerPlayer {
	int position;
	int stunned;
};

struct RunnerGame {
	RunnerField field;
	int my_index;
	RunnerPlayer players[3];
};

struct GameState {
	string gpu;
	int reg_0;
	int reg_1;
	int reg_2;
	int reg_3;
	int reg_4;
	int reg_5;
	int reg_6;

};

int main()
{
	int player_idx;
	cin >> player_idx; cin.ignore();
	int nb_games;
	cin >> nb_games; cin.ignore();

	// game loop
	while (1) {
		for (int i = 0; i < 3; i++) {
			string score_info;
			getline(cin, score_info);
		}
		for (int i = 0; i < nb_games; i++) {

				string gpu;
				int reg_0;
				int reg_1;
				int reg_2;
				int reg_3;
				int reg_4;
				int reg_5;
				int reg_6;
				cin >> gpu >> reg_0 >> reg_1 >> reg_2 >> reg_3 >> reg_4 >> reg_5 >> reg_6; cin.ignore();

		}
					cout << "UP" << endl;

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;


	}
}
