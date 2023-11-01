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
#if !defined(AFX_VIRTUALCAMERADLG_H__DC9B4989_CBA0_4B64_B44F_920580DB7856__INCLUDED_)
#define AFX_VIRTUALCAMERADLG_H__DC9B4989_CBA0_4B64_B44F_920580DB7856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VirtualCameraDlg.h : header file
//

#include "Skin.h"

/////////////////////////////////////////////////////////////////////////////
// VirtualCameraDlg dialog

class VirtualCameraDlg : public CDialog
{
// Construction
public:
	VirtualCameraDlg(CWnd* pParent = NULL);   // standard constructor
	void ListFrames();

// Dialog Data
	//{{AFX_DATA(VirtualCameraDlg)
	enum { IDD = IDD_VIRTUAL_CAMERA_DLG };
	CStatic	m_Static1;
	CEdit	m_SmoothCameraSteps;
	CSkinButton	m_SmoothCameraPath;
	CButton	m_RenderCameraDirection;
	CButton	m_TrackYaw;
	CButton	m_TrackPitch;
	CSkinButton	m_GenerateRelativeCameraPosition;
	CSkinButton	m_ResetVirtualCameraInfo;
	CButton	m_UnSelectTrackingFeaturePoints;
	CButton	m_SelectTrackingFeaturePoints;
	CButton	m_RenderCameraVectorsToPoints;
	CButton	m_RenderCameraPath;
	CSkinButton	m_SetCurrentFrameAsReference;
	CSkinButton	m_GenerateRelativeCameraPath;
	CSkinButton	m_ClearTrackedFeaturePoints;
	CListBox	m_FramesList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VirtualCameraDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VirtualCameraDlg)
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelectTrackingFeaturePoints();
	afx_msg void OnUnselectTrackingFeaturePoints();
	afx_msg void OnClearTrackedFeaturePoints();
	afx_msg void OnRenderCameraVectorsToPoints();
	afx_msg void OnRenderCameraPath();
	afx_msg void OnSetCurrentFrameAsReference();
	afx_msg void OnGenerateRelativeCameraPath();
	afx_msg void OnSelchangeFramesList();
	afx_msg void OnResetVirtualCameraInfo();
	afx_msg void OnGenerateRelativeCameraPosition();
	afx_msg void OnTrackYaw();
	afx_msg void OnTrackPitch();
	afx_msg void OnRenderCameraDirection();
	afx_msg void OnSmoothCameraPath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRTUALCAMERADLG_H__DC9B4989_CBA0_4B64_B44F_920580DB7856__INCLUDED_)
