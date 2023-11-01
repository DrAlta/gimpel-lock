/*  Gimpel3D 2D/3D Stereo Converter
    Copyright (C) 2008-2011  Daniel René Dever (Gimpel)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "G3D.h"
#include <vfw.h>
#include "../Skin.h"


#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "Winmm.lib")

//FIXTHIS use static struct to get frames, don't re-allocate everytime

bool Capture_Image(HWND hWnd, char *file)
{
	SetForegroundWindow(hWnd);
    HDC hdcWindow;
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;

    // Retrieve the handle to a display device context for the client 
    // area of the window. 
    hdcWindow = GetDC(hWnd);

    // Create a compatible DC which is used in a BitBlt from the window DC
    hdcMemDC = CreateCompatibleDC(hdcWindow); 

    if(!hdcMemDC)
    {
        SkinMsgBox(0, "Can'r create compatible DC for bitmap!","Failed", MB_OK);
        return false;
    }

    // Get the client area for size calculation
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    
    // Create a compatible bitmap from the Window DC
    hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
    
    if(!hbmScreen)
    {
        SkinMsgBox(0, "CreateCompatibleBitmap Failed","Failed", MB_OK);
        return false;
    }

    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(hdcMemDC,hbmScreen);
    
    // Bit block transfer into our compatible memory DC.
    if(!BitBlt(hdcMemDC, 
               0,0, 
               rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
               hdcWindow, 
               0,0,
               SRCCOPY))
    {
        SkinMsgBox(0, "BitBlt has failed", "Failed", MB_OK);
        return false;
    }

    // Get the BITMAP from the HBITMAP
    GetObject(hbmScreen,sizeof(BITMAP),&bmpScreen);
     
    BITMAPFILEHEADER   bmfHeader;    
    BITMAPINFOHEADER   bi;
     
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = bmpScreen.bmWidth;    
    bi.biHeight = bmpScreen.bmHeight;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;  
    bi.biXPelsPerMeter = 0;    
    bi.biYPelsPerMeter = 0;    
    bi.biClrUsed = 0;    
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
    char *lpbitmap = (char *)GlobalLock(hDIB);    

    // Gets the "bits" from the bitmap and copies them into a buffer 
    // which is pointed to by lpbitmap.
    GetDIBits(hdcWindow, hbmScreen, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    // A file is created, this is where we will save the screen capture.
    HANDLE hFile = CreateFile(file,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);   
    
    // Add the size of the headers to the size of the bitmap to get the total file size
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
 
    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER); 
    
    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB; 
    
    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM   
 
    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
    
    //Unlock and Free the DIB from the heap
    GlobalUnlock(hDIB);    
    GlobalFree(hDIB);

    //Close the handle for the file that was created
    CloseHandle(hFile);
       
    DeleteObject(hbmScreen);
    ReleaseDC(hWnd, hdcMemDC);
    ReleaseDC(hWnd,hdcWindow);

    return true;
}

bool ErrorMB(DWORD id)
{
	char text[512];
    FormatMessage(0, 0, id, 0, text, 512, 0);
	SkinMsgBox(0, text, "Error", MB_OK);
	return true;
}

bool MCIErrorMB(DWORD id)
{
	char text[512];
	mciGetErrorString(id, text, 512);
	SkinMsgBox(0, text, "MCIError", MB_OK);
	return true;
}


bool Extract_Frames(char *file, char *output)
{
	HWND m_Video = MCIWndCreate(0, GetModuleHandle(0), WS_POPUP | WS_VISIBLE|MCIWNDF_NOTIFYPOS|MCIWNDF_NOPLAYBAR, file);
	if(!m_Video)
	{
		ErrorMB(GetLastError());
		return false;
	}
	int e = MCIWndOpen(m_Video, file, 0);
	if(e)
	{
		MCIErrorMB(e);
		MCIWndDestroy(m_Video);
		return false;
	}
	int frame_id = 0;

	UINT length = MCIWndGetLength(m_Video);
	LONG position = MCIWndGetPosition(m_Video);

	LONG frame_diff = 0;

	while(position!=(LONG)length)
	{
		if(position+frame_diff>=(int)length)
		{
			position = length;
			//reached the end
		}
		else
		{
			int se = MCIWndStep(m_Video, 1);
			if(frame_diff==0)
			{
				frame_diff = MCIWndGetPosition(m_Video);
			}
			if(se==0)
			{
				position = MCIWndGetPosition(m_Video);
				char image_file[512];
				sprintf(image_file, "%s\\%.6i.bmp", output, frame_id);
				frame_id++;
				if(!Capture_Image(m_Video, image_file))
				{
					SkinMsgBox(0, "Error capturing frame!", 0, MB_OK);
					MCIWndDestroy(m_Video);
					return false;
				}
				Print_Status("Extracting frame %i", frame_id);
			}
			else
			{
				//error reading last frame!
				position = length;
			}
		}
	}
	MCIWndDestroy(m_Video);
	return true;
}