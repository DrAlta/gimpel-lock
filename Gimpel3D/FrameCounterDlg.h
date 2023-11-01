/*
#if !defined(AFX_FRAMECOUNTERDLG_H__91098B49_8F36_4548_A3FC_A68FFCDAA442__INCLUDED_)
#define AFX_FRAMECOUNTERDLG_H__91098B49_8F36_4548_A3FC_A68FFCDAA442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameCounterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FrameCounterDlg dialog

struct FC_TEXT_ITEM
{
	char text[256];
	int xpos, ypos;
	int len;
};

class FrameCounterDlg : public CDialog
{
// Construction
public:
	FrameCounterDlg(CWnd* pParent = NULL);   // standard constructor
	void Add_Text_Item(char *text, int xpos, int ypos);
	void ClearTextItems();
	void RefreshWindow();
	vector<FC_TEXT_ITEM> text_items;

// Dialog Data
	//{{AFX_DATA(FrameCounterDlg)
	enum { IDD = IDD_FRAME_COUNTER_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FrameCounterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FrameCounterDlg)
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMECOUNTERDLG_H__91098B49_8F36_4548_A3FC_A68FFCDAA442__INCLUDED_)
*/