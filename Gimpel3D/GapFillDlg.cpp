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
// GapFillDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "GapFillDlg.h"
#include "GapFillTool.h"
#include <gl/gl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GapFillDlg dialog

bool Get_Left_View_Gapmask(unsigned char *rgb);
bool Get_Right_View_Gapmask(unsigned char *rgb);
bool Save_Watermarked_Image(unsigned char *rgb, int w, int h, char *file);
bool Generate_Anaglyph_Image_From_24Bit(int n, unsigned char *left, unsigned char *right, unsigned char *res);

bool Get_Left_Blended_Gap_Fill_Image(unsigned char *rgb);
bool Get_Right_Blended_Gap_Fill_Image(unsigned char *rgb);
bool Save_Stereo_Watermarked_Image(unsigned char *rgb, int w, int h, char *file);
bool Add_Lines_To_Image(unsigned char *rgb, int width, int height);

GapFillDlg *gapFillDlg = 0;

GapFillDlg::GapFillDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GapFillDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(GapFillDlg)
	//}}AFX_DATA_INIT
}


void GapFillDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GapFillDlg)
	DDX_Control(pDX, IDC_EXPORT_RIGHT_GAPMASK, m_ExportRightGapMask);
	DDX_Control(pDX, IDC_EXPORT_RIGHT_FILLED_IMAGE, m_ExportRightFilledImage);
	DDX_Control(pDX, IDC_EXPORT_LEFT_GAPMASK, m_ExportLeftGapMask);
	DDX_Control(pDX, IDC_EXPORT_LEFT_FILLED_IMAGE, m_ExportLeftFilledImage);
	DDX_Control(pDX, IDC_EXPORT_FILLED_STEREO, m_ExportFilledStereo);
	DDX_Control(pDX, IDC_EXPORT_FILLED_ANAGLYPH, m_ExportFilledAnaglyph);
	DDX_Control(pDX, IDC_RESTORE_ORIGINAL_IMAGE, m_RestoreOriginalImage);
	DDX_Control(pDX, IDC_REFRESH_LIST, m_RefreshList);
	DDX_Control(pDX, IDC_PREVIEW_FILLED_ANAGLYPH, m_PreviewFilledAnaglyph);
	DDX_Control(pDX, IDC_PREVIEW_BLENDED_GAP_FILL, m_PreviewBlendedGapFill);
	DDX_Control(pDX, IDC_NO_FILL_ALL, m_NoFillAll);
	DDX_Control(pDX, IDC_INTERPOLATE_ALL, m_InterpolateAll);
	DDX_Control(pDX, IDC_FINALIZE_GAP_PIXELS, m_FinalizeGapPixels);
	DDX_Control(pDX, IDC_FINALIZE_GAP_FILL, m_FinalizeGapFill);
	DDX_Control(pDX, IDC_DUPLICATE_ALL, m_DuplicateAll);
	DDX_Control(pDX, IDC_CLEAR_GAP_INFO, m_ClearGapInfo);
	DDX_Control(pDX, IDC_CLEAR_BLEND_PIXELS, m_ClearBlendPixels);
	DDX_Control(pDX, IDC_BROWSE_BLEND_IMAGE, m_BrowseBlendImage);
	DDX_Control(pDX, IDC_BLEND_IMAGE_ALL, m_BlendImageAll);
	DDX_Control(pDX, IDC_APPLY_BLEND_PIXELS, m_ApplyBlendPixels);
	DDX_Control(pDX, IDC_ALIGN_BLEND_IMAGE, m_AlignBlendImage);
	DDX_Control(pDX, IDC_BLEND_FRAME_LIST, m_FramesList);
	DDX_Control(pDX, IDC_BACKGROUND_COLOR_BUTTON, m_BackgroundColorButton);
	DDX_Control(pDX, IDC_GAP_LIST, m_GapList);
	DDX_Control(pDX, IDC_GAP_THRESHOLD_SLIDER, m_GapThresholdSlider);
	//}}AFX_DATA_MAP
	CheckDlgButton(IDC_GAPFILL_LEFT, view_gapfill_left);
	CheckDlgButton(IDC_GAPFILL_RIGHT, view_gapfill_right);
	CheckDlgButton(IDC_GAPFILL_ANAGLYPH, view_gapfill_anaglyph);
	m_BackgroundColorButton.Set_Color((unsigned char)(gapfill_background_color[0]*255),(unsigned char)(gapfill_background_color[1]*255),(unsigned char)(gapfill_background_color[2]*255));
	threshold_slider_resolution = 256;
	m_GapThresholdSlider.SetRange(0, threshold_slider_resolution);
	CheckDlgButton(IDC_HIGHLIGHT_GAPS, highlight_gaps);
	CheckDlgButton(IDC_SHOW_GAPS, show_gaps);
	CheckDlgButton(IDC_RENDER_GHOST_BLEND_IMAGE, render_ghost_blend_image);
	CheckDlgButton(IDC_RENDER_BACKGROUND_BLEND_IMAGE, render_background_blend_image);

	m_ExportRightGapMask.SetFont(skin_font);
	m_ExportRightFilledImage.SetFont(skin_font);
	m_ExportLeftGapMask.SetFont(skin_font);
	m_ExportLeftFilledImage.SetFont(skin_font);
	m_ExportFilledStereo.SetFont(skin_font);
	m_ExportFilledAnaglyph.SetFont(skin_font);


	GetDlgItem(IDC_GAP_THRESHOLD_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_GAP_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_GAPFILL_SAVE)->SetFont(skin_font);
	GetDlgItem(IDC_GAPFILL_LEFT)->SetFont(skin_font);
	GetDlgItem(IDC_GAPFILL_RIGHT)->SetFont(skin_font);
	GetDlgItem(IDC_GAPFILL_ANAGLYPH)->SetFont(skin_font);
	GetDlgItem(IDC_BACKGROUND_COLOR_BUTTON)->SetFont(skin_font);
	GetDlgItem(IDC_NO_FILL)->SetFont(skin_font);
	GetDlgItem(IDC_INTERPOLATE_FILL)->SetFont(skin_font);
	GetDlgItem(IDC_DUPLICATE_FILL)->SetFont(skin_font);
	GetDlgItem(IDC_BLEND_FILL)->SetFont(skin_font);
	GetDlgItem(IDC_BLEND_FRAME_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_BROWSE_BLEND_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_REFRESH_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_CLEAR_GAP_INFO)->SetFont(skin_font);
	GetDlgItem(IDC_HIGHLIGHT_GAPS)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC4)->SetFont(skin_font);
	GetDlgItem(IDC_NO_FILL_ALL)->SetFont(skin_font);
	GetDlgItem(IDC_INTERPOLATE_ALL)->SetFont(skin_font);
	GetDlgItem(IDC_DUPLICATE_ALL)->SetFont(skin_font);
	GetDlgItem(IDC_BLEND_IMAGE_ALL)->SetFont(skin_font);
	GetDlgItem(IDC_STATUS)->SetFont(skin_font);
	GetDlgItem(IDC_FINALIZE_GAP_FILL)->SetFont(skin_font);
	GetDlgItem(IDC_RENDER_GHOST_BLEND_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_APPLY_BLEND_PIXELS)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_BLEND_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_FINALIZE_GAP_PIXELS)->SetFont(skin_font);
	GetDlgItem(IDC_PREVIEW_BLENDED_GAP_FILL)->SetFont(skin_font);
	GetDlgItem(IDC_RESTORE_ORIGINAL_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_SHOW_GAPS)->SetFont(skin_font);
	GetDlgItem(IDC_PREVIEW_FILLED_ANAGLYPH)->SetFont(skin_font);
	GetDlgItem(IDC_RENDER_BACKGROUND_BLEND_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_CLEAR_BLEND_PIXELS)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(GapFillDlg, CDialog)
	//{{AFX_MSG_MAP(GapFillDlg)
	ON_BN_CLICKED(IDC_INTERPOLATE_FILL, OnInterpolateFill)
	ON_BN_CLICKED(IDC_DUPLICATE_FILL, OnDuplicateFill)
	ON_BN_CLICKED(IDC_BLEND_FILL, OnBlendFill)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_BLEND_FRAME_LIST, OnSelchangeBlendFrameList)
	ON_LBN_SELCHANGE(IDC_GAP_LIST, OnSelchangeGapList)
	ON_BN_CLICKED(IDC_BROWSE_BLEND_IMAGE, OnBrowseBlendImage)
	ON_BN_CLICKED(IDC_REFRESH_LIST, OnRefreshList)
	ON_BN_CLICKED(IDC_GAPFILL_SAVE, OnGapfillSave)
	ON_BN_CLICKED(IDC_GAPFILL_LEFT, OnGapfillLeft)
	ON_BN_CLICKED(IDC_GAPFILL_RIGHT, OnGapfillRight)
	ON_BN_CLICKED(IDC_GAPFILL_ANAGLYPH, OnGapfillAnaglyph)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BACKGROUND_COLOR_BUTTON, OnBackgroundColorButton)
	ON_BN_CLICKED(IDC_CLEAR_GAP_INFO, OnClearGapInfo)
	ON_BN_CLICKED(IDC_NO_FILL, OnNoFill)
	ON_BN_CLICKED(IDC_NO_FILL_ALL, OnNoFillAll)
	ON_BN_CLICKED(IDC_INTERPOLATE_ALL, OnInterpolateAll)
	ON_BN_CLICKED(IDC_DUPLICATE_ALL, OnDuplicateAll)
	ON_BN_CLICKED(IDC_BLEND_IMAGE_ALL, OnBlendImageAll)
	ON_BN_CLICKED(IDC_HIGHLIGHT_GAPS, OnHighlightGaps)
	ON_BN_CLICKED(IDC_FINALIZE_GAP_FILL, OnFinalizeGapFill)
	ON_BN_CLICKED(IDC_RENDER_GHOST_BLEND_IMAGE, OnRenderGhostBlendImage)
	ON_BN_CLICKED(IDC_APPLY_BLEND_PIXELS, OnApplyBlendPixels)
	ON_BN_CLICKED(IDC_ALIGN_BLEND_IMAGE, OnAlignBlendImage)
	ON_BN_CLICKED(IDC_FINALIZE_GAP_PIXELS, OnFinalizeGapPixels)
	ON_BN_CLICKED(IDC_PREVIEW_BLENDED_GAP_FILL, OnPreviewBlendedGapFill)
	ON_BN_CLICKED(IDC_RESTORE_ORIGINAL_IMAGE, OnRestoreOriginalImage)
	ON_BN_CLICKED(IDC_SHOW_GAPS, OnShowGaps)
	ON_BN_CLICKED(IDC_PREVIEW_FILLED_ANAGLYPH, OnPreviewFilledAnaglyph)
	ON_BN_CLICKED(IDC_RENDER_BACKGROUND_BLEND_IMAGE, OnRenderBackgroundBlendImage)
	ON_BN_CLICKED(IDC_CLEAR_BLEND_PIXELS, OnClearBlendPixels)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_EXPORT_LEFT_GAPMASK, OnExportLeftGapmask)
	ON_BN_CLICKED(IDC_EXPORT_RIGHT_GAPMASK, OnExportRightGapmask)
	ON_BN_CLICKED(IDC_EXPORT_LEFT_FILLED_IMAGE, OnExportLeftFilledImage)
	ON_BN_CLICKED(IDC_EXPORT_RIGHT_FILLED_IMAGE, OnExportRightFilledImage)
	ON_BN_CLICKED(IDC_EXPORT_FILLED_ANAGLYPH, OnExportFilledAnaglyph)
	ON_BN_CLICKED(IDC_EXPORT_FILLED_STEREO, OnExportFilledStereo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GapFillDlg message handlers

bool Open_GapFill_Dialog()
{
	if(!gapFillDlg)
	{
		gapFillDlg = new GapFillDlg;
		gapFillDlg->Create(IDD_GAP_FILL_DLG);
		gapFillDlg->SetWindowText("Gap Fill");
		Create_ToolSkin_Frame(gapFillDlg, "Gap Fill", IDC_OPEN_GAPFILL_EDITOR);
	}
	gapFillDlg->GetParent()->ShowWindow(SW_SHOW);
	gapFillDlg->m_GapList.ResetContent();
	gapFillDlg->ListGaps();
	gapFillDlg->ListFrames();
	return true;
}

bool Close_GapFill_Dialog()
{
	if(gapFillDlg)
	{
		gapFillDlg->ShowWindow(SW_HIDE);
	}
	return true;
}

void Print_GapFill_Status(const char *fmt, ...)
{
	if(!gapFillDlg)return;
	char text[256];
	va_list		ap;
	if (fmt == NULL)return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	gapFillDlg->SetDlgItemText(IDC_STATUS, text);
}

void Update_GapFill_Slider()
{
	if(gapFillDlg)
	{
		if(view_gapfill_left)
		{
			gapFillDlg->m_GapThresholdSlider.SetPos(left_gap_slider_pos);
		}
		else if(view_gapfill_right)
		{
			gapFillDlg->m_GapThresholdSlider.SetPos(right_gap_slider_pos);
		}
	}
}

void Update_GapFill_List()
{
	if(gapFillDlg)
	{
		gapFillDlg->ListGaps();
	}
}

void GapFillDlg::OnNoFill() 
{
	int sel = m_GapList.GetCurSel();
	if(sel>-1)
	{
		if(view_gapfill_left)
		{
			Set_Left_Gap_Fill_Type(sel, GAPFILL_NO_FILL);
		}
		else if(view_gapfill_right)
		{
			Set_Right_Gap_Fill_Type(sel, GAPFILL_NO_FILL);
		}
	}
	EnableFillOptionButtons();
}

void GapFillDlg::OnInterpolateFill() 
{
	int sel = m_GapList.GetCurSel();
	if(sel>-1)
	{
		if(view_gapfill_left)
		{
			Set_Left_Gap_Fill_Type(sel, GAPFILL_INTERPOLATE_FILL);
		}
		else if(view_gapfill_right)
		{
			Set_Right_Gap_Fill_Type(sel, GAPFILL_INTERPOLATE_FILL);
		}
	}
	EnableFillOptionButtons();
}

void GapFillDlg::OnDuplicateFill() 
{
	int sel = m_GapList.GetCurSel();
	if(sel>-1)
	{
		if(view_gapfill_left)
		{
			Set_Left_Gap_Fill_Type(sel, GAPFILL_DUPLICATE_FILL);
		}
		else if(view_gapfill_right)
		{
			Set_Right_Gap_Fill_Type(sel, GAPFILL_DUPLICATE_FILL);
		}
	}
	EnableFillOptionButtons();
}

void GapFillDlg::OnBlendFill() 
{
	int sel = m_GapList.GetCurSel();
	if(sel>-1)
	{
		if(view_gapfill_left)
		{
			Set_Left_Gap_Fill_Type(sel, GAPFILL_BLEND_FILL);
		}
		else if(view_gapfill_right)
		{
			Set_Right_Gap_Fill_Type(sel, GAPFILL_BLEND_FILL);
		}
	}
	EnableFillOptionButtons();
}

void GapFillDlg::OnNoFillAll() 
{
	int n, i;
	if(view_gapfill_left){n = Num_Left_Gaps();for(i = 0;i<n;i++){Set_Left_Gap_Fill_Type(i, GAPFILL_NO_FILL);}}
	else if(view_gapfill_right){n = Num_Right_Gaps();for(i = 0;i<n;i++){Set_Right_Gap_Fill_Type(i, GAPFILL_NO_FILL);}}
	EnableFillOptionButtons();
}

void GapFillDlg::OnInterpolateAll() 
{
	int n, i;
	if(view_gapfill_left){n = Num_Left_Gaps();for(i = 0;i<n;i++){Set_Left_Gap_Fill_Type(i, GAPFILL_INTERPOLATE_FILL);}}
	else if(view_gapfill_right){n = Num_Right_Gaps();for(i = 0;i<n;i++){Set_Right_Gap_Fill_Type(i, GAPFILL_INTERPOLATE_FILL);}}
	EnableFillOptionButtons();
}

void GapFillDlg::OnDuplicateAll() 
{
	int n, i;
	if(view_gapfill_left){n = Num_Left_Gaps();for(i = 0;i<n;i++){Set_Left_Gap_Fill_Type(i, GAPFILL_DUPLICATE_FILL);}}
	else if(view_gapfill_right){n = Num_Right_Gaps();for(i = 0;i<n;i++){Set_Right_Gap_Fill_Type(i, GAPFILL_DUPLICATE_FILL);}}
	EnableFillOptionButtons();
}

void GapFillDlg::OnBlendImageAll() 
{
	int n, i;
	if(view_gapfill_left){n = Num_Left_Gaps();for(i = 0;i<n;i++){Set_Left_Gap_Fill_Type(i, GAPFILL_BLEND_FILL);}}
	else if(view_gapfill_right){n = Num_Right_Gaps();for(i = 0;i<n;i++){Set_Right_Gap_Fill_Type(i, GAPFILL_BLEND_FILL);}}
	EnableFillOptionButtons();
}



void GapFillDlg::OnClose() 
{
	Close_GapFill_Tools();
}

void GapFillDlg::OnSelchangeBlendFrameList() 
{
	int sel = m_FramesList.GetCurSel();
	int gsel = m_GapList.GetCurSel();
	if(sel>-1&&gsel>-1)
	{
		if(view_gapfill_left)
		{
			Set_Left_Gap_Blend_Frame(gsel, sel);
		}
		else if(view_gapfill_right)
		{
			Set_Right_Gap_Blend_Frame(gsel, sel);
		}
	}
}

void GapFillDlg::OnBrowseBlendImage() 
{
	char file[512];
	int gsel = m_GapList.GetCurSel();
	if(gsel>-1)
	{
		if(Browse(file, "*", false))
		{
			if(view_gapfill_left)
			{
				Set_Left_Gap_Blend_Image(gsel, file);
			}
			else if(view_gapfill_right)
			{
				Set_Right_Gap_Blend_Image(gsel, file);
			}
		}
	}
	else
	{
		SkinMsgBox("No gap selected!");
	}
}


void GapFillDlg::OnSelchangeGapList() 
{
	int sel = m_GapList.GetCurSel();
	if(sel>-1)
	{
		if(view_gapfill_left)
		{
			Select_Left_Gap(sel);
			redraw_gapfill_window = true;
			EnableFillOptionButtons();
		}
		else if(view_gapfill_right)
		{
			Select_Right_Gap(sel);
			redraw_gapfill_window = true;
			EnableFillOptionButtons();
		}
	}
}

void GapFillDlg::OnRefreshList() 
{
	if(!view_gapfill_left&&!view_gapfill_right)
	{
		SkinMsgBox("Select the left or right image first.");
		return;
	}
	Identify_Gaps();
	ListGaps();
	redraw_gapfill_window = true;
}

void GapFillDlg::OnGapfillSave() 
{
	Save_GapFill_Data();
}

void GapFillDlg::OnGapfillLeft() 
{
	View_GapFill_Left();
	ListGaps();
	Update_GapFill_Slider();
	CheckDlgButton(IDC_GAPFILL_LEFT, view_gapfill_left);
	CheckDlgButton(IDC_GAPFILL_RIGHT, view_gapfill_right);
	CheckDlgButton(IDC_GAPFILL_ANAGLYPH, view_gapfill_anaglyph);
}

void GapFillDlg::OnGapfillRight() 
{
	View_GapFill_Right();
	ListGaps();
	Update_GapFill_Slider();
	CheckDlgButton(IDC_GAPFILL_LEFT, view_gapfill_left);
	CheckDlgButton(IDC_GAPFILL_RIGHT, view_gapfill_right);
	CheckDlgButton(IDC_GAPFILL_ANAGLYPH, view_gapfill_anaglyph);
}

void GapFillDlg::OnGapfillAnaglyph() 
{
	View_GapFill_Anaglyph();
	ListGaps();
	Update_GapFill_Slider();
	CheckDlgButton(IDC_GAPFILL_LEFT, view_gapfill_left);
	CheckDlgButton(IDC_GAPFILL_RIGHT, view_gapfill_right);
	CheckDlgButton(IDC_GAPFILL_ANAGLYPH, view_gapfill_anaglyph);
}

void GapFillDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	float pos = (float)m_GapThresholdSlider.GetPos();
	float res = pos/(threshold_slider_resolution+1);
	if(view_gapfill_left)
	{
		Set_Left_Gap_Threshold(res, (int)pos);
	}
	else if(view_gapfill_right)
	{
		Set_Right_Gap_Threshold(res, (int)pos);
	}
}

void GapFillDlg::OnBackgroundColorButton() 
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK)
	{
		unsigned char rgb[3];
		COLORREF color = dlg.GetColor();
		rgb[0] = GetRValue(color);
		rgb[1] = GetGValue(color);
		rgb[2] = GetBValue(color);
		gapfill_background_color[0] = (float)rgb[0]/255;
		gapfill_background_color[1] = (float)rgb[1]/255;
		gapfill_background_color[2] = (float)rgb[2]/255;
		Set_GapFill_Context();
		glClearColor(gapfill_background_color[0],gapfill_background_color[1],gapfill_background_color[2],1);
		Set_GLContext();
		m_BackgroundColorButton.Set_Color(rgb[0], rgb[1], rgb[2]);
		CWnd *b = GetDlgItem(IDC_BACKGROUND_COLOR_BUTTON);
		b->RedrawWindow();
		redraw_gapfill_window = true;
	}
}

void GapFillDlg::OnClearGapInfo() 
{
	if(view_gapfill_left)
	{
		Free_Left_Gaps();
	}
	else if(view_gapfill_right)
	{
		Free_Right_Gaps();
	}
	redraw_gapfill_window = true;
	ListGaps();
}

void GapFillDlg::ListGaps()
{
	m_GapList.ResetContent();
	int n = 0;
	int sel = -1;
	if(view_gapfill_left)
	{
		n = Num_Left_Gaps();
		sel = Left_Gap_Selection();
	}
	else if(view_gapfill_right)
	{
		n = Num_Right_Gaps();
		sel = Right_Gap_Selection();
	}
	else
	{
		return;
	}
	char text[512];
	for(int i = 0;i<n;i++)
	{
		sprintf(text, "Gap #%i", i+1);
		m_GapList.AddString(text);
	}
	if(sel!=-1)
	{
		m_GapList.SetCurSel(sel);
	}
	EnableFillOptionButtons();
}

void GapFillDlg::OnHighlightGaps() 
{
	highlight_gaps = IsDlgButtonChecked(IDC_HIGHLIGHT_GAPS)!=0;
	redraw_gapfill_window = true;
}

void GapFillDlg::EnableBlendImageControls(bool b)
{
	m_FramesList.EnableWindow(b);
	GetDlgItem(IDC_RENDER_BACKGROUND_BLEND_IMAGE)->EnableWindow(b);
	GetDlgItem(IDC_RENDER_GHOST_BLEND_IMAGE)->EnableWindow(b);
	GetDlgItem(IDC_ALIGN_BLEND_IMAGE)->EnableWindow(b);
	GetDlgItem(IDC_APPLY_BLEND_PIXELS)->EnableWindow(b);
	GetDlgItem(IDC_CLEAR_BLEND_PIXELS)->EnableWindow(b);
	GetDlgItem(IDC_BROWSE_BLEND_IMAGE)->EnableWindow(b);
}

void GapFillDlg::EnableFillOptionButtons()
{
	int sel = -1;
	if(view_gapfill_left)
	{
		sel = Left_Gap_Selection();
	}
	else if(view_gapfill_right)
	{
		sel = Right_Gap_Selection();
	}
	bool enable = sel>-1;
	GetDlgItem(IDC_NO_FILL)->EnableWindow(enable);
	GetDlgItem(IDC_INTERPOLATE_FILL)->EnableWindow(enable);
	GetDlgItem(IDC_DUPLICATE_FILL)->EnableWindow(enable);
	GetDlgItem(IDC_BLEND_FILL)->EnableWindow(enable);
	m_FramesList.SetCurSel(-1);
	if(sel>-1)
	{
		int fill_type = GAPFILL_NO_FILL;
		if(view_gapfill_left)
		{
			fill_type = Get_Left_Gap_Fill_Type(sel);
		}
		else if(view_gapfill_right)
		{
			fill_type = Get_Right_Gap_Fill_Type(sel);
		}
		CheckDlgButton(IDC_NO_FILL, fill_type==GAPFILL_NO_FILL);
		CheckDlgButton(IDC_INTERPOLATE_FILL, fill_type==GAPFILL_INTERPOLATE_FILL);
		CheckDlgButton(IDC_DUPLICATE_FILL, fill_type==GAPFILL_DUPLICATE_FILL);
		CheckDlgButton(IDC_BLEND_FILL, fill_type==GAPFILL_BLEND_FILL);

		EnableBlendImageControls(fill_type==GAPFILL_BLEND_FILL);

		if(fill_type==GAPFILL_BLEND_FILL)
		{
			m_FramesList.EnableWindow(true);
			if(view_gapfill_left)
			{
				int frame = Get_Left_Gap_Blend_Frame(sel);
				if(frame>-1)
				{
					m_FramesList.SetCurSel(frame);
				}
			}
			if(view_gapfill_right)
			{
				int frame = Get_Right_Gap_Blend_Frame(sel);
				if(frame>-1)
				{
					m_FramesList.SetCurSel(frame);
				}
			}
		}
		else
		{
			m_FramesList.EnableWindow(false);
		}
	}
	else
	{
		EnableBlendImageControls(false);
	}
}

void GapFillDlg::OnFinalizeGapFill() 
{
	Finalize_GapFill_Images();
}

void GapFillDlg::ListFrames()
{
	m_FramesList.ResetContent();
	int n = Get_Num_Frames();
	char name[512];
	for(int i = 0;i<n;i++)
	{
		Get_Frame_Name(i, name);
		Get_Display_Name(name);
		m_FramesList.AddString(name);
	}
}

void GapFillDlg::OnRenderGhostBlendImage() 
{
	render_ghost_blend_image = IsDlgButtonChecked(IDC_RENDER_GHOST_BLEND_IMAGE)!=0;
	redraw_gapfill_window = true;
}

void GapFillDlg::OnAlignBlendImage() 
{
	Align_GapFill_Blend_Image();
	redraw_gapfill_window = true;
}

void GapFillDlg::OnApplyBlendPixels() 
{
	Apply_GapFill_Blend_Pixels();
	redraw_gapfill_window = true;
}


void GapFillDlg::OnFinalizeGapPixels() 
{
}

void GapFillDlg::OnPreviewBlendedGapFill() 
{
	Preview_Blended_Gap_Fill();
	redraw_gapfill_window = true;
	show_gaps = false;
	CheckDlgButton(IDC_SHOW_GAPS, show_gaps);
}

void GapFillDlg::OnRestoreOriginalImage() 
{
	Restore_Original_GapFill_Texture();
	redraw_gapfill_window = true;
	show_gaps = true;
	CheckDlgButton(IDC_SHOW_GAPS, show_gaps);
}

void GapFillDlg::OnShowGaps() 
{
	show_gaps = IsDlgButtonChecked(IDC_SHOW_GAPS)!=0;
	redraw_gapfill_window = true;
}

void GapFillDlg::OnPreviewFilledAnaglyph() 
{
	Preview_GapFilled_Anaglyph();
	OnGapfillAnaglyph();
}

void GapFillDlg::OnRenderBackgroundBlendImage() 
{
	render_background_blend_image = IsDlgButtonChecked(IDC_RENDER_BACKGROUND_BLEND_IMAGE)!=0;
	redraw_gapfill_window = true;
}

void GapFillDlg::OnClearBlendPixels() 
{
	Clear_GapFill_Blend_Pixels();
	redraw_gapfill_window = true;
}

HBRUSH GapFillDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX||id==IDC_STATUS)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	return hbr;
}

void GapFillDlg::OnExportLeftGapmask() 
{
	char file[512];
	if(Browse(file, "*", true))
	{
		unsigned char *rgb = new unsigned char[Frame_Width()*Frame_Height()*3];
		Get_Left_View_Gapmask(rgb);
		Save_Watermarked_Image(rgb, Frame_Width(), Frame_Height(), file);
		delete[] rgb;
		Print_To_Console("Saved image %s..", file);
		redraw_gapfill_window = true;
	}
}

void GapFillDlg::OnExportRightGapmask() 
{
	char file[512];
	if(Browse(file, "*", true))
	{
		unsigned char *rgb = new unsigned char[Frame_Width()*Frame_Height()*3];
		Get_Right_View_Gapmask(rgb);
		Save_Watermarked_Image(rgb, Frame_Width(), Frame_Height(), file);
		delete[] rgb;
		Print_To_Console("Saved image %s..", file);
		redraw_gapfill_window = true;
	}
}

void GapFillDlg::OnExportLeftFilledImage() 
{
	char file[512];
	if(Browse(file, "*", true))
	{
		unsigned char *rgb = new unsigned char[Frame_Width()*Frame_Height()*3];
		Get_Left_Blended_Gap_Fill_Image(rgb);
		Add_Lines_To_Image(rgb, Frame_Width(), Frame_Height());
		Save_Watermarked_Image(rgb, Frame_Width(), Frame_Height(), file);
		delete[] rgb;
		Print_To_Console("Saved image %s..", file);
		redraw_gapfill_window = true;
	}
}

void GapFillDlg::OnExportRightFilledImage() 
{
	char file[512];
	if(Browse(file, "*", true))
	{
		unsigned char *rgb = new unsigned char[Frame_Width()*Frame_Height()*3];
		Get_Right_Blended_Gap_Fill_Image(rgb);
		Add_Lines_To_Image(rgb, Frame_Width(), Frame_Height());
		Save_Watermarked_Image(rgb, Frame_Width(), Frame_Height(), file);
		delete[] rgb;
		Print_To_Console("Saved image %s..", file);
		redraw_gapfill_window = true;
	}
}

void GapFillDlg::OnExportFilledAnaglyph() 
{
	char file[512];
	if(Browse(file, "*", true))
	{
		unsigned char *left = new unsigned char[Frame_Width()*Frame_Height()*3];
		unsigned char *right = new unsigned char[Frame_Width()*Frame_Height()*3];
		unsigned char *rgb = new unsigned char[Frame_Width()*Frame_Height()*3];
		Get_Left_Blended_Gap_Fill_Image(left);
		Get_Right_Blended_Gap_Fill_Image(right);
		Generate_Anaglyph_Image_From_24Bit(Frame_Width()*Frame_Height(), left, right, rgb);
		Add_Lines_To_Image(rgb, Frame_Width(), Frame_Height());
		Save_Watermarked_Image(rgb, Frame_Width(), Frame_Height(), file);
		delete[] left;
		delete[] right;
		delete[] rgb;
		Print_To_Console("Saved image %s..", file);
		redraw_gapfill_window = true;
	}
}

void GapFillDlg::OnExportFilledStereo() 
{
	char file[512];
	if(Browse(file, "*", true))
	{
		unsigned char *left = new unsigned char[Frame_Width()*Frame_Height()*3];
		unsigned char *right = new unsigned char[Frame_Width()*Frame_Height()*3];
		unsigned char *rgb = new unsigned char[Frame_Width()*Frame_Height()*6];
		Get_Left_Blended_Gap_Fill_Image(left);
		Get_Right_Blended_Gap_Fill_Image(right);
		Add_Lines_To_Image(left, Frame_Width(), Frame_Height());
		Add_Lines_To_Image(right, Frame_Width(), Frame_Height());
		int w = Frame_Width();
		int h = Frame_Height();
		for(int i = 0;i<h;i++)
		{
			memcpy(&rgb[(i*w*2*3)], &left[i*w*3], sizeof(unsigned char)*w*3);
			memcpy(&rgb[(i*w*2*3)+(w*3)], &right[i*w*3], sizeof(unsigned char)*w*3);
		}
		Save_Stereo_Watermarked_Image(rgb, Frame_Width()*2, Frame_Height(), file);
		delete[] left;
		delete[] right;
		delete[] rgb;
		Print_To_Console("Saved image %s..", file);
		redraw_gapfill_window = true;
	}
}
