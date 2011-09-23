/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////


#include "icf2\sched.hh"
#include "icf2\notify.hh"
#include "gwr\displayTask.h"

void
LoadBitmapFile(char * file_name,BITMAPINFOHEADER **info,unsigned char **buffer)
{
	// Retrieve a handle identifying the file.
	HANDLE hfbm = CreateFile(file_name, GENERIC_READ, 
		FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 
		(HANDLE)NULL);
	if (hfbm == INVALID_HANDLE_VALUE)
	{
		return;
	}

	// Retrieve the BITMAPFILEHEADER structure.
	DWORD dwRead;
	BITMAPFILEHEADER bmfh;
	if (!ReadFile(hfbm, &bmfh, sizeof(BITMAPFILEHEADER), &dwRead, (LPOVERLAPPED)NULL))
	{
		CloseHandle(hfbm);
		return;
	}

	// Retrieve the BITMAPINFOHEADER structure.
	BITMAPINFOHEADER * bmih = new BITMAPINFOHEADER;
	if (!ReadFile(hfbm, bmih, sizeof(BITMAPINFOHEADER),
		&dwRead, (LPOVERLAPPED)NULL))
	{
		CloseHandle(hfbm);
		return;
	}
	// Allocate memory for the required number of bytes. 
	unsigned char * image = new unsigned char[bmfh.bfSize-bmfh.bfOffBits]; 
	ReadFile(hfbm, image, (bmfh.bfSize - bmfh.bfOffBits),&dwRead, (LPOVERLAPPED)NULL);
	*info = bmih;
	*buffer = image;
}
int WINAPI 
WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow) 
{ 
	setNotifyFile("gwr.log");

	sched_t app(128);
	displayTask_ref dt = createDisplayTask("GDI",(char*)&hinstance);
	app << (task_ref)((abstractTask_t *)dt);

	
		unsigned int visualDepth;
		int visualClass;
		dt->getDefaultVisualCombination(&visualDepth,&visualClass);
		window_ref window1 = dt->createWindow("PRUEBA1","850x850+0+0",NULL,visualDepth,visualClass,640,480);
		window1->setBgColor(255,255,0);
		window1->map();

		window1->drawLineInBG(10,10,300,300,5);
		window1->setGeometry("802x810+100+100");
		window1->setFgColor("Green");
		window1->loadFont("Times New Romans",20,0);
		
		//unsigned char * data = new unsigned char[w*h*3];
		//memset(data,0,w*h*3);

		char msg[1024];
		BITMAPINFOHEADER * info;
		unsigned char * data = NULL;
		LoadBitmapFile("c:\\prueba.bmp",&info,&data);
		dt->rootWindow()->putRGB24inBG(data,info->biWidth,info->biHeight,6.0,6.0);
		char r = 0;
		char g = 0;
		char b = 0;
		float zx = 3.1;
		float zy = 3.1;
		float gr = 1.0;
		float gg = 1.0;
		float gb = 1.0;
		int val = 0;
		while(true)
		{
			/*for (int y = 0;y<h;y++)
			{
				if (y<h/3)
				{
					val = 50;
				}else if (y<h*2/3){
					val = 150;
				}else{
					val = 255;
				}
				for(int x=0;x<w*3;x+=3)
				{
					if (x<w)
					{
						data[w*y*3 + x] = 0;
						data[w*y*3 + x + 1] = val+r;
						data[w*y*3 + x + 2] = 0;
					}else if (x<w*2){
						data[w*y*3 + x] = val+r;
						data[w*y*3 + x + 1] = 0;
						data[w*y*3 + x + 2] = 0;
					}else{
						data[w*y*3 + x] = 0;
						data[w*y*3 + x + 1] = 0;
						data[w*y*3 + x + 2] = val+r;
					}
				}				
				r++;
			}*/
			int wh = window1->getHeight();
			int ww = window1->getWidth();
			float zoomx = (float)ww/(float)info->biWidth;
			float zoomy = (float)wh/(float)info->biHeight;
			window1->putRGB24(data,info->biWidth,info->biHeight,zoomx,zoomy);
			sprintf_s(msg,"APLICANDO GAMMA (%f)",gr);
			window1->drawString(2,20,msg,align_e::CENTER);
			window1->setGamma(gr,gg,gb);
			gr += 0.1;gg+=0.1;gb+=0.1;
			if (gr>4.0) gr = 0.2;
			if (gg>4.0) gg = 0.2;
			if (gb>4.0) gb = 0.2;
			/*zx += 0.1;zy+=0.1;
			if (zx>5.0) zx = 0.1;
			if (zy>5.0) zy = 0.1;*/
		}
	return 0;
}