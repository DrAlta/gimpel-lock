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
#if !defined(AFX_ALIGNMENTOPTIONSDLG_H__2CBE1578_5875_493F_B18C_36D5478CF3FC__INCLUDED_)
#define AFX_ALIGNMENTOPTIONSDLG_H__2CBE1578_5875_493F_B18C_36D5478CF3FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlignmentOptionsDlg.h : header file
//

#include "Skin.h"

/////////////////////////////////////////////////////////////////////////////
// AlignmentOptionsDlg dialog

class AlignmentOptionsDlg : public CDialog
{
// Construction
public:
	AlignmentOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AlignmentOptionsDlg)
	enum { IDD = IDD_ALIGNMENT_OPTIONS };
	CButton	m_AutoApplyToOtherFrames;
	CSkinButton	m_ApplyAlignmentToAllFrames;
	CSkinButton	m_AlignPrimitive;
	CSkinButton	m_IdentifyEdgePixels;
	CSkinButton	m_StartAlignment;
	CSkinButton	m_PlusY;
	CSkinButton	m_PlusX;
	CSkinButton	m_MinusY;
	CSkinButton	m_MinusX;
	CSkinButton	m_FormFit;
	CSkinButton	m_AlignYRot;
	CSkinButton	m_AlignXRot;
	CSkinButton	m_AlignParallel;
	CSkinButton	m_AlignDepth;
	CSkinButton	m_AlignAngle;
	CSkinButton	m_AlignAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AlignmentOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AlignmentOptionsDlg)
	afx_msg void OnClose();
	afx_msg void OnAlignDepth();
	afx_msg void OnAlignAngle();
	afx_msg void OnAlignXrot();
	afx_msg void OnAlignParallel();
	afx_msg void OnAlignYrot();
	afx_msg void OnAlignAll();
	afx_msg void OnStartAutoAlignment();
	afx_msg void OnPlusAngleY();
	afx_msg void OnMinusAngleY();
	afx_msg void OnPlusAngleX();
	afx_msg void OnMinusAngleX();
	afx_msg void OnFormFitToSurroundings();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnIdentifyEdgePixels();
	afx_msg void OnActivate();
	afx_msg void OnDeactivate();
	afx_msg void OnAllowScaleX();
	afx_msg void OnAllowScaleY();
	afx_msg void OnAllowScaleZ();
	afx_msg void OnAllowFreeMotion();
	afx_msg void OnAllowRotateX();
	afx_msg void OnAllowRotateY();
	afx_msg void OnAllowRotateZ();
	afx_msg void OnAlignPrimitive();
	afx_msg void OnAllowMoveX();
	afx_msg void OnAllowMoveY();
	afx_msg void OnAllowMoveZ();
	afx_msg void OnApplyAlignmentToAllFrames();
	afx_msg void OnAutoApplyToOtherFrames();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIGNMENTOPTIONSDLG_H__2CBE1578_5875_493F_B18C_36D5478CF3FC__INCLUDED_)
