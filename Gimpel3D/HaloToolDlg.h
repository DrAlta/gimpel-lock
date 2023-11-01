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
#if !defined(AFX_HALOTOOLDLG_H__4CD1E3CB_50F1_4BF9_862F_D06AF7619CEB__INCLUDED_)
#define AFX_HALOTOOLDLG_H__4CD1E3CB_50F1_4BF9_862F_D06AF7619CEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HaloToolDlg.h : header file
//

#include "Skin.h"

/////////////////////////////////////////////////////////////////////////////
// HaloToolDlg dialog

class HaloToolDlg : public CDialog
{
// Construction
public:
	HaloToolDlg(CWnd* pParent = NULL);   // standard constructor
	void ListLayers();

// Dialog Data
	//{{AFX_DATA(HaloToolDlg)
	enum { IDD = IDD_HALO_TOOL_DLG };
	CSkinButton	m_LoadHalosForFrame;
	CButton	m_AutoLoadHalos;
	CSkinButton	m_ClearSavedHaloInfo;
	CSkinButton	m_ApplySavedHaloInfo;
	CSkinButton	m_SaveHaloInfo;
	CSkinButton	m_DeleteAllHaloMasks;
	CStatic	m_Static1;
	CSkinButton	m_UpdateAllHaloGeometry;
	CButton	m_RenderHaloAlphaMask;
	CButton	m_Render3DHalo;
	CButton	m_Render2DHalo;
	CButton	m_BlendHaloWithBackground;
	CSkinButton	m_SaveHaloMask;
	CSkinButton	m_DeleteHaloMask;
	CSkinButton	m_SortHalosModifyFrame;
	CSkinButton	m_GenerateDefaultHalos;
	CListBox	m_LayersList;
	CSkinButton	m_UpdateHaloGeometry;
	CSkinButton	m_FindHaloOutline;
	SkinSlider	m_HaloRangeSlider;
	CStatic	m_HaloPixelRange;
	CSkinButton	m_GenerateHaloMask;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HaloToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(HaloToolDlg)
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnGenerateHaloMask();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFindHaloOutline();
	afx_msg void OnRender2dHalo();
	afx_msg void OnBlendHaloWithBackground();
	afx_msg void OnUpdateHaloGeometry();
	afx_msg void OnRender3dHalo();
	afx_msg void OnRenderHaloAlphamask();
	afx_msg void OnSelchangeLayersList();
	afx_msg void OnGenerateDefaultHalos();
	afx_msg void OnSortHalosModifyFrame();
	afx_msg void OnDeleteHaloMask();
	afx_msg void OnSaveHaloMask();
	afx_msg void OnUpdateAllHaloGeometry();
	afx_msg void OnDeleteAllHaloMasks();
	afx_msg void OnSaveHaloInfo();
	afx_msg void OnApplySavedHaloInfo();
	afx_msg void OnClearSavedHaloInfo();
	afx_msg void OnAutoLoadHalos();
	afx_msg void OnLoadHalosForFrame();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HALOTOOLDLG_H__4CD1E3CB_50F1_4BF9_862F_D06AF7619CEB__INCLUDED_)
