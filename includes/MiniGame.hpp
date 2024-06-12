#pragma once

#include <string>
#include "Key.hpp"
#include <iostream>
#include <array>

struct Medals {
	int gold;
	int silver;
	int bronze;
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
	void runnerDoTurn(Key inputs[3]);
	void skaterDoTurn(Key inputs[3]);
	void archeryDoTurn(Key inputs[3]);
	void divingDoTurn(Key inputs[3]);
	double evaluate(int player);
};
