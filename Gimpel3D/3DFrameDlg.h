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
#if !defined(AFX_3DFRAMEDLG_H__F283D3CF_03BA_4AB8_BD31_DFC757C02CE1__INCLUDED_)
#define AFX_3DFRAMEDLG_H__F283D3CF_03BA_4AB8_BD31_DFC757C02CE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DFrameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C3DFrameDlg dialog

class C3DFrameDlg : public CDialog
{
// Construction
public:
	C3DFrameDlg(CWnd* pParent = NULL);   // standard constructor
	void Close();
	CBrush m_background_brush;

// Dialog Data
	//{{AFX_DATA(C3DFrameDlg)
	enum { IDD = IDD_3D_FRAME_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DFrameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C3DFrameDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DFRAMEDLG_H__F283D3CF_03BA_4AB8_BD31_DFC757C02CE1__INCLUDED_)
