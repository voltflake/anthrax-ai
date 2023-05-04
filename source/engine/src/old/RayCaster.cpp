#include "..\includes\RayCaster.h"


void RayCaster::draw(float x, float y, glm::vec3 color,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices)
{
	Vertex pos;

	pos = {{x,y}, color};
	vertices.push_back(pos);
	
	x += stepx;
	pos = {{x,y},  color};
	vertices.push_back(pos);

	y += stepy;
	pos = {{x,y}, color};
	vertices.push_back(pos);

	x -= stepx;
	pos = {{x,y},  color};
	vertices.push_back(pos);

	int k = 0;
	while (k < 6)
	{
		if (k == 0 || k == 5)
			indices.push_back(last);
		else if (k == 2)
			indices.push_back(ind);
		else if (k == 3)
		{
			indices.push_back(ind);
			ind++;
		}
		else
		{
			indices.push_back(ind);
			ind++;
		}
		k++;
	}
	last += 4;
	ind = last + 1;
}

float toRad(float angle)
{
	// if (angle >= 180)
 //  		angle-= 360;
	//return (M_PI * angle) / 180;
	return angle;
}

void RayCaster::drawRay(std::map<int,  std::vector<Walls>> &wallFin, PlayerVert &player,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices)
{
	glm::vec2 rays;
	stepx *= 0.1;
	stepy *= 0.1;

	for (int i = 0; i < wallFin.size(); i++)
	{
		float plX = (float)player.playerPos.x * 0.1;
		float plY = (float)player.playerPos.y * 0.1;
		plX -= 1;
		plY -=1;
		std::vector<Walls> tmp = wallFin[i];
		for (int j = 0; j < tmp.size(); j++)
		{
			std::cout << i <<" == " <<  tmp[j].rectBegin.x << " " << tmp[j].rectBegin.y << " ||| " <<  tmp[j].rectEnd.x << " " << tmp[j].rectEnd.y << '\n';
			std::cout << i <<" == " <<  tmp[j].rayLength<< " ||| " << tmp[j].rayLengthEnd << '\n';

			float rLenght =  (float)tmp[j].rayLength * 0.1;

			for (float r = 0; r <= rLenght; r += 0.03f) 
			{
				rays = {plX + (r * cos((toRad(tmp[j].minAngle)))),plY + (r * sin(toRad(tmp[j].minAngle)))};

				draw(rays.x, rays.y, {0.0f, 1.0f, 0.0f}, vertices, indices);
			}
			rLenght =  (float)tmp[j].rayLengthEnd * 0.1;

			for (float r = 0; r <= rLenght; r += 0.03f) 
			{
				rays = {plX + (r * cos(toRad(tmp[j].maxAngle))),plY + (r * sin(toRad(tmp[j].maxAngle)))};

				draw(rays.x, rays.y, {0.0f, 1.0f, 0.0f}, vertices, indices);
			}
		}
	}
	stepx /= 0.1;
	stepy /= 0.1;
}

void RayCaster::calculateWalls(std::map<int,  std::vector<Walls>> &wallFin, PlayerVert &player,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices)
{
	glm::vec2 rays;
	stepx *= 0.1;
	stepy *= 0.1;
	for (int i = 0; i < wallFin.size(); i++)
	{
		std::vector<Walls> tmp = wallFin[i];
		for (int j = 0; j < tmp.size(); j++)
		{
			tmp[j].columnHeightBeg = 2.0/(float)((tmp[j].rayLength)*cos(tmp[j].minAngleWall));
			tmp[j].finColumnBeg = (0.0) - tmp[j].columnHeightBeg/(float)2;

			tmp[j].columnHeightEnd = 2.0/(float)((tmp[j].rayLengthEnd)*cos(tmp[j].maxAngleWall));
			tmp[j].finColumnEnd = (0.0) - tmp[j].columnHeightEnd/(float)2;
			
			if (tmp[j].rectBegin.x == tmp[j].rectEnd.x)
			{
				tmp[j].wallLenght = tmp[j].rectBegin.y - tmp[j].rectEnd.y;
			}
			else if (tmp[j].rectBegin.y == tmp[j].rectEnd.y)
			{
				tmp[j].wallLenght = tmp[j].rectBegin.x - tmp[j].rectEnd.x;
			}
			if (tmp[j].wallLenght < 0)
				tmp[j].wallLenght *= -1;

			//tmp[j].wallLenght = 1.0 - (tmp[j].wallLenght * 0.15);
			

		//	if ( tmp[j].finColumnBeg >  tmp[j].finColumnEnd)
				//draw3DLeft(wallFin, player, vertices, indices);
			//else
				draw3DRight(wallFin, player, vertices, indices);

			return ;
			
		}
	}

	stepx /= 0.1;
	stepy /= 0.1;
}


void RayCaster::draw3DRight(std::map<int,  std::vector<Walls>> &wallFin, PlayerVert &player,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices)
{
	glm::vec2 rays;
	stepx *= 0.1;
	stepy *= 0.1;

				std::cout << wallFin.size() << "\n\n";

	if (wallFin.size() == 2)
	{
		xW = 0.0;
	}
	for (int i = wallFin.size(); i > 0; i--)
	{
		float plX = (float)player.playerPos.x * 0.1;
		float plY = (float)player.playerPos.y * 0.1;
		plX -= 1;
		plY -=1;
		std::vector<Walls> tmp = wallFin[i];
		for (int j = 0; j < tmp.size(); j++)
		{


			tmp[j].columnHeightBeg = 2.0/(float)((tmp[j].rayLength)*cos(tmp[j].minAngleWall));
			tmp[j].finColumnBeg = (0.0) - tmp[j].columnHeightBeg/(float)2;

			tmp[j].columnHeightEnd =2.0/(float)((tmp[j].rayLengthEnd)*cos(tmp[j].maxAngleWall));
			tmp[j].finColumnEnd = (0.0) - tmp[j].columnHeightEnd/(float)2;
			
			// if (tmp[j].rectBegin.x == tmp[j].rectEnd.x)
			// {
			// 	tmp[j].wallLenght = tmp[j].rectBegin.y - tmp[j].rectEnd.y;
			// }
			// else if (tmp[j].rectBegin.y == tmp[j].rectEnd.y)
			// {
			// 	tmp[j].wallLenght = tmp[j].rectBegin.x - tmp[j].rectEnd.x;
			// }
			
			//wallLenght = ((float)wallLenght/(float)2) - 2;

			// if (tmp[j].wallLenght < 0)
			// 	tmp[j].wallLenght *= -1;

			// tmp[j].wallLenght = 1.0 - (tmp[j].wallLenght * 0.1);

			// tmp[j].wallLenght =  (int)(tmp[j].wallLenght * 10 );
			// tmp[j].wallLenght /= (float)10;


			// if (wallFin.size() == 3)
			// {
			// 	std::cout << "HA\n\n";
				tmp[j].wallLenght  = 1.0;
		//	}

			std::cout << i <<" ==HBeg " << tmp[j].columnHeightBeg<< " ||| " << tmp[j].finColumnBeg<< '\n';
			std::cout << i <<" ==HEnd " << tmp[j].columnHeightEnd<< " ||| " << tmp[j].finColumnEnd<< '\n';

			std::cout << i <<" ==WallLen " << tmp[j].wallLenght<< '\n';

			Vertex pos;
			float r = 1.0f;

			pos = {{xW, tmp[j].finColumnEnd},  {r, 0.0f, 0.0f}};
			vertices.push_back(pos);
			
			xW += tmp[j].wallLenght;
			pos = {{xW, tmp[j].finColumnBeg},   {0.5, 0.0f, 0.0f}};
			vertices.push_back(pos);

			//columnHeightEnd += finColumnBeg;
			pos = {{xW, tmp[j].columnHeightBeg},  {0.5, 0.0f, 0.0f}};
			vertices.push_back(pos);

			//finColumnEnd +=columnHeightBeg;
			xW -= tmp[j].wallLenght;
			pos = {{xW, tmp[j].columnHeightEnd},   {r, 0.0f, 0.0f}};
			vertices.push_back(pos);


			xW+= tmp[j].wallLenght;

			int k = 0;
			while (k < 6)
			{
				if (k == 0 || k == 5)
					indices.push_back(last);
				else if (k == 2)
					indices.push_back(ind);
				else if (k == 3)
				{
					indices.push_back(ind);
					ind++;
				}
				else
				{
					indices.push_back(ind);
					ind++;
				}
				k++;
			}
			last += 4;
			ind = last + 1;
			// 	draw(rays.x, rays.y, {0.0f, 1.0f, 0.0f}, vertices, indices);
			
		}
	}

	stepx /= 0.1;
	stepy /= 0.1;
}

void RayCaster::draw3DLeft(std::map<int,  std::vector<Walls>> &wallFin, PlayerVert &player,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices)
{
	glm::vec2 rays;
	stepx *= 0.1;
	stepy *= 0.1;
	for (int i = 0; i < wallFin.size(); i++)
	{
		float plX = (float)player.playerPos.x * 0.1;
		float plY = (float)player.playerPos.y * 0.1;
		plX -= 1;
		plY -=1;
		std::vector<Walls> tmp = wallFin[i];
		for (int j = 0; j < tmp.size(); j++)
		{

			tmp[j].columnHeightBeg = 2.0/(float)((tmp[j].rayLength)*cos(tmp[j].minAngleWall));
			tmp[j].finColumnBeg = (0.0) - tmp[j].columnHeightBeg/(float)2;

			tmp[j].columnHeightEnd = 2.0/(float)((tmp[j].rayLengthEnd)*cos(tmp[j].maxAngleWall));
			tmp[j].finColumnEnd = (0.0) - tmp[j].columnHeightEnd/(float)2;
			
			if (tmp[j].rectBegin.x == tmp[j].rectEnd.x)
			{
				tmp[j].wallLenght = tmp[j].rectBegin.y - tmp[j].rectEnd.y;
			}
			else if (tmp[j].rectBegin.y == tmp[j].rectEnd.y)
			{
				tmp[j].wallLenght = tmp[j].rectBegin.x - tmp[j].rectEnd.x;
			}
		
			//wallLenght = ((float)wallLenght/(float)2) - 2;
			if (tmp[j].wallLenght < 0)
				tmp[j].wallLenght *= -1;
			
			tmp[j].wallLenght = 2.0 - (tmp[j].wallLenght * 0.15);

			tmp[j].wallLenght =  (int)(tmp[j].wallLenght * 10 );
			tmp[j].wallLenght /= (float)10;
			
			std::cout << i <<" ==HBeg " << tmp[j].columnHeightBeg<< " ||| " << tmp[j].finColumnBeg<< '\n';
			std::cout << i <<" ==HEnd " << tmp[j].columnHeightEnd<< " ||| " << tmp[j].finColumnEnd<< '\n';

			std::cout << i <<" ==WallLen " << tmp[j].wallLenght<< '\n';

			Vertex pos;

		float r = 1.0f;

		

			pos = {{xW, tmp[j].finColumnEnd},  {r, 0.0f, 0.0f}};
			vertices.push_back(pos);
			
			xW += tmp[j].wallLenght;
			pos = {{xW, tmp[j].finColumnBeg},   {0.5, 0.0f, 0.0f}};
			vertices.push_back(pos);

			//columnHeightEnd += finColumnBeg;
			pos = {{xW, tmp[j].columnHeightBeg},  {0.5, 0.0f, 0.0f}};
			vertices.push_back(pos);

			//finColumnEnd +=columnHeightBeg;
			xW -= tmp[j].wallLenght;
			pos = {{xW, tmp[j].columnHeightEnd},   {r, 0.0f, 0.0f}};
			vertices.push_back(pos);


			xW+= tmp[j].wallLenght;

			int k = 0;
			while (k < 6)
			{
				if (k == 0 || k == 5)
					indices.push_back(last);
				else if (k == 2)
					indices.push_back(ind);
				else if (k == 3)
				{
					indices.push_back(ind);
					ind++;
				}
				else
				{
					indices.push_back(ind);
					ind++;
				}
				k++;
			}
			last += 4;
			ind = last + 1;
			// 	draw(rays.x, rays.y, {0.0f, 1.0f, 0.0f}, vertices, indices);
			
		}
	}

	stepx /= 0.1;
	stepy /= 0.1;
}


void RayCaster::drawMap(std::map<int, std::vector<glm::vec3>> &wallBuilder,
		std::vector<Vertex> &vertices, std::vector<uint16_t> &indices)
{
	int i = 0;
	glm::vec3 xy;
	std::vector<glm::vec3> tmp;

	float tmp1 = (float)1 / (float)19;
	float tmp2 = (float)1 / (float)10;

	stepx = (int)(tmp1 * 10 + .5);
   	stepx = (float)stepx/10;
	stepy = (int)(tmp2 * 10 + .5);
   	stepy = (float)stepy/10;

	Vertex pos;

	while (i < wallBuilder.size())
	{
		tmp = wallBuilder[i];
		for (int j = 0; j < tmp.size() ; j++)
		{
			xy = tmp[j];

			x = xy.x * stepx;
			y = xy.y * stepy;

			x -=1;
			y -=1;

			draw(x, y, {1.0f, 1.0f, 0.0f}, vertices, indices);
		}
		i++;
	}
}
