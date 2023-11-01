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
// ColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "ColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorDlg dialog
extern int primary_skin_color;
extern int secondary_skin_color;
extern float skin_hue_mix;

bool Set_Skin_Color(int color1, int color2, int mix);
bool Update_Menu_Color();

CColorDlg::CColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	skinframe = 0;
	save_colors = false;
}

bool update_slider_position_on_the_first_paint_message_so_it_gets_rendered_correctly_because_doing_it_on_initialization_bypasses_the_custom_rendering_for_no_apparent_reason_even_though_it_works_with_no_problem_when_the_dialog_box_is_non_modal = true;

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorDlg)
	DDX_Control(pDX, IDC_APPLY_DEFAULT_COLORS, m_ApplyDefaults);
	DDX_Control(pDX, IDC_APPLY_COLORS, m_ApplyColors);
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_EditText);
	DDX_Control(pDX, IDC_CHECKBOX, m_CheckBox);
	DDX_Control(pDX, IDC_DISABLED_BUTTON, m_DisabledButton);
	DDX_Control(pDX, IDC_PUSHED_BUTTON, m_PushedButton);
	DDX_Control(pDX, IDC_NORMAL_BUTTON, m_NormalButton);
	DDX_Control(pDX, IDC_MIX_SLIDER, m_MixSlider);
	//}}AFX_DATA_MAP
	if(!skinframe)
	{
		SetWindowText("Color Scheme");
		skinframe = Create_Skin_Frame(this);
		skinframe->SetWindowText("Color Scheme");
	}
	CSliderCtrl *ss = (CSliderCtrl*)GetDlgItem(IDC_MIX_SLIDER);

	UpdateCheckboxes();
	m_EditText.SetWindowText("Edit text");
	m_ListBox.AddString("Selected and");
	m_ListBox.AddString("Normal");
	m_ListBox.AddString("List");
	m_ListBox.AddString("Box");
	m_ListBox.AddString("Items");
	m_ListBox.SetCurSel(0);
	m_PushedButton.render_pushed = true;
	GetDlgItem(IDC_APPLY_DEFAULT_COLORS)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC4)->SetFont(skin_font);
	GetDlgItem(IDC_APPLY_COLORS)->SetFont(skin_font);
	GetDlgItem(IDC_LISTBOX)->SetFont(skin_font);
	GetDlgItem(IDC_CHECKBOX)->SetFont(skin_font);
	GetDlgItem(IDC_DISABLED_BUTTON)->SetFont(skin_font);
	GetDlgItem(IDC_PUSHED_BUTTON)->SetFont(skin_font);
	GetDlgItem(IDC_NORMAL_BUTTON)->SetFont(skin_font);
	
	update_slider_position_on_the_first_paint_message_so_it_gets_rendered_correctly_because_doing_it_on_initialization_bypasses_the_custom_rendering_for_no_apparent_reason_even_though_it_works_with_no_problem_when_the_dialog_box_is_non_modal = true;
}

void CColorDlg::UpdateCheckboxes()
{
	CheckDlgButton(IDC_RED1, primary_skin_color==0);
	CheckDlgButton(IDC_GREEN1, primary_skin_color==1);
	CheckDlgButton(IDC_BLUE1, primary_skin_color==2);
	CheckDlgButton(IDC_RED2, secondary_skin_color==0);
	CheckDlgButton(IDC_GREEN2, secondary_skin_color==1);
	CheckDlgButton(IDC_BLUE2, secondary_skin_color==2);
}

BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
	//{{AFX_MSG_MAP(CColorDlg)
	ON_BN_CLICKED(IDC_RED1, OnRed1)
	ON_BN_CLICKED(IDC_GREEN1, OnGreen1)
	ON_BN_CLICKED(IDC_BLUE1, OnBlue1)
	ON_BN_CLICKED(IDC_RED2, OnRed2)
	ON_BN_CLICKED(IDC_GREEN2, OnGreen2)
	ON_BN_CLICKED(IDC_BLUE2, OnBlue2)
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_APPLY_COLORS, OnApplyColors)
	ON_BN_CLICKED(IDC_APPLY_DEFAULT_COLORS, OnApplyDefaultColors)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorDlg message handlers

void CColorDlg::OnRed1() 
{
	primary_skin_color = 0;
	UpdateCheckboxes();
	Set_Skin_Color(primary_skin_color, secondary_skin_color, m_MixSlider.GetPos());
	RedrawWindow();
	skinframe->RedrawWindow();
}

void CColorDlg::OnGreen1() 
{
	primary_skin_color = 1;
	UpdateCheckboxes();
	Set_Skin_Color(primary_skin_color, secondary_skin_color, m_MixSlider.GetPos());
	RedrawWindow();
	skinframe->RedrawWindow();
}

void CColorDlg::OnBlue1() 
{
	primary_skin_color = 2;
	UpdateCheckboxes();
	Set_Skin_Color(primary_skin_color, secondary_skin_color, m_MixSlider.GetPos());
	RedrawWindow();
	skinframe->RedrawWindow();
}

void CColorDlg::OnRed2() 
{
	secondary_skin_color = 0;
	UpdateCheckboxes();
	Set_Skin_Color(primary_skin_color, secondary_skin_color, m_MixSlider.GetPos());
	RedrawWindow();
	skinframe->RedrawWindow();
}

void CColorDlg::OnGreen2() 
{
	secondary_skin_color = 1;
	UpdateCheckboxes();
	Set_Skin_Color(primary_skin_color, secondary_skin_color, m_MixSlider.GetPos());
	RedrawWindow();
	skinframe->RedrawWindow();
}

void CColorDlg::OnBlue2() 
{
	secondary_skin_color = 2;
	UpdateCheckboxes();
	Set_Skin_Color(primary_skin_color, secondary_skin_color, m_MixSlider.GetPos());
	RedrawWindow();
	skinframe->RedrawWindow();
}

void CColorDlg::OnApplyDefaultColors() 
{
	primary_skin_color = 2;
	secondary_skin_color = 1;
	skin_hue_mix = 0.25f;
	UpdateCheckboxes();
	float sp = skin_hue_mix*100;
	m_MixSlider.SetPos((int)sp);
	Set_Skin_Color(primary_skin_color, secondary_skin_color, m_MixSlider.GetPos());
	RedrawWindow();
	skinframe->RedrawWindow();
}

void CColorDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	Remove_Skinframe(skinframe);
}

void CColorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	Set_Skin_Color(primary_skin_color, secondary_skin_color, m_MixSlider.GetPos());
	RedrawWindow();
	skinframe->RedrawWindow();
}

HBRUSH CColorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3||id==IDC_STATIC4)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
}

void CColorDlg::OnApplyColors() 
{
	skinframe->SendMessage(WM_COMMAND, IDC_CLOSE_BUTTON, IDC_CLOSE_BUTTON);
	save_colors = true;
}

bool Run_Color_Dlg()
{
	int c1 = primary_skin_color;
	int c2 = secondary_skin_color;
	float sm = skin_hue_mix;
	CColorDlg dlg;
	dlg.DoModal();
	if(!dlg.save_colors)
	{
		primary_skin_color = c1;
		secondary_skin_color = c2;
		skin_hue_mix = sm;
		Set_Skin_Color(primary_skin_color, secondary_skin_color, (int)(skin_hue_mix*100));
	}
	else
	{
		Update_Menu_Color();
		mainwindow->ShowWindow(SW_MINIMIZE);
		mainwindow->ShowWindow(SW_MAXIMIZE);
	}
	mainwindow->SetFocus();
	return true;
}


void CColorDlg::OnPaint() 
{
	CPaintDC dc(this);
	if(update_slider_position_on_the_first_paint_message_so_it_gets_rendered_correctly_because_doing_it_on_initialization_bypasses_the_custom_rendering_for_no_apparent_reason_even_though_it_works_with_no_problem_when_the_dialog_box_is_non_modal)
	{
		CSliderCtrl *ss = (CSliderCtrl*)GetDlgItem(IDC_MIX_SLIDER);
		ss->SetRange(0, 100);
		float sp = skin_hue_mix*100;
		ss->SetPos((int)sp);
		update_slider_position_on_the_first_paint_message_so_it_gets_rendered_correctly_because_doing_it_on_initialization_bypasses_the_custom_rendering_for_no_apparent_reason_even_though_it_works_with_no_problem_when_the_dialog_box_is_non_modal = false;
	}
}

void CColorDlg::OnClose() 
{
	CDialog::OnClose();
}
