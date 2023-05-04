#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <map>

#include "Utils.h"

class MapBuilder
{
	public:
		std::map<int, std::vector<glm::vec3>> &getPlayer(PlayerVert &player);
		std::map<int,  std::vector<Walls>> &mapBuilder(PlayerVert &player, float &newAlpha);

		void cleanWalls() { wallFin.clear(); finWall = 0; side = 0;};

	private:

		std::vector<std::string>  strMap = {"111111111111111111111111111",
											"100000000000000000000000001",
											"100000000000000000000000001",
											"100000000000000000000000001",
											"1000p0000000000000000000001",
											"100000000000000000000000001",
											"111111111111111111111111111"};
				

		glm::vec2 rays;
		int finWall = 0;
		int numWall = 0;
		int side = 0;
		int tmpNext = 0;
		int tmpNextSec = 0;

	 	float alpha;

	 	const float fov = M_PI/3.;

		std::map<int, std::vector<int>> finMap;
	 	std::map<int, std::vector<glm::vec3>> wallBuilder;
	 	std::map<int,  std::vector<Walls>> wallFin;

	 	bool checkColums(int i, int j);
	 	bool checkRows(int i, int j);
	 	void parseMap(PlayerVert &player);

		glm::vec3 getLastRay(float &currentAngle, PlayerVert &player);
		void rayProc(float &currentAngle, float maxAngle, PlayerVert &player);
		float getRlength(PlayerVert &player, float &currentAngle);
		bool angleProc(float &currentAngle, float maxAngle, glm::vec3 &rect,  PlayerVert &player);
		int checkBoards(float &currentAngle,float maxAngle,  glm::vec3 &rect, PlayerVert &player);
		int checkSide(int x, int y);
		void xSideProc(glm::vec2 rayInfo, float rayLength, float &currentAngle, float maxAngle,PlayerVert &player);
		void ySideProc(glm::vec2 rayInfo, float rayLength, float &currentAngle, float maxAngle,PlayerVert &player);

};

#endif