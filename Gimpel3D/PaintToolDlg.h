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
#if !defined(AFX_PAINTTOOLDLG_H__E25DB0CA_5412_4D48_A334_14BA9002F652__INCLUDED_)
#define AFX_PAINTTOOLDLG_H__E25DB0CA_5412_4D48_A334_14BA9002F652__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaintToolDlg.h : header file
//

#include "SkinFrame.h"
/////////////////////////////////////////////////////////////////////////////
// PaintToolDlg dialog

class PaintToolDlg : public CDialog
{
// Construction
public:
	PaintToolDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PaintToolDlg)
	enum { IDD = IDD_PAINT_TOOL_DLG };
	CStatic	m_Static3;
	CStatic	m_Static2;
	CStatic	m_Static1;
	CSkinButton	m_ResetGrayscaleImage;
	CEdit	m_GrayscaleNear;
	CEdit	m_GrayscaleFar;
	CButton	m_RenderLayermaskImage;
	CButton	m_RenderGrayscaleImage;
	CButton	m_RenderFrameImage;
	CButton	m_RenderPaintTool;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PaintToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PaintToolDlg)
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRenderPaintTool();
	afx_msg void OnRenderFrameImage();
	afx_msg void OnRenderGrayscaleImage();
	afx_msg void OnRenderLayermaskImage();
	afx_msg void OnResetGrayscaleImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAINTTOOLDLG_H__E25DB0CA_5412_4D48_A334_14BA9002F652__INCLUDED_)
