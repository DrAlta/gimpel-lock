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
// Gimpel3D.h : main header file for the GIMPEL3D application
//

#if !defined(AFX_GIMPEL3D_H__FFB2726A_F20B_48BC_AED7_B153343F399C__INCLUDED_)
#define AFX_GIMPEL3D_H__FFB2726A_F20B_48BC_AED7_B153343F399C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGimpel3DApp:
// See Gimpel3D.cpp for the implementation of this class
//

class CGimpel3DApp : public CWinApp
{
public:
	CGimpel3DApp();
	//virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGimpel3DApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGimpel3DApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GIMPEL3D_H__FFB2726A_F20B_48BC_AED7_B153343F399C__INCLUDED_)
