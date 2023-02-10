#include"texture.h"
#include<algorithm>
void makeTexture(GLubyte image[64][64][3])
{
	{
		for (int i = 0; i < 64; i++)
		{
			for (int j = 0; j < 64; j++)
			{
				GLubyte c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
				
				image[i][j][0] = c;
				image[i][j][1] = c;
				image[i][j][2] = c;
			}
		}
	}
	
}