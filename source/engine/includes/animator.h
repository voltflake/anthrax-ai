#pragma once

#include "vkdefines.h"

class Animator {
	public:
	int ID;
    void settype(AnimationType t) { type = t; };
    AnimationType gettype() { return type; };

	void setpath(std::string str) { path = str; };
	void setposition(Positions postmp) { pos = postmp; };

	void setoffset(float offs) { offset = 1.0f / offs; };
	
	void setanimsize(Positions size) { animsize = size; };
	Positions getanimsize() { return animsize; };

	std::string getpath() { return path; };
	Positions getposition() { return pos; };

	void setfps(float fps) { framecounterlim = MAX_FPS / fps;};
	float getfps() { return framecounterlim;};

	float getoffset() { return offset; };

	float getframe() { return frame; };
	void setframe(float f) { frame = f; };
	
	float getframecounter() { return framecounter; };
	void setframecounter(float f) { framecounter = f; };

	private:
	std::string path = "";
	Positions 	pos = {0, 0};
	Positions 	animsize = {0, 0};
    AnimationType type;

    float offset = 0.0f;
	float frame = 0.0f;
	float framecounter = 0.0f;
	float framecounterlim = 0.0f;
};
