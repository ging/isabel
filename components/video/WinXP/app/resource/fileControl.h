/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
#if !defined(AFX_FILECONTROL_H__E60D95BB_3E18_464A_9BE2_84B879483F16__INCLUDED_)
#define AFX_FILECONTROL_H__E60D95BB_3E18_464A_9BE2_84B879483F16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// fileControl.h : header file
//

#include <afxwin.h>
#include "resource.h"
#include "../general.h"
#include <AFXCMN.H>

/////////////////////////////////////////////////////////////////////////////
// fileControl dialog

class fileControl : public CDialog
{
// Construction
public:
	fileControl(HINSTANCE hInstance = NULL,CWnd* pParent = NULL);   // standard constructor
   
// Dialog Data
	//{{AFX_DATA(fileControl)
	enum { IDD = IDD_FORMVIEW };
	CSliderCtrl	m_slide;
	CButton	m_cmdStop;
	CButton	m_cmdPlay;
	CButton	m_cmdPause;
	//}}AFX_DATA

    bool slideMoved;
    bool play;
    bool event;

    void setGeometry(windowInfo_t);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(fileControl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    
	// Generated message map functions
	//{{AFX_MSG(fileControl)
	afx_msg void OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPause();
	afx_msg void OnPlay();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILECONTROL_H__E60D95BB_3E18_464A_9BE2_84B879483F16__INCLUDED_)
