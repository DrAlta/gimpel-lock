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
#if !defined(AFX_PLUGINSDLG_H__4A5E57E1_33AD_437B_9062_2F23EAC7EE7B__INCLUDED_)
#define AFX_PLUGINSDLG_H__4A5E57E1_33AD_437B_9062_2F23EAC7EE7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PluginsDlg.h : header file
//


#include "Skin.h"
/////////////////////////////////////////////////////////////////////////////
// PluginsDlg dialog

class PluginsDlg : public CDialog
{
// Construction
public:
	PluginsDlg(CWnd* pParent = NULL);   // standard constructor
	void ListPlugins();

// Dialog Data
	//{{AFX_DATA(PluginsDlg)
	enum { IDD = IDD_PLUGINS };
	CSkinButton	m_UnloadPlugin;
	CSkinButton	m_UnloadAllPlugin;
	CSkinButton	m_ReloadPlugin;
	CSkinButton	m_ReloadAllPlugins;
	CSkinButton	m_OpenPlugin;
	CSkinButton	m_ClosePlugins;
	CSkinButton	m_BrowseForPlugin;
	CListBox	m_PluginList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PluginsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PluginsDlg)
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnOpenPlugin();
	afx_msg void OnClosePlugins();
	afx_msg void OnReloadPlugin();
	afx_msg void OnReloadAllPlugins();
	afx_msg void OnUnloadPlugin();
	afx_msg void OnUnloadAllPlugin();
	afx_msg void OnBrowseForPlugin();
	afx_msg void OnDblclkPluginList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINSDLG_H__4A5E57E1_33AD_437B_9062_2F23EAC7EE7B__INCLUDED_)
