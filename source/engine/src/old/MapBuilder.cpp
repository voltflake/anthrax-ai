
#include "..\includes\MapBuilder.h"

bool MapBuilder::checkColums(int i, int j)
{
	if ((i + 1) < strMap.size() && (strMap[i+1][j] == '1'))
	{
		return true;
	}
	else if ((i - 1) > 0 && (strMap[i -1][j] == '1' ))
	{
		return true;
	}
	return false;
}

bool MapBuilder::checkRows(int i, int j)
{
	if ((j + 1) < strMap[i].length() && (strMap[i][j+1] == '1' || strMap[i][j+1] == ','))
	{
		return true;
	}
	else if ((j - 1) > 0 && (strMap[i][j-1] == '1'|| strMap[i][j-1] == ','))
	{
		return true;
	}
	return false;
}

void MapBuilder::parseMap(PlayerVert &player)
{
	int i = 0;
	int checker = 1;

	while (i < strMap.size())
	{
		for (int j = 0; strMap[i][j]; j++)
		{

			if (strMap[i][j] == 'p')
			{
				player.playerPos = {j,i};
				player.oldPos = {j,i};
				strMap[i][j] = '0';
			}
			if (strMap[i][j] == '0')
			{
				finMap[i].push_back(0);
				strMap[i][j] = '0';
			}
			if (strMap[i][j] == '1')
			{
				if (checkRows(i,j))
				{
					for (; strMap[i][j]; j++)
					{
						if (j != 0 && j != strMap[i].length()-1 && checkColums(i, j))
						{
							if ( strMap[i][j+1] != '0')
								checker++;
						}
						if (strMap[i][j] != '1')
							break;
						finMap[i].push_back(checker);
						strMap[i][j] = ',';
					}
					j = 0;
					checker++;
				}
				else
				{
					int tmp;

					if (checkRows(i-1,j))
					{
						finMap[i].push_back(checker);
						strMap[i][j] = '.';	
						checker++;
					}
					else
					{
						std::vector<int> vec = finMap[i-1];
						tmp = vec[j];
						if (tmp == 0)
						{
							tmp = checker;
							checker++;
						}
						finMap[i].push_back(tmp);
						strMap[i][j] = '.';		
					}
								
				}

			}
		}
		i++;
	}

		for (auto &x : finMap)
	{
		std::cout << x.first << ": ";
		for (auto k : x.second)
		{
			std::cout << " |" << k << "| "; 
		}
		std::cout << '\n';

	}
}


float toRadians(float angle)
{
	// if (angle >= 180)
 //  		angle-= 360;
	//return (M_PI * angle) / 180;
	return angle;
}

std::map<int, std::vector<glm::vec3>> &MapBuilder::getPlayer(PlayerVert &player)
{
	parseMap(player);

	for (int i = 0; i < finMap.size(); i++)
	{
		for (int j = 0; j <finMap[i].size(); j++)
		{
			if (finMap[i][j] != 0)
			{
				wallBuilder[finMap[i][j]].push_back({j,i,i+1});

				std::cout << finMap[i][j] << "  " << j << "|" << i <<"|\n";
			}
		}

	}

	return wallBuilder;
}

float MapBuilder::getRlength(PlayerVert &player, float &currentAngle)
{
	glm::vec2 rays;
	float tmp = 1.0;

	for (float r = 0; r <= 30.0f; r += 0.03f) 
	{
		rays = {player.playerPos.x + (r * cos(toRadians(currentAngle))), player.playerPos.y + (r * sin(toRadians(currentAngle)))};

		if (finMap[(int)rays.y][(int)rays.x] != 0)
		{
			return r;
		}
	}
	return 0;
}


glm::vec3 MapBuilder::getLastRay(float &currentAngle, PlayerVert &player)
{
	Vertex pos;
	glm::vec2 rays;
	float tmp = 0;
	//tmp = currentAngle * ((float)M_PI/(float)180);
	for (float r = 0; r <= 30.0f; r += 0.03f) 
	{
		rays = {player.playerPos.x + (r * cos(toRadians(currentAngle))), player.playerPos.y + (r * sin(toRadians(currentAngle)))};

		if (finMap[(int)rays.y][(int)rays.x] != 0)
		{
			numWall = finMap[(int)rays.y][(int)rays.x] ;
			//std::cout << "LAST NUM====" << numWall << '\n';

			int x = (int)rays.x;
			int y = (int)rays.y;

			return {x,y,y+1};
		}
	}
	return {0,0,0};
}




int MapBuilder::checkSide(int x, int y)
{
	if (y + 1 < finMap.size()-1 && finMap[y+1][x] == 0 )
	{
		return 1;
	}
	 if ((y - 1 > 0) && finMap[y-1][x] == 0 )
	{
		return 1;
	}
	if (x + 1 <  finMap[y].size()-1 && finMap[y][x +1] == 0)
	{
		return 2;
	}
	else if ( ( x - 1 > 0 ) && finMap[y][x -1] == 0 )
		return 2;
	return 0;
}

bool MapBuilder::angleProc(float &currentAngle, float maxAngle, glm::vec3 &rect, PlayerVert &player)
{
	currentAngle = atan2((float)(rect.y - player.playerPos.y  ),(float)(rect.x - player.playerPos.x)) ;
	
	// if (currentAngle > M_PI/2)
	// 	currentAngle = 5 * (M_PI/2) - currentAngle;
	// else
	// 	currentAngle = M_PI/2 - currentAngle;
	std::cout << numWall << "  X == " << (float)(rect.x - player.playerPos.x) << " Y ===  " << (float)(rect.y - player.playerPos.y  ) << " ANGLE:  " << currentAngle <<" MAX ANGLE:  " << maxAngle <<   "\n";


	if (currentAngle < 0 &&  (int)maxAngle != 0 )
	{
		float tmp = currentAngle;
		currentAngle += 2*M_PI;

		// if (maxAngle > currentAngle)
		// 	maxAngle -=  2*M_PI;
	// 	if ((currentAngle >= 6.0 && (int)maxAngle == 0 ))
	// 	{
	// 		currentAngle  -= 2*M_PI;
	// std::cout << numWall << "  CURRENT ANGLE 1212 " << currentAngle << " tmp ANGLE:  " << tmp <<" MAX ANGLE:  " << maxAngle <<  "\n";

	// 		maxAngle += 2*M_PI;
	// 	}
		// if ( ( maxAngle >= 6.0) && currentAngle >= 6.0 )
		// {
		// 	currentAngle  -= 2*M_PI;
		// // maxAngle+= 2*M_PI
		// }

	}
	else if (currentAngle < M_PI/2 &&  maxAngle+0.5 >= 6.0 || (currentAngle < 0.5 &&  maxAngle >= 6.0))
		currentAngle += 2*M_PI;





	// if ((rect.y -player.playerPos.y) < 0  && (rect.x - player.playerPos.x) < 0)
	// {
	// 	currentAngle= currentAngle - M_PI ;
		
	// 	std::cout << numWall << "  CURRENT ANGLE 111 " << currentAngle << " MAX ANGLE:  " << maxAngle << "\n";

	// }
	// else if ((rect.y -player.playerPos.y)  >= 0 && (rect.x - player.playerPos.x) < 0)
	// {
	// 	currentAngle= currentAngle + M_PI;

	// 	std::cout << numWall << "  CURRENT ANGLE 222 " << currentAngle << " MAX ANGLE:  " << maxAngle << "\n";

	// }
	// else if ((rect.y -player.playerPos.y)  > 0 && (rect.x - player.playerPos.x) == 0)
	// 	currentAngle= M_PI/2.0;
	// else if ((rect.y -player.playerPos.y) < 0 && (rect.x - player.playerPos.x) == 0)
	// 	currentAngle= M_PI/-2.0;



	// if (currentAngle < maxAngle  && maxAngle + 1 > M_PI || (currentAngle < maxAngle && maxAngle > 0))
	// {
	// 	int tmp = numWall;

	// 	rect = getLastRay(currentAngle, player);

	// 	if (tmp == numWall)
	// 	{

	// 		currentAngle = maxAngle;

	// 		rect = getLastRay(currentAngle, player);
	// 		std::cout << numWall << "  CURRENT ANGLE 555 " << currentAngle << " MAX ANGLE:  " << maxAngle << "\n";

	// 	}

	// }

	// if (maxAngle > 2*M_PI)
	// 	maxAngle = 0;


	if (currentAngle > maxAngle )
	{
		std::cout << numWall << "  CURRENT ANGLE 333 " << currentAngle << " MAX ANGLE:  " << maxAngle << "\n";

		currentAngle = maxAngle ;
		rect = getLastRay(currentAngle, player);

	}
	return true;
}


void MapBuilder::xSideProc(glm::vec2 rayInfo, float rayLength, float &currentAngle, float maxAngle, PlayerVert &player)
{
	finWall++;
	Walls tmp;
	tmpNext = 0;

	tmp.rectBegin = {rayInfo.x,rayInfo.y,rayInfo.y};
	tmp.rayLength = rayLength;
	tmp.minAngle = currentAngle;
	tmp.minAngleWall = (currentAngle-alpha); 

	if (numWall == 4)
		tmp.rectEnd = {wallBuilder[numWall][0].x, wallBuilder[numWall][0].y, wallBuilder[numWall][0].z};

	// float checkA = atan2((float)(tmp.rectEnd.y - player.playerPos.y  ),(float)(tmp.rectEnd.x - player.playerPos.x));

	// // if ((tmp.rectEnd.y -player.playerPos.y) < 0  && (tmp.rectEnd.x - player.playerPos.x) < 0)
	// // {
	// // 	checkA= checkA -M_PI ;
	// // 	//std::cout << numWall << "  CURANGLE 111 " << currentAngle << "   " << maxAngle << "\n";

	// // }
	// if (checkA > 0 && checkA < toRadians(maxAngle))
	// {	
	// 	//std::cout << "!!!!!!!!!!!!\n";
	if (numWall == 1)
		tmp.rectEnd = {wallBuilder[numWall][wallBuilder[numWall].size()-1].x, wallBuilder[numWall][wallBuilder[numWall].size()-1].y, wallBuilder[numWall][wallBuilder[numWall].size()-1].z};
	// }

	if (tmp.rectEnd.y == 0)
		tmp.rectEnd.y++;
	if (tmp.rectBegin.y == 0)
		tmp.rectBegin.y++;


	if (tmp.rectEnd.x == 0)
		tmp.rectEnd.x++;
	if (tmp.rectBegin.x == 0)
		tmp.rectBegin.x++;
	angleProc(currentAngle, maxAngle, tmp.rectEnd, player);
	tmp.rayLengthEnd = getRlength(player, currentAngle);
	tmp.maxAngle = currentAngle;
	tmp.maxAngleWall = currentAngle-alpha;

	// if (currentAngle != maxAngle)
	//	tmpNext = 1;

	wallFin[finWall].push_back(tmp);
}

void MapBuilder::ySideProc(glm::vec2 rayInfo,float rayLength, float &currentAngle, float maxAngle,PlayerVert &player)
{
	finWall++;

	Walls tmp;
	tmpNext = 0;
	tmp.rectBegin = {rayInfo.x,rayInfo.y,rayInfo.y};
	tmp.rayLength = rayLength;
	tmp.minAngle = currentAngle; 
	tmp.minAngleWall = (currentAngle-alpha); 
	if (numWall == 3)
		tmp.rectEnd = {wallBuilder[numWall][wallBuilder[numWall].size()-1].x, wallBuilder[numWall][wallBuilder[numWall].size()-1].y+1, wallBuilder[numWall][wallBuilder[numWall].size()-1].z};

	// float checkA = atan2((float)(tmp.rectEnd.y - player.playerPos.y  ),(float)(tmp.rectEnd.x - player.playerPos.x));

	// if (checkA < 0 && checkA < toRadians(maxAngle))
	// {
	if (numWall == 2)
		tmp.rectEnd = {wallBuilder[numWall][0].x, wallBuilder[numWall][0].y, wallBuilder[numWall][0].z};
	// }

	std::cout << "HHHHHHHHHHHHH" << '\n';


	if (tmp.rectEnd.y == 0)
		tmp.rectEnd.y++;
	if (tmp.rectBegin.y == 0)
		tmp.rectBegin.y++;

	if (tmp.rectEnd.x == 0)
		tmp.rectEnd.x++;
	if (tmp.rectBegin.x == 0)
		tmp.rectBegin.x++;
	angleProc(currentAngle, maxAngle, tmp.rectEnd, player);
	
	tmp.rayLengthEnd = getRlength(player, currentAngle);
	tmp.maxAngle = currentAngle;
	tmp.maxAngleWall = currentAngle-alpha;

	// if (currentAngle != maxAngle)
	// 	tmpNext = 1;
	wallFin[finWall].push_back(tmp);
}

void MapBuilder::rayProc(float &currentAngle, float maxAngle, PlayerVert &player)
{
	Vertex pos;
	glm::vec2 rays;
	int x;
	int y;

	for (float r = 0; r <= 30.0f; r += 0.03f) 
	{
		rays = {player.playerPos.x + (r * cos(currentAngle)), player.playerPos.y + (r * sin(currentAngle))};

		if (finMap[(int)rays.y][(int)rays.x] != 0)
		{
			// if (tmpNext == 0)
			// {
				numWall = finMap[(int)rays.y][(int)rays.x];
				x = (int)rays.x;
				y = (int)rays.y;
			//}

			// if (tmpNext != 0 )
			// {
			// 	glm::vec3 rays2;

			// 	if (side == 1)
			// 	{
			// 		rays2 = {wallBuilder[numWall][0].x+1, wallBuilder[numWall][0].y, wallBuilder[numWall][0].z};

			// 		float checkA = atan((float)(rays2.y - player.playerPos.y  )/(float)(rays2.x - player.playerPos.x));

			// 		if (checkA > 0 && checkA < maxAngle)
			// 		{	
			// 			rays2 = {wallBuilder[numWall][wallBuilder[numWall].size()-1].x+1, wallBuilder[numWall][wallBuilder[numWall].size()-1].y+1, wallBuilder[numWall][wallBuilder[numWall].size()-1].z};
			// 			//numWall = finMap[(int)rays2.y + 1][(int)rays2.x];
					
			// 		}
			// 		// else
			// 		// 	numWall = finMap[(int)rays2.y - 1][(int)rays2.x];

			// 		std::cout << "LLLOOOOO\n";
			// 		ySideProc({rays2.x, rays2.y}, r, currentAngle, maxAngle, player);
			// 	}
			// 	if (side == 2)
			// 	{
			// 		rays2 = {wallBuilder[numWall][wallBuilder[numWall].size()-1].x, wallBuilder[numWall][wallBuilder[numWall].size()-1].y+1, wallBuilder[numWall][wallBuilder[numWall].size()-1].z};

			// 		float checkA = atan((float)(rays2.y - player.playerPos.y  )/(float)(rays2.x - player.playerPos.x));

			// 		if (checkA < 0 && checkA < maxAngle)
			// 		{
			// 			rays2 = {wallBuilder[numWall][0].x, wallBuilder[numWall][0].y, wallBuilder[numWall][0].z};
			// 			//numWall = finMap[(int)rays2.y + 1][(int)rays2.x + 1];

			// 		}
			// 		// else
			// 		// 	numWall = finMap[(int)rays2.y + 1][(int)rays2.x - 1];

			// 		xSideProc({rays2.x, rays2.y}, r, currentAngle, maxAngle, player);

			// 	}

			// 	tmpNext = 0;
			// 	return;
			// }


			side = checkSide(x, y);



			// else if (tmpNextSec != 0 )
			// {
			// 	numWall = tmpNextSec;
			// 	tmpNextSec = 0;
			// 	side = 2;
			// 	ySideProc({wallBuilder[numWall][0].x,wallBuilder[numWall][0].y}, r, currentAngle, maxAngle, player);
			// }
			// else
			// {

				if (side == 1)
				{
					xSideProc({x,y}, r, currentAngle, maxAngle, player);
				}
				if (side == 2)
				{
					ySideProc({x,y}, r, currentAngle, maxAngle, player);
				}

			//}
			return ;

		}


	}
}



std::map<int,  std::vector<Walls>> &MapBuilder::mapBuilder(PlayerVert &player, float &newAlpha)
{
	alpha = newAlpha;
	// alpha = alpha * 180/M_PI;

	// if (alpha < 0 )
	// 	alpha += 360;

	tmpNext = 0;
	tmpNextSec = 0;
	float minAngle = alpha - (fov/(float)2);
	float maxAngle = alpha + (fov/(float)2);

	// if ((int)maxAngle == 0)
	// 	maxAngle == 2*M_PI;
	// if (maxAngle > M_PI/2)
	// 	maxAngle = 5 * (M_PI/2) - maxAngle;
	// else
	// 	maxAngle = M_PI/2 - maxAngle;
	// if (maxAngle > 2* M_PI)
	// 	maxAngle -= 2*M_PI;

	// if (minAngle < 0 )
	// 	minAngle += 360;
	// if (maxAngle < 0 )
	// 	maxAngle += 360;

	if (maxAngle > 2*M_PI)
	{
		maxAngle -= 2*M_PI;
		minAngle -= 2*M_PI;
	}
	// if (maxAngle <= 0)
	// {
		
	// }

	float x =  (int)(maxAngle * 100 );
	x /= (float)100;
	if (x == 0)
	{
		maxAngle += 2*M_PI;
		minAngle += 2*M_PI;
	}
	std::cout << "+++++++++++++++++" << alpha << "maxangle e ee e "<< x  <<  "==== "<< minAngle<< "\n";
	//maxAngle = (( maxAngle + 360 ) * 360 ) / 360;
	while (minAngle < maxAngle)
	{
		rayProc(minAngle, maxAngle, player);
		minAngle += 0.01;
	}
	return wallFin;
}



	



// #include "..\includes\MapBuilder.h"

// bool MapBuilder::checkColums(int i, int j)
// {
// 	if ((i + 1) < strMap.size() && (strMap[i+1][j] == '1'))
// 	{
// 		return true;
// 	}
// 	else if ((i - 1) > 0 && (strMap[i -1][j] == '1' ))
// 	{
// 		return true;
// 	}
// 	return false;
// }

// bool MapBuilder::checkRows(int i, int j)
// {
// 	if ((j + 1) < strMap[i].length() && (strMap[i][j+1] == '1' || strMap[i][j+1] == ','))
// 	{
// 		return true;
// 	}
// 	else if ((j - 1) > 0 && (strMap[i][j-1] == '1'|| strMap[i][j-1] == ','))
// 	{
// 		return true;
// 	}
// 	return false;
// }

// void MapBuilder::parseMap(PlayerVert &player)
// {
// 	int i = 0;
// 	int checker = 1;

// 	while (i < strMap.size())
// 	{
// 		for (int j = 0; strMap[i][j]; j++)
// 		{

// 			if (strMap[i][j] == 'p')
// 			{
// 				player.playerPos = {j,i};
// 				player.oldPos = {j,i};
// 				strMap[i][j] = '0';
// 			}
// 			if (strMap[i][j] == '0')
// 			{
// 				finMap[i].push_back(0);
// 				strMap[i][j] = '0';
// 			}
// 			if (strMap[i][j] == '1')
// 			{
// 				if (checkRows(i,j))
// 				{
// 					for (; strMap[i][j]; j++)
// 					{
// 						if (j != 0 && j != strMap[i].length()-1 && checkColums(i, j))
// 						{
// 							if ( strMap[i][j+1] != '0')
// 								checker++;
// 						}
// 						if (strMap[i][j] != '1')
// 							break;
// 						finMap[i].push_back(checker);
// 						strMap[i][j] = ',';
// 					}
// 					j = 0;
// 					checker++;
// 				}
// 				else
// 				{
// 					int tmp;

// 					if (checkRows(i-1,j))
// 					{
// 						finMap[i].push_back(checker);
// 						strMap[i][j] = '.';	
// 						checker++;
// 					}
// 					else
// 					{
// 						std::vector<int> vec = finMap[i-1];
// 						tmp = vec[j];
// 						if (tmp == 0)
// 						{
// 							tmp = checker;
// 							checker++;
// 						}
// 						finMap[i].push_back(tmp);
// 						strMap[i][j] = '.';		
// 					}
								
// 				}

// 			}
// 		}
// 		i++;
// 	}

// 		for (auto &x : finMap)
// 	{
// 		std::cout << x.first << ": ";
// 		for (auto k : x.second)
// 		{
// 			std::cout << " |" << k << "| "; 
// 		}
// 		std::cout << '\n';

// 	}
// }

// std::map<int, std::vector<glm::vec3>> &MapBuilder::getPlayer(PlayerVert &player)
// {
// 	parseMap(player);

// 	for (int i = 0; i < finMap.size(); i++)
// 	{
// 		for (int j = 0; j <finMap[i].size(); j++)
// 		{
// 			if (finMap[i][j] != 0)
// 			{
// 				wallBuilder[finMap[i][j]].push_back({j,i,i+1});

// 				std::cout << finMap[i][j] << "  " << j << "|" << i <<"|\n";
// 			}
// 		}

// 	}

// 	return wallBuilder;
// }

// float MapBuilder::getRlength(PlayerVert &player, float &currentAngle)
// {
// 	glm::vec2 rays;
// 	float tmp = 1.0;

// 	for (float r = 0; r <= 30.0f; r += 0.03f) 
// 	{
// 		rays = {player.playerPos.x + (r * cos(currentAngle)), player.playerPos.y + (r * sin(currentAngle))};

// 		if (finMap[(int)rays.y][(int)rays.x] != 0)
// 		{
// 			return r;
// 		}
// 	}
// 	return 0;
// }

// glm::vec3 MapBuilder::getLastRay(float &currentAngle, PlayerVert &player)
// {
// 	Vertex pos;
// 	glm::vec2 rays;
// 	float tmp = 1.0;

// 	for (float r = 0; r <= 30.0f; r += 0.03f) 
// 	{
// 		rays = {player.playerPos.x + (r * cos(currentAngle)), player.playerPos.y + (r * sin(currentAngle))};

// 		if (finMap[(int)rays.y][(int)rays.x] != 0)
// 		{
// 			numWall = finMap[(int)rays.y][(int)rays.x] ;
// 			std::cout << "LAST NUM====" << numWall << '\n';

// 			int x = (int)rays.x;
// 			int y = (int)rays.y;

// 			return {x,y,y+1};
// 		}
// 	}
// 	return {0,0,0};
// }


// int MapBuilder::checkSide(int x, int y)
// {
// 	if (y + 1 < finMap.size()-1 && finMap[y+1][x] == 0 )
// 	{
// 		return 1;
// 	}
// 	 if ((y - 1 > 0) && finMap[y-1][x] == 0 )
// 	{
// 		return 1;
// 	}
// 	if (x + 1 <  finMap[y].size()-1 && finMap[y][x +1] == 0)
// 	{
// 		return 2;
// 	}
// 	else if ( ( x - 1 > 0 ) && finMap[y][x -1] == 0 )
// 		return 2;
// 	return 0;
// }

// bool MapBuilder::angleProc(float &currentAngle, float maxAngle, glm::vec3 &rect, PlayerVert &player)
// {
// 	currentAngle = atan((float)(rect.y - player.playerPos.y  )/(float)(rect.x - player.playerPos.x));

// 	if ((rect.y -player.playerPos.y) < 0  && (rect.x - player.playerPos.x) < 0)
// 	{
// 		currentAngle= currentAngle -M_PI ;

// 		std::cout << numWall << "  CURANGLE 111 " << currentAngle << "   " << maxAngle << "\n";

// 	}
// 	else if ((rect.y -player.playerPos.y)  > 0 && (rect.x - player.playerPos.x) < 0)
// 	{
// 		currentAngle= currentAngle + M_PI;
// 		std::cout << numWall << "  CURANGLE 222 " << currentAngle << "   " << maxAngle << "\n";

// 	}
// 	// else if (currentAngle < 0)
// 	// {

// 	// }

// 	if (currentAngle < maxAngle  && maxAngle + 1 > M_PI
// 	|| (currentAngle < maxAngle && maxAngle > 0))
// 	{
// 		int tmp = numWall;
// 		rect = getLastRay(maxAngle, player);
// 		if (tmp == numWall)
// 		{
// 			currentAngle = maxAngle;

// 			rect = getLastRay(currentAngle, player);
// 		std::cout << numWall << "  CURANGLE 555 " << currentAngle << "   " << maxAngle << "\n";

// 		}
// 		else
// 		{
// 			//tmpNext = numWall;
// 			if (side == 2)
// 			{
// 				rect = {wallBuilder[tmp][wallBuilder[tmp].size()-1].x, wallBuilder[tmp][wallBuilder[tmp].size()-1].y+1, wallBuilder[tmp][wallBuilder[tmp].size()-1].z};

// 				float checkA = atan((float)(rect.y - player.playerPos.y  )/(float)(rect.x - player.playerPos.x));

// 				if (checkA < 0 && checkA < maxAngle)
// 				{
// 					rect = {wallBuilder[tmp][0].x, wallBuilder[tmp][0].y, wallBuilder[tmp][0].z};
// 				}
// 				if (currentAngle < 0)
// 					tmpNext = numWall;

// 		std::cout << numWall << "  CURANGLE 999 " << currentAngle << "   " << maxAngle << "\n";

// 			}
// 			if (side == 1)
// 			{
		
// 				rect = {wallBuilder[tmp][0].x+1, wallBuilder[tmp][0].y, wallBuilder[tmp][0].z};

// 				float checkA = atan((float)(rect.y - player.playerPos.y  )/(float)(rect.x - player.playerPos.x));
// 				std::cout << tmp << " !!!! " << maxAngle << "\n\n";

// 				if ((checkA > 0 && checkA < (maxAngle)))
// 				{

// 					rect = {wallBuilder[tmp][wallBuilder[tmp].size()-1].x+1, wallBuilder[tmp][wallBuilder[tmp].size()-1].y, wallBuilder[tmp][wallBuilder[tmp].size()-1].z};
// 				std::cout << rect.x << "   " << rect.y << "   ====" << currentAngle << '\n';
// 				}
// 				if (currentAngle < 0 && maxAngle < M_PI)
// 				{

// 					int n = finMap[wallBuilder[tmp][wallBuilder[tmp].size()-1].y+2][wallBuilder[tmp][wallBuilder[tmp].size()-1].x];
// 					tmpNextSec = n;
// 					std::cout << n << "  CURANGLE 121212 " << currentAngle << "   " << maxAngle << "\n";

// 				}
// 			}


// 		numWall = tmp;

// 		}
// 	}

// 	if (currentAngle > maxAngle)
// 	{
// 		std::cout << numWall << "  CURANGLE 333 " << currentAngle << "   " << maxAngle << "\n";

// 		currentAngle = maxAngle;
// 		rect = getLastRay(currentAngle, player);

// 	}
// 	return true;
// }


// void MapBuilder::xSideProc(glm::vec2 rayInfo, float rayLength, float &currentAngle, float maxAngle, PlayerVert &player)
// {
// 	finWall++;
// 	Walls tmp;

// 	tmp.rectBegin = {rayInfo.x,rayInfo.y,rayInfo.y};
// 	tmp.rayLength = rayLength;
// 	tmp.minAngle = currentAngle;
// 	tmp.minAngleWall = currentAngle-alpha; 

// 	tmp.rectEnd = {wallBuilder[numWall][0].x+1, wallBuilder[numWall][0].y, wallBuilder[numWall][0].z};

// 	float checkA = atan((float)(tmp.rectEnd.y - player.playerPos.y  )/(float)(tmp.rectEnd.x - player.playerPos.x));

// 	if (checkA > 0 && checkA < maxAngle)
// 	{	
// 		//tmp.rectBegin = {rayInfo.x,rayInfo.y+1,rayInfo.y};

// 		tmp.rectEnd = {wallBuilder[numWall][wallBuilder[numWall].size()-1].x+1, wallBuilder[numWall][wallBuilder[numWall].size()-1].y+1, wallBuilder[numWall][wallBuilder[numWall].size()-1].z};
// 	}

// 	angleProc(currentAngle, maxAngle, tmp.rectEnd, player);
// 	tmp.rayLengthEnd = getRlength(player, currentAngle);
// 	tmp.maxAngle = currentAngle;
// 	tmp.maxAngleWall = currentAngle-alpha;

// 	wallFin[finWall].push_back(tmp);
// }

// void MapBuilder::ySideProc(glm::vec2 rayInfo,float rayLength, float &currentAngle, float maxAngle,PlayerVert &player)
// {
// 	finWall++;

// 	Walls tmp;

// 	tmp.rectBegin = {rayInfo.x,rayInfo.y,rayInfo.y};
// 	tmp.rayLength = rayLength;
// 	tmp.minAngle = currentAngle; 
// 	tmp.minAngleWall = currentAngle-alpha; 

// 	tmp.rectEnd = {wallBuilder[numWall][wallBuilder[numWall].size()-1].x, wallBuilder[numWall][wallBuilder[numWall].size()-1].y+1, wallBuilder[numWall][wallBuilder[numWall].size()-1].z};

// 	float checkA = atan((float)(tmp.rectEnd.y - player.playerPos.y  )/(float)(tmp.rectEnd.x - player.playerPos.x));

// 	if (checkA < 0 && checkA < maxAngle)
// 	{
// 		tmp.rectEnd = {wallBuilder[numWall][0].x, wallBuilder[numWall][0].y, wallBuilder[numWall][0].z};
// 	}

// 				std::cout << tmp.rectEnd.x<< " ||||  " << tmp.rectEnd.y << "   ====" << currentAngle << '\n';

// 	angleProc(currentAngle, maxAngle, tmp.rectEnd, player);
	
// 	tmp.rayLengthEnd = getRlength(player, currentAngle);
// 	tmp.maxAngle = currentAngle;
// 	tmp.maxAngleWall = currentAngle-alpha;

// 	wallFin[finWall].push_back(tmp);
// }

// void MapBuilder::rayProc(float &currentAngle, float maxAngle, PlayerVert &player)
// {
// 	Vertex pos;
// 	glm::vec2 rays;

// 	for (float r = 0; r <= 30.0f; r += 0.03f) 
// 	{
// 		rays = {player.playerPos.x + (r * cos(currentAngle)), player.playerPos.y + (r * sin(currentAngle))};

// 		if (finMap[(int)rays.y][(int)rays.x] != 0)
// 		{

// 			numWall = finMap[(int)rays.y][(int)rays.x];
// 			int x = (int)rays.x;
// 			int y = (int)rays.y;

// 			side = checkSide(x, y);


// 			if (tmpNext != 0 )
// 			{
// 				numWall = tmpNext;
// 				tmpNext = 0;
// 				side = 1;
// 				xSideProc({wallBuilder[numWall][0].x+1,wallBuilder[numWall][0].y}, r, currentAngle, maxAngle, player);
// 			}

// 			else if (tmpNextSec != 0 )
// 			{
// 				numWall = tmpNextSec;
// 				tmpNextSec = 0;
// 				side = 2;
// 				ySideProc({wallBuilder[numWall][0].x,wallBuilder[numWall][0].y}, r, currentAngle, maxAngle, player);
// 			}
// 			else
// 			{

// 				if (side == 1)
// 				{
// 					xSideProc({x,y}, r, currentAngle, maxAngle, player);
// 				}
// 				if (side == 2)
// 				{
// 					ySideProc({x,y}, r, currentAngle, maxAngle, player);
// 				}

// 			}
// 			return ;

// 		}


// 	}
// }



// std::map<int,  std::vector<Walls>> &MapBuilder::mapBuilder(PlayerVert &player, float &newAlpha)
// {
// 	alpha = newAlpha;
// 	tmpNext = 0;
// 	tmpNextSec = 0;
// 	float minAngle = alpha - (fov/(float)2);
// 	float maxAngle = alpha + (fov/(float)2);


// 	while (minAngle < maxAngle)
// 	{
// 		rayProc(minAngle, maxAngle, player);
// 		minAngle +=  0.01;
// 		// if ((minAngle < maxAngle))
// 		// rayProc(minAngle, maxAngle, player);

// 	}
// 	return wallFin;
// }