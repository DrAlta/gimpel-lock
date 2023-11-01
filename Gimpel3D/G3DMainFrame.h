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
#if !defined(AFX_G3DMAINFRAME_H__963B3401_26CB_4FE7_AD13_CCD7B677046D__INCLUDED_)
#define AFX_G3DMAINFRAME_H__963B3401_26CB_4FE7_AD13_CCD7B677046D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// G3DMainFrame.h : header file
//

#include "MenuToolTips.h"

/////////////////////////////////////////////////////////////////////////////
// G3DMainFrame frame

class G3DMainFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(G3DMainFrame)
//protected:
	G3DMainFrame();           // protected constructor used by dynamic creation
	bool CheckSaveSession();
	void SetProjectTitle();
	HICON m_hIcon;
	CMenuTipManager m_menuTipManager;

// Attributes
public:

// Operations
public:
//	BOOL OnMeasureItem(int nIDCtl,MEASUREITEMSTRUCT*pMIS);
//	BOOL OnDrawItem(int nIDCtl,DRAWITEMSTRUCT*pDIS);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(G3DMainFrame)
	//}}AFX_VIRTUAL

// Implementation
//protected:
	virtual ~G3DMainFrame();

	// Generated message map functions
	//{{AFX_MSG(G3DMainFrame)
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLoadProject();
	afx_msg void OnNewProject();
	afx_msg void OnSaveProject();
	afx_msg void OnExit();
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg void OnSaveSession();
	afx_msg void OnReplaceImage();
	afx_msg void OnLoadSession();
	afx_msg void OnLoadFrame();
	afx_msg void OnImportLayers();
	afx_msg void OnImportDepthMask();
	afx_msg void OnExportLayerMask();
	afx_msg void OnExportDepthMask();
	afx_msg void OnImportStereoSettings();
	afx_msg void OnFlipDepthMaskDepth();
	afx_msg void OnFlipDepthMaskHorizontal();
	afx_msg void OnFlipDepthMaskVertical();
	afx_msg void OnFlipImageHorizontal();
	afx_msg void OnFlipLayersHorizontal();
	afx_msg void OnFlipLayersVertical();
	afx_msg void OnFlipRedBlue();
	afx_msg void OnFlipImageVertical();
	afx_msg void OnDualMonitor();
	afx_msg void OnSingleMonitor();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSwapMonitors();
	afx_msg void OnFullScreen2d();
	afx_msg void OnFullScreen3d();
	afx_msg void OnWideScreen3d();
	afx_msg void OnSplitScreen();
	afx_msg void OnAnaglyph();
	afx_msg void OnStereoSettings();
	afx_msg void On3dViewOptions();
	afx_msg void OnSelectAll();
	afx_msg void OnUnselectLayers();
	afx_msg void OnRenameSelection();
	afx_msg void OnMergeLayers();
	afx_msg void OnDeleteLayers();
	afx_msg void OnEvaporateLayers();
	afx_msg void OnSplitLayer();
	afx_msg void OnOpenDepthSlider();
	afx_msg void OnOpenScaleSlider();
	afx_msg void OnPlanarProjection();
	afx_msg void OnContourExtrusion();
	afx_msg void OnReliefMap();
	afx_msg void OnAutoAlignment();
	afx_msg void On2dViewOptions();
	afx_msg void OnLayerAllFrames();
	afx_msg void OnLayerSingleFrame();
	afx_msg void OnKeyCommands();
	afx_msg void OnShiftEdges();
	afx_msg void OnOpenPostEditor();
	afx_msg void OnOpenGapfillEditor();
	afx_msg void OnApplyGeometry();
	afx_msg void OnAutoSegmentImage();
	afx_msg void OnHelpReadmeFirst();
	afx_msg void OnHelpSingleImage();
	afx_msg void OnHelpSequence();
	afx_msg void OnHelpRotoscoping();
	afx_msg void OnHelpGapfill();
	afx_msg void OnHelpStereoPreview();
	afx_msg void OnHelp3dEditing();
	afx_msg void OnHelpImportingData();
	afx_msg void OnHelpExportingData();
	afx_msg void OnHelpToolsReference();
	afx_msg void OnShowHelp();
	afx_msg void OnGetPerspectiveProjection();
	afx_msg void OnOpenShapesTool();
	afx_msg void OnOpenAnchorPointsTool();
	afx_msg void OnOpenVirtualSetTool();
	afx_msg void OnFeaturePointsTool();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnFullScreenApp();
	afx_msg void OnTrackOutline();
	afx_msg void OnColorScheme();
	afx_msg void OnPreviewTool();
	afx_msg void OnCopyEditsAllFrames();
	afx_msg void OnCopyEditsSingleFrame();
	afx_msg void OnOpenVirtualCameraTool();
	afx_msg void OnGridButton();
	afx_msg void OnCameraButton();
	afx_msg void OnPaintTool();
	afx_msg void OnPlugins();
	afx_msg void OnTouchupsTool();
	afx_msg void OnHaloTool();
	afx_msg void OnImportLayerEdits();
	afx_msg void OnHiResPerformance();
	afx_msg void OnExportBackgroundMesh();
	afx_msg void OnImportBackgroundMesh();
	afx_msg void OnExportLeftGapMask();
	afx_msg void OnExportRightGapMask();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern G3DMainFrame *mainframe;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_G3DMainFrame_H__963B3401_26CB_4FE7_AD13_CCD7B677046D__INCLUDED_)
