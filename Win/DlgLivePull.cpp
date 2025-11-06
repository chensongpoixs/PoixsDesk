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
#include "DlgLivePull.h" 
// DlgLivePull 对话框
#include <thread>
IMPLEMENT_DYNAMIC(DlgLivePull, CDialog)

DlgLivePull::DlgLivePull()
	: CDialog(DlgLivePull::IDD)
	, m_strUrl(_T("rtsp://admin:hik@12345@192.168.1.64/streaming/channels/101"))
	//, m_pAVRtmplayer(NULL)
	, m_pDlgVideoMain(NULL) 
{
}

DlgLivePull::~DlgLivePull()
{
}

void DlgLivePull::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_URL, m_editUrl);
	DDX_Control(pDX, IDC_BTN_PULL, m_btnLive);
	DDX_Text(pDX, IDC_EDIT_URL, m_strUrl);
	DDX_Control(pDX, IDC_STATIC_CAPTRUE, m_staticCaptrue);
}


BEGIN_MESSAGE_MAP(DlgLivePull, CDialog)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_MY_PULL_MESSAGE, OnMyMessage)
	ON_BN_CLICKED(IDC_BTN_PULL, &DlgLivePull::OnBnClickedBtnPull)
//	ON_STN_CLICKED(IDC_STATIC_URL, &DlgLivePull::OnStnClickedStaticUrl)
END_MESSAGE_MAP()


// DlgLivePull 消息处理程序

void DlgLivePull::OnOK()
{
}

void DlgLivePull::OnCancel()
{
	CDialog::EndDialog(0);
}

void DlgLivePull::OnClose()
{
	CDialog::EndDialog(0);
}

BOOL DlgLivePull::OnInitDialog()
{
	CDialog::OnInitDialog();

	{// Video player
		m_pDlgVideoMain = new DlgVideo(this);
		m_pDlgVideoMain->Create(DlgVideo::IDD, this);
		CRect rc;
		m_staticCaptrue.GetWindowRect(rc);
		m_staticCaptrue.ShowWindow(SW_HIDE);
		ScreenToClient(rc);
		m_pDlgVideoMain->SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL DlgLivePull::DestroyWindow()
{
	//if (m_pAVRtmplayer) {
	//	m_pAVRtmplayer->StopRtmpPlay();
	//	RTMPGuester::Destory(m_pAVRtmplayer);
	//	m_pAVRtmplayer = NULL;
	//}
	if (m_pDlgVideoMain) {
		m_pDlgVideoMain->DestroyWindow();
		delete m_pDlgVideoMain;
		m_pDlgVideoMain = NULL;
	}

	return CDialog::DestroyWindow();
}

BOOL DlgLivePull::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

LRESULT DlgLivePull::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	CString *pstrGet = (CString *)lParam;
	char ss[128];
	memset(ss, 0, 128);
	int fnlen = pstrGet->GetLength();
	for (int i = 0; i <= fnlen; i++) {
		ss[i] = pstrGet->GetAt(i);
	}
	DlgVideo* ptrDlg = NULL;
	delete pstrGet;
	return 0;
}

void DlgLivePull::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDblClk(nFlags, point);
}


#if 0

#include <winsock2.h>
//#include <ws2tcp.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

#define DATA_BUFSIZE 8192

typedef struct _SOCKET_INFORMATION {
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	WSAOVERLAPPED Overlapped;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

int main____connect() {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	WSAEVENT EventArray[1];
	DWORD EventTotal = 0, Index;
	LPSOCKET_INFORMATION SocketInfo;

	// 初始化Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed: %d\n", WSAGetLastError());
		return 1;
	}

	// 创建事件对象
	EventArray[0] = WSACreateEvent();
	if (EventArray[0] == WSA_INVALID_EVENT) {
		printf("WSACreateEvent failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// 创建TCP套接字
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("socket failed: %d\n", WSAGetLastError());
		WSACloseEvent(EventArray[0]);
		WSACleanup();
		return 1;
	}

	// 设置服务器地址
	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr("192.168.1.64");
	ServerAddr.sin_port = htons(544);

	// 注册网络事件
	if (WSAEventSelect(ConnectSocket, EventArray[0], FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR) {
		printf("WSAEventSelect failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACloseEvent(EventArray[0]);
		WSACleanup();
		return 1;
	}

	// 发起连接
	if (connect(ConnectSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			printf("connect failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACloseEvent(EventArray[0]);
			WSACleanup();
			return 1;
		}
	}

	// 分配套接字信息结构
	SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION));
	SocketInfo->Socket = ConnectSocket;
	SocketInfo->BytesSEND = 0;
	SocketInfo->BytesRECV = 0;

	// 主事件循环
	while (TRUE) {
		Index = WSAWaitForMultipleEvents(EventTotal + 1, EventArray, FALSE, WSA_INFINITE, FALSE);
		if (Index == WSA_WAIT_FAILED) {
			printf("WSAWaitForMultipleEvents failed: %d\n", WSAGetLastError());
			break;
		}

		Index = Index - WSA_WAIT_EVENT_0;
		WSANETWORKEVENTS NetworkEvents;
		if (WSAEnumNetworkEvents(ConnectSocket, EventArray[Index], &NetworkEvents) == SOCKET_ERROR) {
			printf("WSAEnumNetworkEvents failed: %d\n", WSAGetLastError());
			break;
		}

		// 处理连接完成事件
		if (NetworkEvents.lNetworkEvents & FD_CONNECT) {
			if (NetworkEvents.iErrorCode[FD_CONNECT_BIT] != 0) {
				printf("FD_CONNECT failed: %d\n", NetworkEvents.iErrorCode[FD_CONNECT_BIT]);
				break;
			}
			printf("Connection established\n");
			// 发送测试数据
			const char* sendbuf = "Hello from client";
			if (send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0) == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				break;
			}
		}

		// 处理可读事件
		if (NetworkEvents.lNetworkEvents & FD_READ) {
			if (NetworkEvents.iErrorCode[FD_READ_BIT] != 0) {
				printf("FD_READ failed: %d\n", NetworkEvents.iErrorCode[FD_READ_BIT]);
				break;
			}

			int recvResult = recv(ConnectSocket, SocketInfo->Buffer, DATA_BUFSIZE, 0);
			if (recvResult > 0) {
				printf("Received %d bytes: %.*s\n", recvResult, recvResult, SocketInfo->Buffer);
			}
			else if (recvResult == 0) {
				printf("Connection closed\n");
				break;
			}
			else {
				printf("recv failed: %d\n", WSAGetLastError());
				break;
			}
		}

		// 处理关闭事件
		if (NetworkEvents.lNetworkEvents & FD_CLOSE) {
			if (NetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0) {
				printf("FD_CLOSE failed: %d\n", NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
			}
			printf("Server disconnected\n");
			break;
		}
	}

	// 清理资源
	GlobalFree(SocketInfo);
	closesocket(ConnectSocket);
	WSACloseEvent(EventArray[0]);
	WSACleanup();
	return 0;
}
#endif //


void DlgLivePull::OnBnClickedBtnPull()
{
	// TODO:  在此添加控件通知处理程序代码
	//if (m_pAVRtmplayer == NULL) {
	//	m_pAVRtmplayer = RTMPGuester::Create(*this);
	//	UpdateData(TRUE);
	//	char ss[128];
	//	memset(ss, 0, 128);
	//	int fnlen = m_strUrl.GetLength();
	//	for (int i = 0; i <= fnlen; i++) {
	//		ss[i] = m_strUrl.GetAt(i);
	//	}
	//	m_pAVRtmplayer->StartRtmpPlay(ss, m_pDlgVideoMain->m_hWnd);
	//	m_btnRtmp.SetWindowTextW(L"结束");
	//}
	//else 

//#define GB28181 1

	//static libmedia_transfer_protocol::libsip::SipServer sip_server_;
	//sip_server_.Start();
#if 0
		
	rtsp_client_.network_thread()->PostTask(RTC_FROM_HERE, [this]() {
			char ss[1024];
			memset(ss, 0, 1024);
			GetDlgItem(IDC_EDIT_URL)->GetWindowText(m_strUrl);
			int fnlen = m_strUrl.GetLength();
			for (int i = 0; i <= fnlen; i++)
			{
				ss[i] = m_strUrl.GetAt(i);
			}
			CRect rc;
			m_staticCaptrue.GetWindowRect(rc);
			video_renderer_ = libcross_platform_collection_render::cvideo_renderer::Create(m_pDlgVideoMain->m_hWnd, rc.Width(), rc.Height(), nullptr);
			rtsp_client_.RegisterDecodeCompleteCallback(video_renderer_);
			rtsp_client_.Open(ss);
		});
 
#else 


	/*gb28181_server_.network_thread()->PostTask(RTC_FROM_HERE, [this]() {
		CRect rc;
		m_staticCaptrue.GetWindowRect(rc);
		video_renderer_ = libcross_platform_collection_render::cvideo_renderer::Create(m_pDlgVideoMain->m_hWnd, rc.Width(), rc.Height(), nullptr);
		gb28181_server_.RegisterDecodeCompleteCallback(video_renderer_);
		gb28181_server_.Startup("192.168.1.2", 10000);
	});*/


	//std::thread([]()
	//{
	//
	//	main____connect();
	//}).detach();
#endif ///	

		
	{
		m_btnLive.SetWindowText("拉流");
		//m_pAVRtmplayer->StopRtmpPlay();
		//RTMPGuester::Destory(m_pAVRtmplayer);
		//m_pAVRtmplayer = NULL;
	}
}



