#pragma once

#include "parameters.hpp"
#include <string>
#include "Key.hpp"
#include <iostream>
#include <array>
#include <climits>

struct Medals {
	int gold = 0;
	int silver = 0;
	int bronze = 0;
};


std::ostream& operator<<(std::ostream& o, const Medals& medals) {
	return o << medals.gold << "G " << medals.silver << "S " << medals.bronze << "B";
}

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
	int regs[7];
	Medals medals[3];
	int simulated_turns = 0;

	void update_state() {
		std::cin >> gpu >> regs[0] >> regs[1] >> regs[2] >> regs[3] >> regs[4] >> regs[5] >> regs[6]; std::cin.ignore();
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
		if (gpu[0] == 'G')
			return ;
		bool done = false;
		done |= runnerDoPlayer(regs[0], regs[3], inputs[0]);
		done |= runnerDoPlayer(regs[1], regs[4], inputs[1]);
		done |= runnerDoPlayer(regs[2], regs[5], inputs[2]);
		if (done) {
			int positions[3] = {
				std::min(FIELD_SIZE - 1, regs[0]),
				std::min(FIELD_SIZE - 1, regs[1]),
				std::min(FIELD_SIZE - 1, regs[2])
			};

			grant_medals(positions);
			gpu = "GAME_OVER";
		}
	}
	void runnerDoTurns(const Strat inputs[3], int turn) {
		//TODO : calculate position of leader, and check if they could finish or not
		if (gpu[0] == 'G')
			return;
		bool done = false;
		Strat p1 = inputs[0];
		Strat p2 = inputs[1];
		Strat p3 = inputs[2];
		for(uint8_t i = 0; i < MOVE_PER_STRAT && turn + i < MAX_TURN; ++i) {
			done |= runnerDoPlayer(regs[0], regs[3], (Key)(p1 & 3));
			done |= runnerDoPlayer(regs[1], regs[4], (Key)(p2 & 3));
			done |= runnerDoPlayer(regs[2], regs[5], (Key)(p3 & 3));
			if (done) {
				int positions[3] = {
					std::min(FIELD_SIZE - 1, regs[0]),
					std::min(FIELD_SIZE - 1, regs[1]),
					std::min(FIELD_SIZE - 1, regs[2])
				};

				grant_medals(positions);
				gpu = "GAME_OVER";
				return;
			}
			p1 >>= 2;
			p2 >>= 2;
			p3 >>= 2;
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
		if (gpu[0] == 'G')
			return ;
		if (simulated_turns)
			return ; //Risk is random every turn, unpredictable
		skaterDoPlayer(regs[0], regs[3], inputs[0]);
		skaterDoPlayer(regs[1], regs[4], inputs[1]);
		skaterDoPlayer(regs[2], regs[5], inputs[2]);
		bool shares_space[3] = {false, false, false};
		if (regs[0] % 10 == regs[1] % 10)
		{
			shares_space[0] = true;
			shares_space[1] = true;
		}
		if (regs[1] % 10 == regs[2] % 10)
		{
			shares_space[1] = true;
			shares_space[2] = true;
		}
		if (regs[0] % 10 == regs[2] % 10)
		{
			shares_space[0] = true;
			shares_space[2] = true;
		}
		skaterDoPlayer2(regs[3], shares_space[0]);
		skaterDoPlayer2(regs[4], shares_space[1]);
		skaterDoPlayer2(regs[5], shares_space[2]);
		regs[6]--;
		if (!regs[6]) {
			int positions[3] = {regs[0], regs[1], regs[2]};
			grant_medals(positions);
			gpu = "GAME_OVER";
		}
	}
	void skaterDoTurns(const Strat inputs[3], int turn) {
		if (turn + regs[6] >= MAX_TURN) {
			gpu = "GAME_OVER";// makes evaluation ignore partial score
			return;
		}
		Key keys[3] = { (Key)(inputs[0] & 3), (Key)(inputs[1] & 3), (Key)(inputs[2] & 3)};
		skaterDoTurn(keys);
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
		if (gpu[0] == 'G')
			return ;
		archeryDoPlayer(regs[0], regs[1], inputs[0]);
		archeryDoPlayer(regs[2], regs[3], inputs[1]);
		archeryDoPlayer(regs[4], regs[5], inputs[2]);
		gpu.erase(0, 1); // remove first char
		if (gpu.empty())
		{
			gpu = "GAME_OVER";
			int dists[3] = {
				-(regs[0] * regs[0] + regs[1] * regs[1]),
				-(regs[2] * regs[2] + regs[3] * regs[3]),
				-(regs[4] * regs[4] + regs[5] * regs[5]),
			};
			grant_medals(dists);
		}
	}
	void archeryDoTurns(const Strat inputs[3], int turn) {
		if (gpu[0] == 'G')
			return;
		if (turn + regs[6] >= MAX_TURN) {
			gpu = "GAME_OVER";// makes evaluation ignore partial score
			return;
		}
		Strat p1 = inputs[0];
		Strat p2 = inputs[1];
		Strat p3 = inputs[2];
		for(uint8_t i = 0; i < MOVE_PER_STRAT && turn + i < MAX_TURN; ++i) {
			archeryDoPlayer(regs[0], regs[1], (Key)(p1 & 3));
			archeryDoPlayer(regs[2], regs[3], (Key)(p2 & 3));
			archeryDoPlayer(regs[4], regs[5], (Key)(p3 & 3));
			gpu.erase(0, 1); // remove first char
			if (gpu.empty())
			{
				gpu = "GAME_OVER";
				int dists[3] = {
					-(regs[0] * regs[0] + regs[1] * regs[1]),
					-(regs[2] * regs[2] + regs[3] * regs[3]),
					-(regs[4] * regs[4] + regs[5] * regs[5]),
				};
				grant_medals(dists);
				return;
			}
			p1 >>= 2;
			p2 >>= 2;
			p3 >>= 2;
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
		if (gpu[0] == 'G')
			return ;
		divingDoPlayer(regs[0], regs[3], inputs[0]);
		divingDoPlayer(regs[1], regs[4], inputs[1]);
		divingDoPlayer(regs[2], regs[5], inputs[2]);
		gpu.erase(0, 1); // remove first char
		if (gpu.empty()) {
			gpu = "GAME_OVER";
			int scores[3] = {regs[0], regs[1], regs[2]};
			grant_medals(scores);
		}
	}
	void divingDoTurns(const Strat inputs[3], int turn) {
		if (gpu[0] == 'G')
			return ;
		Strat p1 = inputs[0];
		Strat p2 = inputs[1];
		Strat p3 = inputs[2];
		for(uint8_t i = 0; i < MOVE_PER_STRAT && turn + i < MAX_TURN; ++i) {
			divingDoPlayer(regs[0], regs[3], (Key)(p1 & 3));
			divingDoPlayer(regs[1], regs[4], (Key)(p2 & 3));
			divingDoPlayer(regs[2], regs[5], (Key)(p3 & 3));
			gpu.erase(0, 1); // remove first char
			if (gpu.empty()) {
				gpu = "GAME_OVER";
				int scores[3] = {regs[0], regs[1], regs[2]};
				grant_medals(scores);
				return;
			}
			p1 >>= 2;
			p2 >>= 2;
			p3 >>= 2;
		}
	}
	static constexpr const char* names[] = {
		"RUNNER",
		"SKATER",
		"ARCHERY",
		"DIVING",
	};
	void display_medals() const {
		Evaluation score = evaluate(100);
		std::cerr
			<< names[type]
			<< medals[0] << ":" << score[0] << std::endl
			<< medals[1] << ":" << score[1] << std::endl
			<< medals[2] << ":" << score[2] << std::endl
		;
	}
	void display_status() const {
		if (gpu[0] == 'G') {
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
	}
	void runner_display() const {
		std::string field;
		field = gpu;
		field[regs[0]] = '@';
		std::cerr << "{" << regs[1] << "," << regs[4] << "}" << field << std::endl;
		field = gpu;
		field[regs[1]] = '@';
		std::cerr << "{" << regs[1] << "," << regs[4] << "}" << field << std::endl;
		field = gpu;
		field[regs[2]] = '@';
		std::cerr << "{" << regs[2] << "," << regs[5] << "}" << field << std::endl;
	}
	void skater_display() const {
		char field[41] = "   |   |   |   |   |   |   |   |   |   |";
		field[(regs[0] % 10) * 4 + 0] = '1';
		field[(regs[1] % 10) * 4 + 1] = '2';
		field[(regs[2] % 10) * 4 + 2] = '3';
		std::cerr << field << std::endl;
		std::cerr << regs[0] << "," << regs[3] << std::endl;
		std::cerr << regs[1] << "," << regs[4] << std::endl;
		std::cerr << regs[2] << "," << regs[5] << std::endl;
		std::cerr << "turns: " << regs[6] << std::endl;
	}
	void archery_display() const {
		std::cerr << regs[0] << "," << regs[1] << std::endl;
		std::cerr << regs[2] << "," << regs[3] << std::endl;
		std::cerr << regs[4] << "," << regs[5] << std::endl;
		std::cerr << gpu << std::endl;
	}
	void diving_display() const {
		std::cerr << regs[0] << "," << regs[3] << std::endl;
		std::cerr << regs[1] << "," << regs[4] << std::endl;
		std::cerr << regs[2] << "," << regs[5] << std::endl;
		std::cerr << gpu << std::endl;
	}
	typedef std::array<double, 3> Evaluation;
	Evaluation skaterEvaluate() const {
		if (gpu[0] == 'G')
			return {0,0,0}; // exact score is known
		return {
			((double)regs[0] + (regs[3]>=0?-regs[3]*0.2:regs[3]*2))/10,
			((double)regs[1] + (regs[4]>=0?-regs[4]*0.2:regs[4]*2))/10,
			((double)regs[2] + (regs[5]>=0?-regs[5]*0.2:regs[5]*2))/10,
		};
	}
	Evaluation runnerEvaluate() const {
		if (gpu[0] == 'G')
			return {0,0,0}; // exact score is known
		return {
			((double)regs[0] - regs[3] * 2)/10,
			((double)regs[1] - regs[4] * 2)/10,
			((double)regs[2] - regs[5] * 2)/10,
		};
		// TODO : take dist_left into account
	}
	static constexpr int max_archery_dist = 20 * 20;
	Evaluation archeryEvaluate() const {
		if (gpu[0] == 'G')
			return {0, 0, 0}; // exact score is known
		return {
			(double)(max_archery_dist - (regs[0] * regs[0] + regs[1] * regs[1])) /max_archery_dist,
			(double)(max_archery_dist - (regs[2] * regs[2] + regs[3] * regs[3])) /max_archery_dist,
			(double)(max_archery_dist - (regs[4] * regs[4] + regs[5] * regs[5])) /max_archery_dist,
		};
	}
	//TODO:
	// calculate max possible score for self and opp, and if
	// result is guaranteed ignore this minigame
	Evaluation divingEvaluate() const {
		if (gpu[0] == 'G')
			return {0, 0, 0}; // exact score is known
		return {
			((double)regs[0] + regs[3]*0.1)/10,
			((double)regs[1] + regs[4]*0.1)/10,
			((double)regs[2] + regs[5]*0.1)/10,
		};
	}
	Evaluation evaluate(int turn) const {
		Evaluation scores;
		scores[0] = medals[0].silver + medals[0].gold * 3 + (turn < 50 ? (50 - turn)/10:0);
		scores[1] = medals[1].silver + medals[1].gold * 3 + (turn < 50 ? (50 - turn)/10:0);
		scores[2] = medals[2].silver + medals[2].gold * 3 + (turn < 50 ? (50 - turn)/10:0);
		if (gpu[0] == 'G')
			return scores;
		Evaluation partial_scores;
		switch(type) {
			case Runner:
				partial_scores = runnerEvaluate();
				break;
			case Skater:
				partial_scores = skaterEvaluate();
				break;
			case Archery:
				partial_scores = archeryEvaluate();
				break;
			case Diving:
				partial_scores = divingEvaluate();
				break;
			default:
				throw std::runtime_error("unknown type");
		}
		int positions[3];
		positions[0] = (partial_scores[0] >= partial_scores[1]) + (partial_scores[0] >= partial_scores[2]);
		positions[1] = (partial_scores[1] >= partial_scores[0]) + (partial_scores[1] >= partial_scores[2]);
		positions[2] = (partial_scores[2] >= partial_scores[0]) + (partial_scores[2] >= partial_scores[1]);
		scores[0] += ((double) positions[0]) /2;
		scores[1] += ((double) positions[1]) /2;
		scores[2] += ((double) positions[2]) /2;
		return scores;
	}
};
