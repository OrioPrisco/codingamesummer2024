#pragma once

enum Key {
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

constexpr const char *KeyStrs[] = {
	"UP",
	"DOWN",
	"LEFT",
	"RIGHT",
};

constexpr char charToKey[] = {
//NUL
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//A
0,0,0,DOWN,0,0,0,0,0,0,0,LEFT,0,0,0,0,0,RIGHT,0,0,UP,0,0,0,0,0,
};
