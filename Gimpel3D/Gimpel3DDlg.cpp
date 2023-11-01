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
// Gimpel3DDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "Gimpel3DDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGimpel3DDlg dialog

CGimpel3DDlg::CGimpel3DDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGimpel3DDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGimpel3DDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//mainwindow = this;
}

void CGimpel3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGimpel3DDlg)
	DDX_Control(pDX, IDC_SPLASH, m_SplashImage);
	DDX_Control(pDX, IDC_START_EVALUATION, m_StartEvaluation);
	//}}AFX_DATA_MAP
	HDC mhDC = CreateDC("DISPLAY", 0, 0, 0); 
	int width = GetDeviceCaps(mhDC, HORZRES); 
	int height = GetDeviceCaps(mhDC, VERTRES); 
	DeleteDC(mhDC); 
	CRect rect;
	GetClientRect(&rect);
	int w = 700;
	int h = 237;
	int xpos = (width-w)/2;
	int ypos = (height-h)/2;
	SetWindowPos(0, xpos, ypos, w, h, 0);
	m_SplashImage.SetWindowPos(0, 0, 0, w, h, 0);
	SetWindowText("G3D - Stereo Conversion Software");

	GetDlgItem(IDC_STATUS)->SetFont(skin_font);
	GetDlgItem(IDC_START_EVALUATION)->SetFont(skin_font);


}

BEGIN_MESSAGE_MAP(CGimpel3DDlg, CDialog)
	//{{AFX_MSG_MAP(CGimpel3DDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_EVALUATION, OnStartEvaluation)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_SPLASH, &CGimpel3DDlg::OnStnClickedSplash)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGimpel3DDlg message handlers

BOOL CGimpel3DDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGimpel3DDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGimpel3DDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGimpel3DDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool splash_dlg_running = false;

void CGimpel3DDlg::OnStartEvaluation() 
{
	OnClose();
	ShowWindow(SW_HIDE);
	splash_dlg_running = false;
}

void CGimpel3DDlg::Close() 
{
	OnClose();
	splash_dlg_running = false;
}

CGimpel3DDlg *splashDlg = 0;

bool Init_Splash_Dialog()
{
	splashDlg = new CGimpel3DDlg;
	splashDlg->Create(IDD_GIMPEL3D_DIALOG);
	splashDlg->ShowWindow(SW_SHOW);
	splash_dlg_running = true;
	return true;
}

bool Free_Splash_Dialog()
{
	delete splashDlg;
	return true;
}

bool Wait_For_Splash_Dialog()
{
	splashDlg->SetFocus();
	Init_Clock();
	float elapsed = 0;
	float delay = 5;
	int last_rm = (int)delay;
	while(splash_dlg_running)
	{
		Update_Clock();
		elapsed += Delta_Time();
		if(elapsed>=delay)
		{
			splashDlg->GetDlgItem(IDC_STATUS)->EnableWindow(false);
			splashDlg->GetDlgItem(IDC_STATUS)->ShowWindow(SW_HIDE);
			splashDlg->GetDlgItem(IDC_START_EVALUATION)->EnableWindow(true);
			splashDlg->GetDlgItem(IDC_START_EVALUATION)->ShowWindow(SW_SHOW);
		}
		else
		{
			float rm = delay-elapsed;
			int trm = (int)rm;
			if(trm!=last_rm)
			{
				last_rm = trm;
				char text[256];
				sprintf(text, "Starting in %i seconds.", last_rm+1);
				splashDlg->SetDlgItemText(IDC_STATUS, text);
			}
		}
		MSG msg;
		while(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{TranslateMessage(&msg);DispatchMessage(&msg);}
	}
	return true;
}

bool Init_Closing_Splash_Dialog()
{
	splashDlg = new CGimpel3DDlg;
	splashDlg->Create(IDD_GIMPEL3D_DIALOG);
	splashDlg->ShowWindow(SW_SHOW);
	splash_dlg_running = true;
	return true;
}

bool Free_Closing_Splash_Dialog()
{
	delete splashDlg;
	return true;
}

bool Wait_For_Closing_Splash_Dialog()
{
	splashDlg->SetFocus();
	Init_Clock();
	float elapsed = 0;
	float delay = 5;
	int last_rm = (int)delay;
	splashDlg->GetDlgItem(IDC_CLOSING_STATEMENT)->ShowWindow(SW_SHOW);
	splashDlg->GetDlgItem(IDC_CLOSING_STATEMENT)->SetWindowText("                                                         G3D-Stereo Conversion Software                                                          Copyright 2010 - René Gimpel");
	splashDlg->GetDlgItem(IDC_STATUS)->EnableWindow(false);
	splashDlg->GetDlgItem(IDC_STATUS)->ShowWindow(SW_HIDE);
	splashDlg->GetDlgItem(IDC_START_EVALUATION)->EnableWindow(true);
	splashDlg->GetDlgItem(IDC_START_EVALUATION)->SetWindowText("Close");
	splashDlg->GetDlgItem(IDC_START_EVALUATION)->ShowWindow(SW_SHOW);
	splash_dlg_running = true;
	while(splash_dlg_running)
	{
		MSG msg;
		while(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{TranslateMessage(&msg);DispatchMessage(&msg);}
	}
	return true;
}


HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

HBRUSH CGimpel3DDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATUS||id==IDC_CLOSING_STATEMENT)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CGimpel3DDlg::OnStnClickedSplash()
{
}
