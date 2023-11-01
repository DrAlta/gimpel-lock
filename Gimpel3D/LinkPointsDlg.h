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
#if !defined(AFX_LINKPOINTSDLG_H__B9A19DCE_65C3_4EC2_AB70_FF9483AEADDB__INCLUDED_)
#define AFX_LINKPOINTSDLG_H__B9A19DCE_65C3_4EC2_AB70_FF9483AEADDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkPointsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LinkPointsDlg dialog

class LinkPointsDlg : public CDialog
{
// Construction
public:
	LinkPointsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(LinkPointsDlg)
	enum { IDD = IDD_LINK_POINTS_DLG };
	CButton	m_ShowVectors;
	CSkinButton	m_FindStartAlignment;
	CSkinButton	m_SaveLinks;
	CSkinButton	m_AlignModel4;
	CSkinButton	m_AlignModel3;
	CSkinButton	m_AlignModel2;
	CSkinButton	m_UnLinkSelectedPoints;
	CSkinButton	m_ClearLinks;
	CSkinButton	m_LinkSelectedPoints;
	CSkinButton	m_SaveLinkPoints;
	CSkinButton	m_ReLoadLinkPoints;
	CSkinButton	m_ClearLinkPoints;
	CButton	m_ViewScene;
	CButton	m_ViewModel;
	CButton	m_ClickToMovePoints;
	CButton	m_ClickToDeletePoints;
	CButton	m_ClickToAddPoints;
	CSkinButton	m_AlignModel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LinkPointsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(LinkPointsDlg)
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnAlignModel();
	afx_msg void OnClickToAddPoints();
	afx_msg void OnClickToDeletePoints();
	afx_msg void OnClickToMovePoints();
	afx_msg void OnViewModel();
	afx_msg void OnViewScene();
	afx_msg void OnClearLinkPoints();
	afx_msg void OnSaveLinkPoints();
	afx_msg void OnReloadLinkPoints();
	afx_msg void OnLinkSelectedPoints();
	afx_msg void OnClearLinks();
	afx_msg void OnUnlinkSelectedPoints();
	afx_msg void OnShowVectors();
	afx_msg void OnAlignModel2();
	afx_msg void OnAlignModel3();
	afx_msg void OnAlignModel4();
	afx_msg void OnSaveLinks();
	afx_msg void OnFindStartAlignment();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKPOINTSDLG_H__B9A19DCE_65C3_4EC2_AB70_FF9483AEADDB__INCLUDED_)
