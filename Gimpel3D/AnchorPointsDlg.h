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
#if !defined(AFX_ANCHORPOINTSDLG_H__E171D2DC_0E7E_4CE0_BC60_6B1827EC3F37__INCLUDED_)
#define AFX_ANCHORPOINTSDLG_H__E171D2DC_0E7E_4CE0_BC60_6B1827EC3F37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnchorPointsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AnchorPointsDlg dialog

class AnchorPointsDlg : public CDialog
{
// Construction
public:
	AnchorPointsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AnchorPointsDlg)
	enum { IDD = IDD_ANCHOR_POINTS_DLG };
	CSkinButton	m_RemoveAnchorPointFromSelectedFeaturePoint;
	CSkinButton	m_RemoveAllAnchorPoints;
	CSkinButton	m_AttachAnchorPointToSelectedFeaturePoint;
	CStatic	m_Static1;
	CSkinButton	m_CopyAnchorPoints;
	CSkinButton	m_ClearSavedAnchorPoints;
	CSkinButton	m_ApplySavedAnchorPoints;
	CButton	m_UpdateInRealtime;
	CButton	m_ClickToMovePoints;
	CButton	m_ClickToDeletePoints;
	CButton	m_ClickToAddPoints;
	CSkinButton	m_UpdateAffectedLayers;
	CSkinButton	m_ClearAnchorPoints;
	CEdit	m_AnchorRange;
	SkinSlider	m_AnchorRangeSlider;
	CListBox	m_AnchorPointsList;
	CListBox	m_LayersList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AnchorPointsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AnchorPointsDlg)
	afx_msg void OnClose();
	afx_msg void OnSelchangeLayersList();
	afx_msg void OnSelchangeAnchorPointsList();
	afx_msg void OnClickToAddPoints();
	afx_msg void OnClearAnchorPoints();
	afx_msg void OnClickToDeletePoints();
	afx_msg void OnClickToMovePoints();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateAffectedLayers();
	afx_msg void OnLinearFalloff();
	afx_msg void OnStretchFalloff();
	afx_msg void OnCurveFalloff();
	afx_msg void OnUpdateAnchorPointsInRealtime();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCopyAnchorPoints();
	afx_msg void OnApplySavedAnchorPoints();
	afx_msg void OnClearSavedAnchorPoints();
	afx_msg void OnAttachAnchorPointToSelectedFeaturePoint();
	afx_msg void OnRemoveAnchorPointFromSelectedFeaturePoint();
	afx_msg void OnRemoveAllAnchorPoints();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANCHORPOINTSDLG_H__E171D2DC_0E7E_4CE0_BC60_6B1827EC3F37__INCLUDED_)
