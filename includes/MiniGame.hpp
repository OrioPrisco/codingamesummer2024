#pragma once

#include <string>
#include "Key.hpp"
#include <iostream>
#include <array>

struct Medals {
	int gold = 0;
	int silver = 0;
	int bronze = 0;
};

enum Type {
	Runner,
	Skater,
	Archery,
	Diving,
};

class MiniGame {
public:
	Type type;
	std::string gpu;
	int reg_0;
	int reg_1;
	int reg_2;
	int reg_3;
	int reg_4;
	int reg_5;
	int reg_6;
	int turn = -1;
	Medals medals[3];
	int simulated_turns = 0;

	void update_state() {
		std::cin >> gpu >> reg_0 >> reg_1 >> reg_2 >> reg_3 >> reg_4 >> reg_5 >> reg_6; std::cin.ignore();
		simulated_turns = 0;
		turn++;
		if (gpu == "GAME_OVER")
			turn = -1;
	}

	void simulateTurn(Key inputs[3]) {
		switch(type) {
			case Runner:
				runnerDoTurn(inputs);
				break;
			case Skater:
				skaterDoTurn(inputs);
				break;
			case Archery:
				archeryDoTurn(inputs);
				break;
			case Diving:
				divingDoTurn(inputs);
				break;
			default:
				throw std::runtime_error("unknown type");
		}
		simulated_turns++;
	}
	void grant_medals(int scores[3]) {
		int sec_max = 0;
		int max = 0;
		for (int i = 0; i < 3; i++) {
			if (scores[i] > max) {
				sec_max = max;
				max = scores[i];
			} else if (scores[i] > sec_max) {
				sec_max = scores[i];
			}
		}
		for (int i = 0; i < 3; i++)
		{
			if (scores[i] >= max) {
				medals[i].gold++;
			} else if (scores[i] == sec_max) {
				medals[i].silver++;
			} else {
				medals[i].bronze++;
			}
		}
	}
	static constexpr int STUN_DURATION = 2;
	static constexpr int FIELD_SIZE = 30;
	bool runnerDoPlayer(int& pos, int& stun, Key key) {
		if (stun) {
			stun--;
			return false;
		}
		if (key == UP) {
			pos += 2;
		}
		else if (key == LEFT) {
			pos++;
		}
		else if (key == DOWN) {
			pos++;
			if (gpu[pos] && gpu[pos] != '#')
				pos++;
		}
		else if (key == RIGHT) {
			pos++;
			if (gpu[pos] && gpu[pos] != '#')
				pos++;
			if (gpu[pos] && gpu[pos] != '#')
				pos++;
		}

		if (pos < 29)
			return true;
		if (gpu[pos] == '#')
			stun = STUN_DURATION;
		return false;
	}
	void runnerDoTurn(Key inputs[3]) {
		if (gpu == "GAME_OVER")
			return ;
		bool done = false;
		done |= runnerDoPlayer(reg_0, reg_3, inputs[0]);
		done |= runnerDoPlayer(reg_1, reg_4, inputs[1]);
		done |= runnerDoPlayer(reg_2, reg_5, inputs[2]);
		if (done) {
			int positions[3] = {
				std::min(FIELD_SIZE - 1, reg_0),
				std::min(FIELD_SIZE - 1, reg_1),
				std::min(FIELD_SIZE - 1, reg_2)
			};

			grant_medals(positions);
			gpu = "GAME_OVER";
		}
	}
	void skaterDoPlayer(int& pos, int& risk, Key key) {
		if (risk < 0)
		{
			risk++;
			return ;
		}
		char gpu_key = "UDLR"[key];
		if (gpu_key == gpu[0])
		{
			if (risk)
				risk--;
			pos++;
		} else if (gpu_key == gpu[1]) {
			pos += 2;
		} else if (gpu_key == gpu[2]) {
			pos += 2;
			risk++;
		} else if (gpu_key == gpu[3]) {
			pos += 3;
			risk += 2;
		}
	}
	void skaterDoTurn(Key inputs[3]) {
		if (simulated_turns == 1)
			return ; //Risk is random every turn, unpredictable
		skaterDoPlayer(reg_0, reg_3, inputs[0]);
		skaterDoPlayer(reg_1, reg_4, inputs[1]);
		skaterDoPlayer(reg_2, reg_5, inputs[2]);
		bool shares_space[3];
		if (reg_0 % 10 == reg_1 % 10)
		{
			shares_space[0] = true;
			shares_space[1] = true;
		}
		if (reg_1 % 10 == reg_2 % 10)
		{
			shares_space[1] = true;
			shares_space[2] = true;
		}
		if (reg_0 % 10 == reg_2 % 10)
		{
			shares_space[0] = true;
			shares_space[2] = true;
		}
		if (reg_3 >= 0 && shares_space[0])
			reg_3 += 2;
		if (reg_4 >= 0 && shares_space[1])
			reg_4 += 2;
		if (reg_5 >= 0 && shares_space[2])
			reg_5 += 2;
		if (!reg_6) {
			int positions[3] = {reg_0, reg_1, reg_2};
			grant_medals(positions);
			gpu = "GAME_OVER";
		}
		reg_6--;
	}
	void archeryDoTurn(Key inputs[3]);
	void divingDoTurn(Key inputs[3]);
	double evaluate(int player);
};
