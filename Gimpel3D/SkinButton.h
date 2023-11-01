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
#ifndef SKIN_BUTTON_H
#define SKIN_BUTTON_H

#include "SkinControlEdge.h"

bool Register_Tooltip_Button(CButton *b);
bool UnRegister_Tooltip_Button(CButton *b);

class CSkinButton : public CButton
{
public:
	bool render_pushed;
	bool render_bmp;
	int bmp_width, bmp_height;
	int bmp_xoff, bmp_yoff;
	int bmp_xstart, bmp_ystart;
	CToolTipCtrl *m_pToolTip;
	CDC *pBMPDC;
	CSkinButton()
	{
		render_pushed = false;
		m_pToolTip = 0;
		pBMPDC = 0;
		render_bmp = false;
		bmp_width = 0;
		bmp_height = 0;
		bmp_xoff = 0;
		bmp_yoff = 0;
		bmp_xstart = 0;
		bmp_ystart = 0;
	}
	~CSkinButton()
	{
		if(m_pToolTip)
		{
			delete m_pToolTip;
			UnRegister_Tooltip_Button(this);
		}
		if(pBMPDC)
		{
			delete pBMPDC;
		}
	}
	bool SetBitmap(CBitmap *bmp, int width, int height, int xoff, int yoff, int xstart, int ystart)
	{
		pBMPDC = new CDC;
		pBMPDC->CreateCompatibleDC(GetDC());
		pBMPDC->SelectObject(bmp);
		render_bmp = true;
		bmp_width = width;
		bmp_height = height;
		bmp_xoff = xoff;
		bmp_yoff = yoff;
		bmp_xstart = xstart;
		bmp_ystart = ystart;
		return true;
	}
	bool SetBitmapInfo(int xoff, int yoff, int xstart, int ystart)
	{
		bmp_xoff = xoff;
		bmp_yoff = yoff;
		bmp_xstart = xstart;
		bmp_ystart = ystart;
		return true;
	}
	bool SetToolTip(const char *text)
	{
		if(!m_pToolTip)
		{
			m_pToolTip = new CToolTipCtrl;
			m_pToolTip->Create(this, TTS_ALWAYSTIP);
			m_pToolTip->Activate(TRUE);
			CRect rect; 
			GetClientRect(rect);
			m_pToolTip->AddTool(this, text, rect, 1);
			Register_Tooltip_Button(this);
		}
		m_pToolTip->UpdateTipText(text, this, 1);
		return true;
	}
	bool Autoload_Tooltip()
	{
		char text[512];
		unsigned int id = GetDlgCtrlID();
		if(LoadString(0, id, text, 512)!=0)
		{
			SetToolTip(text);
			return true;
		}
		return false;
	}
	//used for external plugins to render generic skinned buttons
	static __forceinline void DrawSkinnedButton(CButton *button, LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		   UINT uStyle = DFCS_BUTTONPUSH;			

		   // This code only works with buttons.
		   ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

			RECT rectOutside;
			RECT rectInside;		

			HDC hDC = lpDrawItemStruct->hDC;
			CDC dc;
			dc.Attach(hDC);

			button->GetWindowRect(&rectOutside);
			button->ScreenToClient(&rectOutside);

			int border = 2;

			rectInside.left = rectOutside.left + border;
			rectInside.right = rectOutside.right - border;
			rectInside.top = rectOutside.top + border;
			rectInside.bottom = rectOutside.bottom - border;
		
			if(lpDrawItemStruct->itemState & ODS_DISABLED)
			{
				dc.FillSolidRect(&rectInside,skin_button_disabled_color); 
				DrawSkinControlEdge(&dc, &rectOutside, true);
				::SetBkColor(hDC, skin_button_disabled_color);
			}
			else
			{
				UINT state = button->GetState();
				if(state & BST_PUSHED)
				{
					dc.FillSolidRect(&rectInside,skin_button_pushed_color); 
					DrawSkinControlEdge(&dc, &rectOutside, false);
					::SetBkColor(hDC, skin_button_pushed_color);
					::SetTextColor(hDC, skin_button_text_color);
				}
				else
				{
					dc.FillSolidRect(&rectInside,skin_button_color); 
					DrawSkinControlEdge(&dc, &rectOutside, true);
					::SetBkColor(hDC, skin_button_color);
					::SetTextColor(hDC, skin_button_text_color);
				}
			}
			
			int style = button->GetStyle();
			
			unsigned int wrap_style = 0;
			if(style & BS_MULTILINE)
			{
				wrap_style = DT_WORDBREAK|DT_VCENTER|DT_CENTER;
			}
			else
			{
				wrap_style = DT_SINGLELINE|DT_VCENTER|DT_CENTER;
			}		

		   CString strText;
		   button->GetWindowText(strText);

		   ::DrawText(hDC, strText, strText.GetLength(), &lpDrawItemStruct->rcItem, wrap_style);
	}
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
	{
		   UINT uStyle = DFCS_BUTTONPUSH;			

		   // This code only works with buttons.
		   ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

			RECT rectOutside;
			RECT rectInside;		

			HDC hDC = lpDrawItemStruct->hDC;
			CDC dc;
			dc.Attach(hDC);

			GetWindowRect(&rectOutside);
			ScreenToClient(&rectOutside);

			int border = 2;

			rectInside.left = rectOutside.left + border;
			rectInside.right = rectOutside.right - border;
			rectInside.top = rectOutside.top + border;
			rectInside.bottom = rectOutside.bottom - border;
		
			if(lpDrawItemStruct->itemState & ODS_DISABLED)
			{
				dc.FillSolidRect(&rectInside,skin_button_disabled_color); 
				DrawSkinControlEdge(&dc, &rectOutside, true);
				::SetBkColor(hDC, skin_button_disabled_color);
			}
			else
			{
				UINT state = GetState();
				if(state & BST_PUSHED || render_pushed)
				{
					dc.FillSolidRect(&rectInside,skin_button_pushed_color); 
					DrawSkinControlEdge(&dc, &rectOutside, false);
					::SetBkColor(hDC, skin_button_pushed_color);
					::SetTextColor(hDC, skin_button_text_color);
				}
				else
				{
					dc.FillSolidRect(&rectInside,skin_button_color); 
					DrawSkinControlEdge(&dc, &rectOutside, true);
					::SetBkColor(hDC, skin_button_color);
					::SetTextColor(hDC, skin_button_text_color);
				}
			}
			
			int style = GetStyle();
			
			unsigned int wrap_style = 0;
			if(style & BS_MULTILINE)
			{
				wrap_style = DT_WORDBREAK|DT_VCENTER|DT_CENTER;
			}
			else
			{
				wrap_style = DT_SINGLELINE|DT_VCENTER|DT_CENTER;
			}		

		   CString strText;
		   GetWindowText(strText);

		   ::DrawText(hDC, strText, strText.GetLength(), &lpDrawItemStruct->rcItem, wrap_style);
		   if(render_bmp)
		   {
			   dc.BitBlt(bmp_xstart, bmp_ystart, bmp_width, bmp_height, pBMPDC, bmp_xoff, bmp_yoff, SRCCOPY);
		   }
	}
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		//this doesn't get called in modeless dialog boxes
		//like 95% of the ones used in this app and ALL of the ones
		//that actually need tooltips
		if(m_pToolTip){m_pToolTip->RelayEvent(pMsg);}
		return CButton::PreTranslateMessage(pMsg);
	}
};







#endif
