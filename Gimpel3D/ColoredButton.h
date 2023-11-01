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
#ifndef COLORED_BUTTON_H
#define COLORED_BUTTON_H

#include "stdafx.h"
#include "SkinControlEdge.h"

class CColoredButton : public CButton
{
public:
	CColoredButton()
	{
	}
	~CColoredButton()
	{
	}
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
	{
	   UINT uStyle = DFCS_BUTTONPUSH;

	   // This code only works with buttons.
	   ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);


			RECT rectOutside;
			RECT rectInside;		

			CDC *pDC = this->GetDC();

			HDC hDC = lpDrawItemStruct->hDC;

			GetWindowRect(&rectOutside);
			ScreenToClient(&rectOutside);

			int border = 2;

			rectInside.left = rectOutside.left + border;
			rectInside.right = rectOutside.right - border;
			rectInside.top = rectOutside.top + border;
			rectInside.bottom = rectOutside.bottom - border;
		
			COLORREF color = RGB(rgb[0],rgb[1],rgb[2]);
			
			if(lpDrawItemStruct->itemState & ODS_DISABLED)
			{
				pDC->FillSolidRect(&rectInside,color); 
				DrawSkinControlEdge(pDC, &rectOutside, true);
			}
			else
			{
				UINT state = GetState();
				if(state & BST_PUSHED)
				{
					pDC->FillSolidRect(&rectInside,color); 
					DrawSkinControlEdge(pDC, &rectOutside, false);
				}
				else
				{
					pDC->FillSolidRect(&rectInside,color); 
					DrawSkinControlEdge(pDC, &rectOutside, true);
				}
			}
	}
	void Set_Color(unsigned char r, unsigned char g, unsigned char b)
	{
		rgb[0] = r;
		rgb[1] = g;
		rgb[2] = b;
	}
	void Get_Color(unsigned char *r, unsigned char *g, unsigned char *b)
	{
		*r = rgb[0];
		*g = rgb[1];
		*b = rgb[2];
	}
	char rgb[3];
};











#endif
