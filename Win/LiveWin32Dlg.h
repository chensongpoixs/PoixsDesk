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
#ifndef _C_LIVE_WIN32_DLG_H_
#define _C_LIVE_WIN32_DLG_H_
#include "Resource.h"

// CLiveWin32Dlg 对话框
class CLiveWin32Dlg : public CDialogEx
{
// 构造
public:
	CLiveWin32Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LIVEWIN32_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL DestroyWindow();
	
	afx_msg void OnBnClickedBtnPush();
	afx_msg void OnBnClickedBtnPull();
	afx_msg void OnBnClickedBtnRtcp();
};


#endif // _C_LIVE_WIN32_DLG_H_