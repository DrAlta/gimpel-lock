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
#if !defined(AFX_IMPORTOPTIONSDLG_H__7E59E36D_1277_4F8D_AE27_9893AB64B738__INCLUDED_)
#define AFX_IMPORTOPTIONSDLG_H__7E59E36D_1277_4F8D_AE27_9893AB64B738__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ImportOptionsDlg dialog

class ImportOptionsDlg : public CDialog
{
// Construction
public:
	ImportOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdatePaths();

// Dialog Data
	//{{AFX_DATA(ImportOptionsDlg)
	enum { IDD = IDD_IMPORT_OPTIONS };
	CSkinButton	m_ImportLayermasks;
	CSkinButton	m_ImportImages;
	CSkinButton	m_ImportDepthMasks;
	CSkinButton	m_BrowseLayermasks;
	CSkinButton	m_BrowseImages;
	CSkinButton	m_BrowseDepthmasks;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ImportOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ImportOptionsDlg)
	afx_msg void OnClose();
	afx_msg void OnImportImages();
	afx_msg void OnBrowseImagesPath();
	afx_msg void OnImportLayermasks();
	afx_msg void OnBrowseLayermasksPath();
	afx_msg void OnApplyImageFilter();
	afx_msg void OnImportDepthmasks();
	afx_msg void OnBrowseDepthmasksPath();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTOPTIONSDLG_H__7E59E36D_1277_4F8D_AE27_9893AB64B738__INCLUDED_)
