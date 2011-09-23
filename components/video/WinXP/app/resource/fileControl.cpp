/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
// fileControl.cpp : implementation file
//

#include "filecontrol.h"
#include "fileControl.h"
#include "../main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// fileControl dialog


fileControl::fileControl(HINSTANCE hInstance,CWnd* pParent /*=NULL*/)
	: CDialog(fileControl::IDD, pParent)
{
    Create(IDD);

    HICON iconPlay = LoadIcon(videoInstance,MAKEINTRESOURCE(IDI_PLAY));
    HICON iconPause = LoadIcon(videoInstance,MAKEINTRESOURCE(IDI_PAUSE));
    HICON iconStop = LoadIcon(videoInstance,MAKEINTRESOURCE(IDI_STOP));
   
    m_cmdPlay.SetIcon(iconPlay);
    m_cmdPause.SetIcon(iconPause);
    m_cmdStop.SetIcon(iconStop);
    
    slideMoved = false;
    play = false;
    event = false;


	//{{AFX_DATA_INIT(fileControl)
	//}}AFX_DATA_INIT
}

void 
fileControl::setGeometry(windowInfo_t windowInfo)
{
    SetWindowPos(NULL,
                 windowInfo.left, 
                 windowInfo.top + windowInfo.heigth,
                 windowInfo.width, 
                 22, 
                 SWP_NOZORDER);

    m_slide.SetWindowPos(NULL,0,0,windowInfo.width-58,20,SWP_NOZORDER);
    m_cmdPlay.SetWindowPos(NULL,windowInfo.width-54,2,16,16,SWP_NOZORDER);
    m_cmdStop.SetWindowPos(NULL,windowInfo.width-37,2,16,16,SWP_NOZORDER);
    m_cmdPause.SetWindowPos(NULL,windowInfo.width-20,2,16,16,SWP_NOZORDER);

    m_slide.RedrawWindow();
    m_cmdPlay.RedrawWindow();
    m_cmdStop.RedrawWindow();
    m_cmdPause.RedrawWindow();
    CWnd::RedrawWindow();
    ShowWindow(SW_SHOW);
}


void 
fileControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(fileControl)
	DDX_Control(pDX, IDC_SLIDER1, m_slide);
	DDX_Control(pDX, IDC_STOP, m_cmdStop);
	DDX_Control(pDX, IDC_PLAY, m_cmdPlay);
	DDX_Control(pDX, IDC_PAUSE, m_cmdPause);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(fileControl, CDialog)
	//{{AFX_MSG_MAP(fileControl)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnReleasedcaptureSlider1)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void fileControl::OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	slideMoved = true;
	*pResult = 0;
}

void fileControl::OnPause() 
{
    play  = false;
    event = true;
	
}

void fileControl::OnPlay() 
{
	play = true;
    event = true;
	
}

void fileControl::OnStop() 
{
    play  = false;
    event = true;
    m_slide.SetPos(0);
    slideMoved = true;
}
