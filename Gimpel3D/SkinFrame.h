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
#if !defined(AFX_SKINFRAME_H__5BD916E1_C7AA_4F1A_A0D2_BBBE246EDB6D__INCLUDED_)
#define AFX_SKINFRAME_H__5BD916E1_C7AA_4F1A_A0D2_BBBE246EDB6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinFrame.h : header file
//
#include "Skin.h"
/////////////////////////////////////////////////////////////////////////////
// SkinFrame dialog

class SkinFrame : public CDialog
{
// Construction
public:
	SkinFrame(CWnd* pParent = NULL);   // standard constructor
	CPoint mouse_pos;
	bool left_mouse_pressed;
	bool dragging_window;
	void DragWindow(int dx, int dy);
	bool PointInsideTitleBar(CPoint *p);
	void DrawSkinFrame(CDC *pDC);
	CWnd *child_window;
	int tooltray_control_id;
	bool plugin_control;

// Dialog Data
	//{{AFX_DATA(SkinFrame)
	enum { IDD = IDD_SKIN_FRAME };
	CSkinButton	m_MinimizeButton;
	CSkinButton	m_CloseButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SkinFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SkinFrame)
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCloseButton();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMinimizeButton();
//	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

bool ReSize_Skinframe(SkinFrame *sf);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINFRAME_H__5BD916E1_C7AA_4F1A_A0D2_BBBE246EDB6D__INCLUDED_)
