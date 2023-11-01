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
#if !defined(AFX_VIRTUALSETDLG_H__68211A1C_2EF5_4583_B98F_9DBE5FD7CC5A__INCLUDED_)
#define AFX_VIRTUALSETDLG_H__68211A1C_2EF5_4583_B98F_9DBE5FD7CC5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VirtualSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VirtualSetDlg dialog

class VirtualSetDlg : public CDialog
{
// Construction
public:
	VirtualSetDlg(CWnd* pParent = NULL);   // standard constructor
	void ListLayers();

// Dialog Data
	//{{AFX_DATA(VirtualSetDlg)
	enum { IDD = IDD_VIRTUAL_SET };
	CSkinButton	m_StaticToDynamic;
	CSkinButton	m_DynamicToStatic;
	CSkinButton	m_TrackToNextFrame;
	CSkinButton	m_LoadFramework;
	CSkinButton	m_SaveFramework;
	CSkinButton	m_GenerateFramework;
	CListBox	m_DynamicLayersList;
	CListBox	m_StaticLayersList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VirtualSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VirtualSetDlg)
	afx_msg void OnClose();
	afx_msg void OnGenerateSceneFramework();
	afx_msg void OnSaveSceneFramework();
	afx_msg void OnTrackToNextFrame();
	afx_msg void OnLoadSceneFramework();
	afx_msg void OnRenderSceneFramework();
	afx_msg void OnSelchangeStaticLayersList();
	afx_msg void OnSelchangeDynamicLayersList();
	afx_msg void OnChangeStaticToDynamic();
	afx_msg void OnChangeDynamicToStatic();
	afx_msg void OnDblclkDynamicLayersList();
	afx_msg void OnDblclkStaticLayersList();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRTUALSETDLG_H__68211A1C_2EF5_4583_B98F_9DBE5FD7CC5A__INCLUDED_)
