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
// PluginsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "PluginsDlg.h"
#include "Plugins.h"
#include "Skin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PluginsDlg dialog

PluginsDlg *pluginsDlg = 0;

PluginsDlg::PluginsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PluginsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PluginsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_UnloadPlugin;
	m_UnloadAllPlugin;
	m_ReloadPlugin;
	m_ReloadAllPlugins;
	m_OpenPlugin;
	m_ClosePlugins;
	m_BrowseForPlugin;
	m_PluginList;
}


void PluginsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PluginsDlg)
	DDX_Control(pDX, IDC_UNLOAD_PLUGIN, m_UnloadPlugin);
	DDX_Control(pDX, IDC_UNLOAD_ALL_PLUGIN, m_UnloadAllPlugin);
	DDX_Control(pDX, IDC_RELOAD_PLUGIN, m_ReloadPlugin);
	DDX_Control(pDX, IDC_RELOAD_ALL_PLUGINS, m_ReloadAllPlugins);
	DDX_Control(pDX, IDC_OPEN_PLUGIN, m_OpenPlugin);
	DDX_Control(pDX, IDC_CLOSE_PLUGINS, m_ClosePlugins);
	DDX_Control(pDX, IDC_BROWSE_FOR_PLUGIN, m_BrowseForPlugin);
	DDX_Control(pDX, IDC_PLUGIN_LIST, m_PluginList);
	//}}AFX_DATA_MAP
	m_UnloadPlugin.SetFont(skin_font);
	m_UnloadAllPlugin.SetFont(skin_font);
	m_ReloadPlugin.SetFont(skin_font);
	m_ReloadAllPlugins.SetFont(skin_font);
	m_OpenPlugin.SetFont(skin_font);
	m_ClosePlugins.SetFont(skin_font);
	m_BrowseForPlugin.SetFont(skin_font);
	m_PluginList.SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(PluginsDlg, CDialog)
	//{{AFX_MSG_MAP(PluginsDlg)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_OPEN_PLUGIN, OnOpenPlugin)
	ON_BN_CLICKED(IDC_CLOSE_PLUGINS, OnClosePlugins)
	ON_BN_CLICKED(IDC_RELOAD_PLUGIN, OnReloadPlugin)
	ON_BN_CLICKED(IDC_RELOAD_ALL_PLUGINS, OnReloadAllPlugins)
	ON_BN_CLICKED(IDC_UNLOAD_PLUGIN, OnUnloadPlugin)
	ON_BN_CLICKED(IDC_UNLOAD_ALL_PLUGIN, OnUnloadAllPlugin)
	ON_BN_CLICKED(IDC_BROWSE_FOR_PLUGIN, OnBrowseForPlugin)
	ON_LBN_DBLCLK(IDC_PLUGIN_LIST, OnDblclkPluginList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PluginsDlg message handlers


bool Open_Plugins_Dialog()
{
	if(!pluginsDlg)
	{
		pluginsDlg = new PluginsDlg;
		pluginsDlg->Create(IDD_PLUGINS);
		pluginsDlg->SetWindowText("Plugins");
		Create_ToolSkin_Frame(pluginsDlg, "Plugins", IDC_PLUGINS);
	}
	pluginsDlg->GetParent()->ShowWindow(SW_SHOW);
	pluginsDlg->ListPlugins();
	return true;
}

bool Close_Plugins_Dialog()
{
	if(!pluginsDlg)
	{
		return false;
	}
	pluginsDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}


void PluginsDlg::OnClose() 
{
	Close_Plugins_Tool();
}

HBRUSH PluginsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
}

void PluginsDlg::ListPlugins()
{
	m_PluginList.ResetContent();
	int n = Num_Plugins();
	char name[512];
	for(int i = 0;i<n;i++)
	{
		if(Get_Plugin_Name(i, name))
		{
			m_PluginList.AddString(name);
		}
	}
}

void PluginsDlg::OnOpenPlugin() 
{
	int sel = m_PluginList.GetCurSel();
	if(sel!=-1)
	{
		char text[512];
		m_PluginList.GetText(sel, text);
		Open_Plugin(text);
	}
}

void PluginsDlg::OnClosePlugins() 
{
	Close_All_Plugins();
}

void PluginsDlg::OnReloadPlugin() 
{
	int sel = m_PluginList.GetCurSel();
	if(sel!=-1)
	{
		char text[512];
		m_PluginList.GetText(sel, text);
		Reload_Plugin(text);
	}
}

void PluginsDlg::OnReloadAllPlugins() 
{
	Reload_All_Plugins();
}

void PluginsDlg::OnUnloadPlugin() 
{
	int sel = m_PluginList.GetCurSel();
	if(sel!=-1)
	{
		char text[512];
		m_PluginList.GetText(sel, text);
		Unload_Plugin(text);
		ListPlugins();
	}
}

void PluginsDlg::OnUnloadAllPlugin() 
{
	Unload_All_Plugins();
	ListPlugins();
}

void PluginsDlg::OnBrowseForPlugin() 
{
	char file[512];
	if(Browse(file, "dll", false))
	{
		Load_Custom_Plugin(file);
		ListPlugins();
	}
}

void PluginsDlg::OnDblclkPluginList() 
{
	OnOpenPlugin();
}
