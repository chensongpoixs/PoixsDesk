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
#ifndef _C_DLG_LIVE_PULL_H_
#define _C_DLG_LIVE_PULL_H_

#include "Resource.h"
//#include "RtmpGuester.h"
#include "DlgVideo.h"
 

#define WM_MY_PULL_MESSAGE (WM_USER + 102)

// DlgVideo 对话框
class CLiveWin32Dlg;
class DlgLivePull : public CDialog //, public RTMPGuesterEvent
{
	DECLARE_DYNAMIC(DlgLivePull)

public:
	DlgLivePull();   // 标准构造函数
	virtual ~DlgLivePull();

// 对话框数据
	enum { IDD = IDD_DIALOG_PULL };

public:
	//* For RTMPCGuesterEvent
	virtual void OnRtmplayerOK() {};
	virtual void OnRtmplayerStatus(int cacheTime, int curBitrate) {};
	virtual void OnRtmplayerCache(int time) {};
	virtual void OnRtmplayerClosed(int errcode) {};


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);

	CEdit	m_editUrl;
	CButton m_btnLive;
	CString m_strUrl;
	CStatic m_staticCaptrue;

	afx_msg void OnBnClickedBtnPull();

private:
	DlgVideo		*m_pDlgVideoMain;
	//RTMPGuester		*m_pAVRtmplayer;
	 


public:
	//afx_msg void OnStnClickedStaticUrl();
};


#endif // _C_DLG_LIVE_PULL_H_