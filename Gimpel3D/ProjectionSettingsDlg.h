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
#if !defined(AFX_PROJECTIONSETTINGSDLG_H__03B8FE56_22BB_45FE_B786_142793FC574D__INCLUDED_)
#define AFX_PROJECTIONSETTINGSDLG_H__03B8FE56_22BB_45FE_B786_142793FC574D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectionSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ProjectionSettingsDlg dialog

class ProjectionSettingsDlg : public CDialog
{
// Construction
public:
	ProjectionSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ProjectionSettingsDlg)
	enum { IDD = IDD_PROJECTION_SETTINGS_DLG };
	CSkinButton	m_SetFov;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ProjectionSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ProjectionSettingsDlg)
	afx_msg void OnClose();
	afx_msg void OnFlatProjection();
	afx_msg void OnSetFov();
	afx_msg void OnDomeProjection();
	afx_msg void OnCylinderProjection();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTIONSETTINGSDLG_H__03B8FE56_22BB_45FE_B786_142793FC574D__INCLUDED_)
