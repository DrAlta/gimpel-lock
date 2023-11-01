// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#define _WIN32_WINNT 0x0400
#if !defined(AFX_STDAFX_H__8DF014E0_C53E_4365_8BF9_F53D74A8380F__INCLUDED_)
#define AFX_STDAFX_H__8DF014E0_C53E_4365_8BF9_F53D74A8380F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

bool Quit();
void Print_Status(const char *fmt, ...);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_STDAFX_H__8DF014E0_C53E_4365_8BF9_F53D74A8380F__INCLUDED_)
