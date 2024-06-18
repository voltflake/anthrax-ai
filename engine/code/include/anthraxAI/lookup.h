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
	TYPE_UNDEFINED = -1,
	TYPE_PLAYER = 0,
	TYPE_BACKGROUND,
	TYPE_OBJECT,
	TYPE_MODEL = 100,
	TYPE_GIZMO = 200,
};

enum MouseState {
	MOUSE_IDLE = 0,
	MOUSE_PRESSED,
	MOUSE_MOVE,
	MOUSE_RELEASED,
	MOUSE_SELECTED
};

enum GizmoAxis {
	AXIS_UNDEF = -1,
	AXIS_Z = TYPE_GIZMO + 0,
	AXIS_Y = TYPE_GIZMO + 1,
	AXIS_X = TYPE_GIZMO + 2,
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

template <typename T>
inline T GetKey(std::string str) {
	if (typeid(TriggerType) == typeid(T)) {
		if (str == "TYPE_NONE")
			return static_cast<T>(TYPE_NONE);
		if (str ==  "TYPE_TEXT")
			return static_cast<T>(TYPE_TEXT);
		if (str ==  "TYPE_ACTION")
			return static_cast<T>(TYPE_ACTION);
	}
	if (typeid(T) == typeid(AnimationType)) {
		if (str ==  "ANIM_TYPE_NONE")
			return static_cast<T>(ANIM_TYPE_NONE);
		if (str == "ANIM_TYPE_IDLE")
			return static_cast<T>(ANIM_TYPE_IDLE);
		if (str == "ANIM_TYPE_JUMP")
			return static_cast<T>(ANIM_TYPE_JUMP);
	}
	return static_cast<T>(TYPE_NONE);
}

template <typename T>
inline const char* GetValue(T key) {
		switch(key) {
			case static_cast<T>(TYPE_NONE):
				return "TYPE_NONE";
			case static_cast<T>(TYPE_TEXT):
				return "TYPE_TEXT";
			case static_cast<T>(TYPE_ACTION):
				return "TYPE_ACTION";
		}
	return "";
}