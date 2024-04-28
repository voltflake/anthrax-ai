#pragma once
#include <iostream>

enum Physics {
	IDLE 		= 1 << 0,
	MOVE_RIGHT 	= 1 << 1,
	MOVE_UP 	= 1 << 2,
	MOVE_LEFT 	= 1 << 3,
	MOVE_DOWN 	= 1 << 4,
	JUMP 		= 1 << 5
};

enum DataTypes {
	TYPE_PLAYER = 0,
	TYPE_BACKGROUND,
	TYPE_OBJECT
};

enum MouseState {
	MOUSE_IDLE = 0,
	MOUSE_PRESSED,
	MOUSE_MOVE,
	MOUSE_RELEASED
};

#define TRIGGER_TYPE_LIST   \
  X(TYPE_NONE)              \
  X(TYPE_TEXT)              \
  X(TYPE_ACTION)            \

typedef enum
{
  #define X(name) name,
    TRIGGER_TYPE_LIST
  #undef X
  TYPE_SIZE
} TriggerType;

static const char* LOOKUP_NAME[TYPE_SIZE] =
{
  #define X(name) [name] = #name,
    TRIGGER_TYPE_LIST
  #undef X
};

template <typename T>
inline T GetKey(std::string str) {
	for (int i = TYPE_NONE; i < TYPE_SIZE; ++i) {
		if (str == LOOKUP_NAME[i]) {
			return static_cast<T>(i);
		}
	}
	return TYPE_NONE;
}