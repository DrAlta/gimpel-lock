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
#if !defined(AFX_FEATUREPOINTSDLG_H__2040CE70_3F18_4111_BFA3_A79BB3F14202__INCLUDED_)
#define AFX_FEATUREPOINTSDLG_H__2040CE70_3F18_4111_BFA3_A79BB3F14202__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FeaturePointsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FeaturePointsDlg dialog

class FeaturePointsDlg : public CDialog
{
// Construction
public:
	FeaturePointsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FeaturePointsDlg)
	enum { IDD = IDD_FEATURE_POINTS_DLG };
	CSkinButton	m_ClearAllKeyframesForThisPoint;
	CSkinButton	m_KeyframeAllPointsForThisFrame;
	CButton	m_KeyframeSelectedFeaturePoint;
	CSkinButton	m_StartOutline;
	CSkinButton	m_FinishOutline;
	CSkinButton	m_ClickToMovePoints;
	CSkinButton	m_ClickToDeletePoints;
	CSkinButton	m_ClickToAddPoints;
	CSkinButton	m_ClearAllPoints;
	CSkinButton	m_ClearAllKeyframes;
	CSkinButton	m_CancelOutline;
	CListBox	m_OutlineList;
	CListBox	m_FeaturePointList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FeaturePointsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FeaturePointsDlg)
	afx_msg void OnClose();
	afx_msg void OnClickToAddPoints();
	afx_msg void OnClickToDeletePoints();
	afx_msg void OnClickToMovePoints();
	afx_msg void OnClearAllPoints();
	afx_msg void OnTrackToNextFrame();
	afx_msg void OnTrackFeaturePoints();
	afx_msg void OnClearAllFeaturePointKeyframes();
	afx_msg void OnSelchangeFeaturePointOutlineList();
	afx_msg void OnStartFeaturePointOutline();
	afx_msg void OnFinishFeaturePointOutline();
	afx_msg void OnCancelFeaturePointOutline();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnKeyframeSelectedFeaturePoint();
	afx_msg void OnKeyframeAllPointsForThisFrame();
	afx_msg void OnClearAllKeyframesForThisPoint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FEATUREPOINTSDLG_H__2040CE70_3F18_4111_BFA3_A79BB3F14202__INCLUDED_)
