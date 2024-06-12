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
	Medals medals[3];
	int simulated_turns = 0;

	void update_state() {
		std::cin >> gpu >> reg_0 >> reg_1 >> reg_2 >> reg_3 >> reg_4 >> reg_5 >> reg_6; std::cin.ignore();
		simulated_turns = 0;
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

		if (pos  >= 29)
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
	void skaterDoPlayer2(int& risk, bool shares_space) {
		if (risk < 0)
		{
			risk++;
			return ;
		}
		if (shares_space)
			risk += 2;
		if (risk >= 5)
			risk = -2;
	}
	void skaterDoTurn(Key inputs[3]) {
		if (gpu == "GAME_OVER")
			return ;
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
		skaterDoPlayer2(reg_3, shares_space[0]);
		skaterDoPlayer2(reg_4, shares_space[1]);
		skaterDoPlayer2(reg_5, shares_space[2]);
		reg_6--;
		if (!reg_6) {
			int positions[3] = {reg_0, reg_1, reg_2};
			grant_medals(positions);
			gpu = "GAME_OVER";
		}
	}
	void archeryDoPlayer(int& x, int& y, Key key) {
		int wind_force = gpu[0] - '0';
		if (key == UP)
			y -= wind_force;
		else if (key == DOWN)
			y += wind_force;
		else if (key == RIGHT)
			x += wind_force;
		else if (key == LEFT)
			x -= wind_force;
		if (x > 20)
			x = 20;
		if (y > 20)
			y = 20;
		if (x < -20)
			x = -20;
		if (y < -20)
			y = -20;
	}
	void archeryDoTurn(Key inputs[3]) {
		if (gpu == "GAME_OVER")
			return ;
		archeryDoPlayer(reg_0, reg_1, inputs[0]);
		archeryDoPlayer(reg_2, reg_3, inputs[1]);
		archeryDoPlayer(reg_4, reg_5, inputs[2]);
		gpu.erase(0, 1); // remove first char
		if (gpu.empty())
		{
			gpu = "GAME_OVER";
			int dists[3] = {
				-(reg_0 * reg_0 + reg_1 * reg_1),
				-(reg_2 * reg_2 + reg_3 * reg_3),
				-(reg_4 * reg_4 + reg_5 * reg_5),
			};
			grant_medals(dists);
		}
	}
	void divingDoPlayer(int& score, int& combo, Key key) {
		char gpu_key = "UDLR"[key];
		if (gpu[0] == gpu_key) {
			combo++;
			score += combo;
		} else {
			combo = 0;
		}
	}
	void divingDoTurn(Key inputs[3]) {
		if (gpu == "GAME_OVER")
			return ;
		divingDoPlayer(reg_0, reg_3, inputs[0]);
		divingDoPlayer(reg_1, reg_4, inputs[1]);
		divingDoPlayer(reg_2, reg_5, inputs[2]);
		gpu.erase(0, 1); // remove first char
		if (gpu.empty()) {
			gpu = "GAME_OVER";
			int scores[3] = {reg_0, reg_1, reg_2};
			grant_medals(scores);
		}
	}
	void display_status() {
		if (gpu == "GAME_OVER") {
			std::cerr << "GAME_OVER" << std::endl;
			return ;
		}
		switch(type) {
			case Runner:
				runner_display();
				break;
			case Skater:
				skater_display();
				break;
			case Archery:
				archery_display();
				break;
			case Diving:
				diving_display();
				break;
			default:
				throw std::runtime_error("unknown type");
		}
		simulated_turns++;
	}
	void runner_display() {
		std::string field;
		field = gpu;
		field[reg_0] = '@';
		std::cerr << "{" << reg_1 << "," << reg_4 << "}" << field << std::endl;
		field = gpu;
		field[reg_1] = '@';
		std::cerr << "{" << reg_1 << "," << reg_4 << "}" << field << std::endl;
		field = gpu;
		field[reg_2] = '@';
		std::cerr << "{" << reg_2 << "," << reg_5 << "}" << field << std::endl;
	}
	void skater_display() {
		char field[41] = "   |   |   |   |   |   |   |   |   |   |";
		field[(reg_0 % 10) * 4 + 0] = '1';
		field[(reg_1 % 10) * 4 + 1] = '2';
		field[(reg_2 % 10) * 4 + 2] = '3';
		std::cerr << field << std::endl;
		std::cerr << reg_0 << "," << reg_3 << std::endl;
		std::cerr << reg_1 << "," << reg_4 << std::endl;
		std::cerr << reg_2 << "," << reg_5 << std::endl;
	}
	void archery_display() {
		std::cerr << reg_0 << "," << reg_1 << std::endl;
		std::cerr << reg_2 << "," << reg_3 << std::endl;
		std::cerr << reg_4 << "," << reg_5 << std::endl;
		std::cerr << gpu << std::endl;
	}
	void diving_display() {
		std::cerr << reg_0 << "," << reg_3 << std::endl;
		std::cerr << reg_1 << "," << reg_4 << std::endl;
		std::cerr << reg_2 << "," << reg_5 << std::endl;
		std::cerr << gpu << std::endl;
	}
	double evaluate(int player);
};
