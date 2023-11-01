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
#if !defined(AFX_TRACKOUTLINEDLG_H__30DDFB07_9D3B_4717_AB89_20ED3062E3E4__INCLUDED_)
#define AFX_TRACKOUTLINEDLG_H__30DDFB07_9D3B_4717_AB89_20ED3062E3E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrackOutlineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TrackOutlineDlg dialog

class TrackOutlineDlg : public CDialog
{
// Construction
public:
	TrackOutlineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TrackOutlineDlg)
	enum { IDD = IDD_TRACK_OUTLINE_DLG };
	CSkinButton	m_ReplaceLayers;
	CSkinButton	m_GenerateTrackingInfo;
	CSkinButton	m_ExtraSmoothOutlines;
	CSkinButton	m_ExpandOutlines;
	CSkinButton	m_CreateLayersFromTrackingInfo;
	CSkinButton	m_CreateLayersAndAdvanceFrame;
	CSkinButton	m_ContractOutlines;
	CSkinButton	m_ClearTrackingInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TrackOutlineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TrackOutlineDlg)
	afx_msg void OnGenerateTrackingInfoForSelection();
	afx_msg void OnClearTrackingInfo();
	afx_msg void OnCreateLayersFromTrackingInfo();
	afx_msg void OnReplaceLayersFromTrackingInfo();
	afx_msg void OnCreateLayersAndAdvanceFrame();
	afx_msg void OnExtraSmoothOutlines();
	afx_msg void OnExpandOutlines();
	afx_msg void OnContractOutlines();
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACKOUTLINEDLG_H__30DDFB07_9D3B_4717_AB89_20ED3062E3E4__INCLUDED_)
