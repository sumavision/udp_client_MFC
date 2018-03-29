
// clientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


typedef enum
{
	SOCK_TCP = 0,
	SOCK_UDP
}SOCK_Protocol;

struct data
{
	SOCKET socket;
	int index;
	CString ListName;
	int protocol;
	sockaddr ServerAddr;
	int ServerAddrLen;
//	DWORD dwThreadID;
};
// CclientDlg �Ի���
class CclientDlg : public CDialogEx
{
// ����
public:
	CclientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int index;
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CIPAddressCtrl m_IP;
	CEdit m_Port;
	CEdit m_SndCount;
	CEdit m_IntervalTime;
	CListBox m_list;
	CEdit send_msg;
	CEdit m_message;
	CStatic m_showname;
	CButton m_TcpCheck;
	CButton m_UdpCheck;
	
	static int threadcount;
	static int RecvCount;
	static int SndCount;
	static DWORD WINAPI GetServerData(LPVOID lpParam);
	
	//static int SockTcpRecv(SOCKET       Sock_Fd,char *buff,int len);
	//static int SockTcpSend(SOCKET       Sock_Fd,char *buff,int len);
	//static int SockUdpRecv(SOCKET       Sock_Fd,char *buff,int len);
	//static int SockUdpSend(SOCKET       Sock_Fd,char *buff,int len);
	afx_msg SOCKET SockTcpStart(data *d);
	afx_msg SOCKET SockUdpStart();
	afx_msg void GetServerAddr(data *d);
	afx_msg int DisplayCurrConnectAddr(data *d);
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnLbnSelchangeListClient();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonSnd();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonClearServer();
	afx_msg void OnBnClickedButtonClearClnt();
};
