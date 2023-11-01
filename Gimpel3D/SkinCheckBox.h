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
#ifndef SKIN_CHECKBOX_H
#define SKIN_CHECKBOX_H

#include "Skin.h"

#include "SkinControlEdge.h"

class CSkinCheckBox : public CButton
{
public:
	CSkinCheckBox()
	{
		checked = false;
	}
	~CSkinCheckBox()
	{
	}
	bool checked;
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
	{
		   UINT uStyle = DFCS_BUTTONCHECK;			

		   // This code only works with buttons.
		   ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);

		RECT rect = lpDrawItemStruct->rcItem;
		int bsize = 14;

		rect.right = rect.left+bsize;
		rect.bottom = rect.top+bsize;

		if(lpDrawItemStruct->itemState & BST_CHECKED)
		{
			dc.FillSolidRect(&rect, RGB(0,0,0)); 
		}
		else
		{
			dc.FillSolidRect(&rect, skin_checkbox_color); 
		}
		DrawSkinControlEdge(&dc, &rect, false);
		
		rect = lpDrawItemStruct->rcItem;
		rect.left += 18;
		CString strText;
		GetWindowText(strText);

		::DrawText(lpDrawItemStruct->hDC, strText, strText.GetLength(), &rect, 0);

	}
};






#endif