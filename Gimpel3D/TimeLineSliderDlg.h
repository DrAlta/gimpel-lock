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
#if !defined(AFX_TIMELINESLIDERDLG_H__DB41F6FD_EB16_40D1_84D9_21BBB232E4E3__INCLUDED_)
#define AFX_TIMELINESLIDERDLG_H__DB41F6FD_EB16_40D1_84D9_21BBB232E4E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeLineSliderDlg.h : header file
//
struct FC_TEXT_ITEM
{
	char text[256];
	int xpos, ypos;
	int len;
};

/////////////////////////////////////////////////////////////////////////////
// TimeLineSliderDlg dialog

class TimeLineSliderDlg : public CDialog
{
// Construction
public:
	TimeLineSliderDlg(CWnd* pParent = NULL);   // standard constructor
	void Add_Text_Item(char *text, int xpos, int ypos);
	void ClearTextItems();
	void RefreshWindow();
	vector<FC_TEXT_ITEM> text_items;

// Dialog Data
	//{{AFX_DATA(TimeLineSliderDlg)
	enum { IDD = IDD_TIMELINE_SLIDER_DLG };
	CSkinButton	m_ShiftForward;
	CSkinButton	m_ShiftBack;
	CSkinButton	m_ZoomOut;
	CSkinButton	m_ZoomIn;
	SkinSlider	m_Slider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TimeLineSliderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TimeLineSliderDlg)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnPause();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnShiftBack();
	afx_msg void OnShiftForward();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMELINESLIDERDLG_H__DB41F6FD_EB16_40D1_84D9_21BBB232E4E3__INCLUDED_)
