#ifndef RAYCASTER_H
#define RAYCASTER_H
#define _USE_MATH_DEFINES

#include <iostream>
#include <map>
#include "Utils.h"

class RayCaster
{
	private:
		int last = 0;
		int ind = 1;
		float x = -1.0f;
		float y = -1.0f;
		float xW = -1.0;

		float stepx;
		float stepy;

		void draw(float x, float y, glm::vec3 color,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices);


	public:
		void cleanAll() { x = -1.0f; y = -1.0f; last = 0; ind = 1; xW = -1.0; };

		void drawMap(std::map<int, std::vector<glm::vec3>> &wallBuilder,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices);

		void drawRay(std::map<int,  std::vector<Walls>> &wallFin, PlayerVert &player,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices);

		void draw3DRight(std::map<int,  std::vector<Walls>> &wallFin, PlayerVert &player,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices);

		void draw3DLeft(std::map<int,  std::vector<Walls>> &wallFin, PlayerVert &player,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices);

		void calculateWalls(std::map<int,  std::vector<Walls>> &wallFin, PlayerVert &player,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices);
};

#endif