#pragma once
#include <iostream>
#include <typeinfo>
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
	TYPE_OBJECT,
	TYPE_MODEL = 100,
};

enum MouseState {
	MOUSE_IDLE = 0,
	MOUSE_PRESSED,
	MOUSE_MOVE,
	MOUSE_RELEASED
};

#define ANIM_TYPE_LIST   \
  X(ANIM_TYPE_NONE)             \
  X(ANIM_TYPE_IDLE)             \
  X(ANIM_TYPE_JUMP)            \

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

typedef enum
{
  #define X(name) name,
    ANIM_TYPE_LIST 
  #undef X
  ANIM_TYPE_SIZE
} AnimationType;

static const char* LOOKUP_NAME[TYPE_SIZE] =
{
  #define X(name) [name] = #name,
    TRIGGER_TYPE_LIST
  #undef X
};

static const char* LOOKUP_ANIM[ANIM_TYPE_SIZE] =
{
  #define X(name) [name] = #name,
    ANIM_TYPE_LIST
  #undef X
};

template <typename T>
inline T GetKey(std::string str) {
	if (typeid(TriggerType) == typeid(T)) {
		for (int i = TYPE_NONE; i < TYPE_SIZE; ++i) {
			if (str == LOOKUP_NAME[i]) {
				return static_cast<T>(i);
			}
		}
	}
	if (typeid(T) == typeid(AnimationType)) {
		for (int i = ANIM_TYPE_NONE; i < ANIM_TYPE_SIZE; ++i) {
			if (str == LOOKUP_ANIM[i]) {
				return static_cast<T>(i);
			}
		}
	}
	return static_cast<T>(TYPE_NONE);
}