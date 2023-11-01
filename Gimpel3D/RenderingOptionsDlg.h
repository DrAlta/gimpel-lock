/*
#if !defined(AFX_RENDERINGOPTIONSDLG_H__40E34EA7_3E19_45DB_8B2A_26B44C1F35C6__INCLUDED_)
#define AFX_RENDERINGOPTIONSDLG_H__40E34EA7_3E19_45DB_8B2A_26B44C1F35C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenderingOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// RenderingOptionsDlg dialog

class RenderingOptionsDlg : public CDialog
{
// Construction
public:
	RenderingOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	CBrush m_background_brush;

// Dialog Data
	//{{AFX_DATA(RenderingOptionsDlg)
	enum { IDD = IDD_RENDERING_OPTIONS_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RenderingOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RenderingOptionsDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnAnaglyph();
	afx_msg void OnAntialiasHardEdges();
	afx_msg void OnCullBackfaces();
	afx_msg void OnFullScreen2d();
	afx_msg void OnFullScreen3d();
	afx_msg void OnRenderBorders();
	afx_msg void OnRenderNormals();
	afx_msg void OnSplitScreen();
	afx_msg void OnWideScreen3d();
	afx_msg void OnCa();
	afx_msg void OnGs();
	afx_msg void OnLs();
	afx_msg void OnOa();
	afx_msg void OnShowOldGuis();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDERINGOPTIONSDLG_H__40E34EA7_3E19_45DB_8B2A_26B44C1F35C6__INCLUDED_)
*/
