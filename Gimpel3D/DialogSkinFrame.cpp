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
// DialogSkinFrame.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "DialogSkinFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DialogSkinFrame

DialogSkinFrame::DialogSkinFrame()
{
}

DialogSkinFrame::~DialogSkinFrame()
{
}


BEGIN_MESSAGE_MAP(DialogSkinFrame, CWnd)
	//{{AFX_MSG_MAP(DialogSkinFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// DialogSkinFrame message handlers

bool _Create_Skin_Frame(CDialog *child)
{
	DialogSkinFrame *sf = new DialogSkinFrame;
	if(sf->Create(0, "Dialog Frame", WS_POPUP | WS_CAPTION, CRect(200, 200, 300, 300), 0, 1234)==0)
	{
		MessageBox(0, "Error creating dialog frame window!", 0, MB_OK);
		delete sf;
		return false;
	}
	sf->SetForegroundWindow();
	sf->ShowWindow(SW_SHOW);
	return true;
}
