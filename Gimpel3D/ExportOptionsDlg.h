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
#if !defined(AFX_EXPORTOPTIONSDLG_H__F24B5547_7384_4B91_AA2A_37650F924B2D__INCLUDED_)
#define AFX_EXPORTOPTIONSDLG_H__F24B5547_7384_4B91_AA2A_37650F924B2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ExportOptionsDlg dialog

class ExportOptionsDlg : public CDialog
{
// Construction
public:
	ExportOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	char extension[8];
	char file_prefix[64];
	int image_type;
	int start_frame, end_frame;
	char output_path[512];
	void UpdateData();
	void Export_AVI();

// Dialog Data
	//{{AFX_DATA(ExportOptionsDlg)
	enum { IDD = IDD_EXPORT_OPTIONS };
	CSkinButton	m_StartExport;
	CSkinButton	m_SaveStereo;
	CSkinButton	m_SaveRight;
	CSkinButton	m_SaveLeft;
	CSkinButton	m_SaveAnaglyph;
	CSkinButton	m_BrowseOutputPath;
	CComboBox	m_FileFormat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ExportOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ExportOptionsDlg)
	afx_msg void OnClose();
	afx_msg void OnAnaglyphOutput();
	afx_msg void OnStereoOutput();
	afx_msg void OnLeftRightOutput();
	afx_msg void OnCenterView();
	afx_msg void OnStartExport();
	afx_msg void OnBrowseOutputPath();
	afx_msg void OnSaveLeftImage();
	afx_msg void OnSaveRightImage();
	afx_msg void OnSaveStereoImage();
	afx_msg void OnSaveAnaglyph();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnUseManualRasterization();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTOPTIONSDLG_H__F24B5547_7384_4B91_AA2A_37650F924B2D__INCLUDED_)
