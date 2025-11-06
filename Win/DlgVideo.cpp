/*
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
 /*****************************************************************************
				   Author: chensong
				   date:  2025-09-17



 ******************************************************************************/
#include "stdafx.h"
#include "DlgVideo.h"

// DlgVideo 对话框

IMPLEMENT_DYNAMIC(DlgVideo, CDialog)

DlgVideo::DlgVideo(CWnd* pParentWnd)
	: CDialog(DlgVideo::IDD, pParentWnd)
{
}

DlgVideo::~DlgVideo()
{
}

void DlgVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgVideo, CDialog)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// DlgVideo 消息处理程序

void DlgVideo::OnOK()
{
}

void DlgVideo::OnCancel()
{
}

void DlgVideo::OnClose()
{
}

BOOL DlgVideo::OnInitDialog()
{
	CDialog::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgVideo::OnPaint() {
}

BOOL DlgVideo::DestroyWindow()
{

	return CDialog::DestroyWindow();
}

BOOL DlgVideo::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void DlgVideo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//m_rDlgMain.OnFullScreen(this);

	CDialog::OnLButtonDblClk(nFlags, point);
}
