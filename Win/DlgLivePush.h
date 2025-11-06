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
#ifndef _C_DLG_LIVE_PUSH_H_
#define _C_DLG_LIVE_PUSH_H_


#include "Resource.h"
//#include "RtmpHoster.h"
#include "DlgVideo.h"


#include "libcross_platform_collection_render/video_render/cvideo_render_factory.h"
#include "libcross_platform_collection_render/video_render/cvideo_render.h"
#include "libcross_platform_collection_render/track_capture/ctrack_capture.h"
#include "media/crtc_media_sink.h"
#include "libmedia_codec/x264_encoder.h"
#include "rtc_base/third_party/sigslot/sigslot.h"
#include "libp2p_peerconnection/cp2p_peerconnection.h"
#include "libcross_platform_collection_render/audio_capture/audio_capture.h"
#include "media/crtc_media_sink.h"
#include "libmedia_codec/audio_codec/opus_encoder.h"
#include <map>
#define WM_MY_PUSH_MESSAGE (WM_USER + 101)

// DlgVideo 对话框
class CLiveWin32Dlg;
class DlgLivePush : public CDialog, public sigslot::has_slots<>//, public RTMPHosterEvent
{
	DECLARE_DYNAMIC(DlgLivePush)

public:
	DlgLivePush();   // 标准构造函数
	virtual ~DlgLivePush();

// 对话框数据
	enum { IDD = IDD_DIALOG_PUSH };

protected:
	//* For RTMPHosterEvent
	virtual void OnRtmpStreamOK() {};
	virtual void OnRtmpStreamReconnecting(int times) {};
	virtual void OnRtmpStreamStatus(int delayMs, int netBand) {};
	virtual void OnRtmpStreamFailed(int code) {};
	virtual void OnRtmpStreamClosed() {};

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
	virtual void OnTimer(UINT nIDEvent);

	CEdit	m_editUrl;
	CButton	m_btnRtmp;

	CString m_strUrl;

	//CEdit	m_editUserName;
//	CString m_strUserName;
	
	//CEdit	m_editStreamName;
	//CString m_strStreamName;
	CComboBox  m_comboxType;
	CString m_strAudioDeviceType;


	CComboBox  m_comboxcaptureType;
	CString m_strCaptureType;
	CStatic m_staticCaptrue;
	afx_msg void OnBnClickedBtnPush();
public:


	void OnTragetTransferRate(crtc::CRTCMediaSink*, const libice::TargetTransferRate& target);

protected:
	DlgVideo		*m_pDlgVideoMain;
	//RTMPHoster		*m_pAVRtmpstreamer;
public:
	//afx_msg void OnStnClickedStaticUrl();
	afx_msg void OnBnClickedAudioVideo();


private:

	libcross_platform_collection_render::cvideo_render_factory*   video_render_factory_;
	libcross_platform_collection_render::cvideo_renderer*         video_renderer_;

	rtc::scoped_refptr<libcross_platform_collection_render::CapturerTrackSource>       capture_track_source_;


	crtc::CRTCMediaSink *                         crtc_media_sink_;

	 libmedia_codec::X264Encoder *								   x264_encoder_;
	 libcross_platform_collection_render::AudioCapture* audio_capture_;
	 libmedia_codec::OpusEncoder2 *                     opus_encoder2_;
	 std::map<std::string, std::string>           audio_gurild_;
public:
	//afx_msg void OnEnChangeStreamName();
//	afx_msg void OnCbnSelchangeCombo1();
	//afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnCbnSelchangeCombo2();
};

#endif // _C_DLG_LIVE_PUSH_H_