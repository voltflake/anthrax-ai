#include "..\includes\VulkanMain.h"

bool mEngine::checkCollision()
{
	int l = 0;
	float plX =  (int)(player.playerPos.x * 10 );
	plX /= (float)10;

	float plY = (int)(player.playerPos.y * 10 );
	plY /= (float)10;
	while (l <= wallFin.size())
	{
		std::vector<Walls> tmp = wallFin[l];
		for (int j = 0; j < tmp.size(); j++)
		{
			if (tmp[j].rectBegin.x > tmp[j].rectEnd.x && tmp[j].rectBegin.y > tmp[j].rectEnd.y)
			{
				if (plX <= tmp[j].rectBegin.x && plX >=tmp[j].rectEnd.x
				&& plY <= tmp[j].rectBegin.y && plY >=tmp[j].rectEnd.y )
					return false;
			}
			else if (tmp[j].rectBegin.x < tmp[j].rectEnd.x && tmp[j].rectBegin.y < tmp[j].rectEnd.y)
			{
				if (plX <= tmp[j].rectEnd.x && plX >= tmp[j].rectBegin.x
				&& plY <= tmp[j].rectEnd.y && plY >= tmp[j].rectBegin.y )
					return false;
			}
		}
		// if ((fabs((wall1.boards[l].x) - plX) < 0.005f )
		// && fabs((wall1.boards[l].y + 0.1) -  plY ) < 0.005f)
		// {
		// 	return false;
		// }
		// if ((fabs((wall1.boards[l].x ) -plX) < 0.005f )
		// && fabs((wall1.boards[l].y) -  plY ) < 0.005f)
		// {
		// 	return false;
		// }
		// if ((fabs((wall1.boards[l].x + 0.1) - plX) < 0.005f )
		// && fabs((wall1.boards[l].y) -  plY ) < 0.005f)
		// {
		// 	return false;
		// }
		// if ((fabs((wall1.boards[l].x - 0.1) - plX) < 0.005f )
		// && fabs((wall1.boards[l].y) -  plY ) < 0.005f)
		// {
		// 	return false;
		// }

		l++;
	}
		
	return true;
}

void mEngine::keyHandle()
{
	if (GetAsyncKeyState(S_KEY) < 0)
    {
    		player.oldPos.y = player.playerPos.y;
    		player.playerPos.y -= 0.05;
    // 		 x = -1.0f;
			 // y = -1.0f;
    		// if (!checkCollision())
    		// {
    		// 	player.playerPos = {player.oldPos.x, player.oldPos.y};
    		// 	return ;
    		// }
			
			 nFrame = true;
    }
   	if (GetAsyncKeyState(W_KEY) < 0)
    {
    	   player.oldPos.y = player.playerPos.y;

	    	player.playerPos.y += 0.05;
	   //  	 x = -1.0f;
			 // y = -1.0f;
	    	// if (!checkCollision())
    		// {
    		// 	player.playerPos = {player.oldPos.x, player.oldPos.y};

    		// 	return ;
    		// }
			
			 nFrame = true;
    }
    if (GetAsyncKeyState(A_KEY) < 0)
   	{
    	   player.oldPos.x = player.playerPos.x;

	    	player.playerPos.x -= 0.05;
	   //  	 x = -1.0f;
			 // y = -1.0f;
	    	// if (!checkCollision())
    		// {
    		// 	player.playerPos = {player.oldPos.x, player.oldPos.y};

    		// 	return ;
    		// }
			 nFrame = true;
    }
     if (GetAsyncKeyState(D_KEY) < 0)
   	{
    	      	      		player.oldPos.x = player.playerPos.x;


	    	player.playerPos.x += 0.05;
	   //  	x = -1.0f;
			 // y = -1.0f;
	    	// if (!checkCollision())
    		// {
    		// 	player.playerPos = {player.oldPos.x, player.oldPos.y};

    		// 	return ;
    		// }
			 nFrame = true;
    }
    if (GetAsyncKeyState(VK_LEFT) < 0)
    {
    	alpha += (float)M_PI/(float)180;
    	if (alpha > (2*M_PI))
	 	{
	 		//alpha -= 2*M_PI;
	 		alpha = 0;

	 	}
		 nFrame = true;
    }
       if (GetAsyncKeyState(VK_RIGHT) < 0)
    {
    //	if (alpha > 0)
    	alpha -= (float)M_PI/(float)180;

    	if (alpha < 0)
	 	{
	 		alpha = 2*M_PI;
	 	}
		 nFrame = true;
    }
}