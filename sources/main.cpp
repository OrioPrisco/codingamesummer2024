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
		std::string data;
};

struct RunnerPlayer {
	int position;
	int stunned;
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

	void update_state() {
		cin >> gpu >> reg_0 >> reg_1 >> reg_2 >> reg_3 >> reg_4 >> reg_5 >> reg_6; cin.ignore();
	}

};

enum Output {
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

const char *Outputstrs[] = {
	"UP",
	"DOWN",
	"LEFT",
	"RIGHT",
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
		for (typename std::vector<T>::const_iterator it = vec.begin();it != vec.end(); ++it) {
		os << *it << ", ";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const RunnerPlayer& player) {
	os << "{" << player.position << ";" << player.stunned << "}";
	return os;
}
//typedef std::vector<std::pair<Output, int>> DesiredOut;
//#define ANY_OUT {{ {UP,0}, {DOWN,0}, {LEFT,0}, {RIGHT,0} }}

typedef std::vector<int> DesiredOut;
#define ANY_OUT {{ 0, 0, 0, 0, }}

struct RunnerGame {
	RunnerField field;
	int my_index;
	RunnerPlayer players[3];
	void load_state(const GameState& state) {
		field.data = state.gpu;
		players[0] = {state.reg_0, state.reg_3};
		players[1] = {state.reg_1, state.reg_4};
		players[2] = {state.reg_2, state.reg_5};
	}
	DesiredOut oneTurnLookAhead() const {
		DesiredOut out(4);
		std::cerr << "Players :" << players[0] << " " << players[1] << " " << players[2] << std::endl;
		if (field.data == "GAME_OVER" || players[my_index].stunned) {
			std::cerr << "No opinion" << std::endl;
			return ANY_OUT;
		}
		int my_pos = players[my_index].position;
		int to_check = 30 - my_pos;
		//std::cerr << "pos:" << my_pos << "; to_check" << to_check << std::endl;
		std::string foo = field.data;
		foo[my_pos] = '@';
		std::cerr << foo << "(" << my_pos << ")" << std::endl;
		if (to_check == 0)
		{
			std::cerr << "0 tiles to check ??" << std::endl;
			return ANY_OUT;
		}
		// if hedge in front jump
		if (field.data[my_pos + 1] == '#')
		{
			out[UP] = 2;
			out[DOWN] = -4;
			out[LEFT] = -4;
			out[RIGHT] = -4;
			return out;
		}
		else
			out[LEFT] = 1;
		if (to_check == 1)
		{
			return ANY_OUT;
		}
		if (field.data[my_pos + 1] != '#'
		 && field.data[my_pos + 2] != '#')
		{
			out[DOWN] = 2;
			out[UP] = 2;
			//std::cerr << "2 tiles are clear" << std::endl;
		}
		else
		{
			out[DOWN] = -4;
			out[UP] = -4;
		}
		if (to_check == 2) {
			out[RIGHT] = out[DOWN];
			return out;
		}
		if (field.data[my_pos + 1] != '#'
		 && field.data[my_pos + 2] != '#'
		 && field.data[my_pos + 3] != '#')
		{
			out[RIGHT] = 3;
			//std::cerr << "Coast is clear" << std::endl;
		}
		else
			out[RIGHT] = -4;

		return out;
	}
};


int main()
{
	int player_idx;
	cin >> player_idx; cin.ignore();
	int nb_games;
	cin >> nb_games; cin.ignore();

	std::vector<GameState> gameStates(nb_games);
	std::vector<RunnerGame> games(nb_games);
	for (int i = 0; i < nb_games; i++) 
		games[i].my_index = player_idx;
	// game loop
	while (1) {
		for (int i = 0; i < 3; i++) {
			string score_info;
			getline(cin, score_info);
		}
		DesiredOut outs[4];
		DesiredOut total(4);
		for (int i = 0; i < nb_games; i++) {
			gameStates[i].update_state();
			games[i].load_state(gameStates[i]);
			outs[i] = games[i].oneTurnLookAhead();
			for (int j = 0; j < 4; j++)
				total[j] += outs[i][j];
			std::cerr << "Game " << i << ": " << outs[i] << std::endl;
		}
		auto it = std::max_element(total.begin(), total.end());
		int output = it - total.begin();
		cout << Outputstrs[output] << endl;

	}
}
