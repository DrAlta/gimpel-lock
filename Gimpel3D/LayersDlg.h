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
#if !defined(AFX_LAYERSDLG_H__E7AC8ECD_2E73_4AD9_8B71_5434EB896939__INCLUDED_)
#define AFX_LAYERSDLG_H__E7AC8ECD_2E73_4AD9_8B71_5434EB896939__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayersDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LayersDlg dialog

class LayersDlg : public CDialog
{
// Construction
public:
	LayersDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateSelectionInfo();

// Dialog Data
	//{{AFX_DATA(LayersDlg)
	enum { IDD = IDD_LAYERS_DLG };
	CSkinButton	m_UpdateLayers;
	CStatic	m_LayersLabel;
	CListBox	m_LayersList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LayersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(LayersDlg)
	afx_msg void OnSelchangeLayersList();
	afx_msg void OnLayerKeyframe();
	afx_msg void OnLayerVisible();
	afx_msg void OnOutlineLayer();
	afx_msg void OnSpeckleLayer();
	afx_msg void OnFreezeLayer();
	afx_msg void OnUpdateLayerData();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERSDLG_H__E7AC8ECD_2E73_4AD9_8B71_5434EB896939__INCLUDED_)
