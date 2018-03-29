
// clientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "client.h"
#include "clientDlg.h"
#include "afxdialogex.h"
#include "ctrl_frame.h"
#include <share.h>

#define MAXSIZE 1024//����д����

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CclientDlg SockClnt;
sockaddr ServerAddr;
// CclientDlg �Ի���
int ServerAddrLen = sizeof (sockaddr);
int protocol = SOCK_UDP;

//Exit :1;Close:0
int ExitCloseFlag = -1;

//ģ�ⷢ�Ͱ��� :1;�ֶ����Ͱ���:0
int SndAutoFlag = -1;

int CclientDlg::threadcount = 0;
int CclientDlg::SndCount = 0;
int CclientDlg::RecvCount = 0;

CclientDlg::CclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CclientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_SERVER, m_IP);
	DDX_Control(pDX, IDC_EDIT_PORT, m_Port);
	DDX_Control(pDX, IDC_LIST_CLIENT, m_list);
	//  DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_MESSAGEs, m_message);
	DDX_Control(pDX, IDC_STATIC_NAME, m_showname);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, send_msg);

	DDX_Control(pDX, IDC_RADIO_TCP, m_TcpCheck);
	DDX_Control(pDX, IDC_RADIO_UDP, m_UdpCheck);

	DDX_Control(pDX, IDC_EDIT_SND_COUNT, m_SndCount);
	DDX_Control(pDX, IDC_EDIT_INTERVAL_TIME, m_IntervalTime);
}

BEGIN_MESSAGE_MAP(CclientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CclientDlg::OnBnClickedButtonConnect)
	ON_LBN_SELCHANGE(IDC_LIST_CLIENT, &CclientDlg::OnLbnSelchangeListClient)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CclientDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_SND, &CclientDlg::OnBnClickedButtonSnd)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CclientDlg::OnBnClickedButtonExit)
	//ON_STN_CLICKED(IDC_STATIC_NAME, &CclientDlg::OnStnClickedStaticName)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SERVER, &CclientDlg::OnBnClickedButtonClearServer)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_CLNT, &CclientDlg::OnBnClickedButtonClearClnt)
END_MESSAGE_MAP()


// CclientDlg ��Ϣ�������

BOOL CclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_TcpCheck.SetCheck(0);         //Ĭ��tcp�ر�
	m_UdpCheck.SetCheck(1);         //Ĭ��ѡ��udp

	index = 0;

	m_IP.SetWindowTextW(_T("192.165.53.152"));
	m_Port.SetWindowTextW(_T("6180"));

	m_SndCount.SetWindowTextW(_T("1"));
	m_IntervalTime.SetWindowTextW(_T("1"));
	Sleep(500);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CclientDlg::OnPaint()
{
#if 1
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
#endif
}
#if 1
//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
#endif 


BOOL CclientDlg::PreTranslateMessage(MSG* pMsg)
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_PORT);
	CString str;
    ASSERT(pEdit && pEdit->GetSafeHwnd());

    if((WM_KEYDOWN == pMsg->message) && (pEdit->GetSafeHwnd() == pMsg->hwnd))
    {
		if((pMsg->wParam >= _T('0') && pMsg->wParam <= _T('9'))
            || ((pMsg->wParam >= VK_NUMPAD0) && (pMsg->wParam <= VK_NUMPAD9)))
		{
			if(pEdit->GetWindowTextLengthW()<5){
				return CDialog::PreTranslateMessage(pMsg);
			}
			else
			{
				return TRUE;
			}
		}
		else if((_T('*') == pMsg->wParam) || (_T('#') == pMsg->wParam)
             || (VK_LEFT == pMsg->wParam) || (VK_RIGHT == pMsg->wParam)
             || (VK_UP == pMsg->wParam) || (VK_DOWN == pMsg->wParam)
             || (VK_DELETE == pMsg->wParam) || (VK_BACK == pMsg->wParam))
        {
            return CDialog::PreTranslateMessage(pMsg);
        }
        else
        {
            return TRUE;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}


void CclientDlg::GetServerAddr(data *d)
{
	//CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_PORT);

	CString sPort;

	//pEdit->GetWindowTextW(sPort);//��ȡ�˿��ַ���
	m_Port.GetWindowTextW(sPort);
	int dPort = _ttoi(sPort);//��ȡ�˿�����

	DWORD dwIP;
	m_IP.GetAddress(dwIP);//��ȡip

	sockaddr_in *serAddr;
	memset((char*)&(d->ServerAddr),0,sizeof(sockaddr));
	serAddr= (sockaddr_in *)&(d->ServerAddr);
    serAddr->sin_family = AF_INET;
    serAddr->sin_port = htons(dPort);
    serAddr->sin_addr.S_un.S_addr = htonl(dwIP); 

	d->ServerAddrLen = sizeof(sockaddr);
}
int CclientDlg::DisplayCurrConnectAddr(data *d)
{
	CString sPort;
	m_Port.GetWindowTextW(sPort);

	DWORD dwIP;
	m_IP.GetAddress(dwIP);//��ȡip

	unsigned char* pDwIP = (unsigned char*)&dwIP;

	CString name;
	name.Format(_T("%u.%u.%u.%u"),*(pDwIP+3),*(pDwIP+2),*(pDwIP+1),*pDwIP);
	name += _T(":") + sPort;
	CString Proto;
	if(d->protocol == SOCK_UDP)
	{
		Proto = _T("(UDP)");
	}
	else if (d->protocol == SOCK_TCP)
	{
		Proto = _T("(TCP)");
	}
	name += Proto;
	d->ListName = name;

	//�����õ�ַ�Ƿ��Ѿ�����
	if (m_list.FindString(-1,d->ListName) >= 0)
	{
		return -1;//�����ӷ���
	}
	
	m_list.SetItemData(m_list.AddString(name),(DWORD_PTR)d);

	return 0;
}
int /*CclientDlg::*/SockTcpRecv(SOCKET       Sock_Fd,char *buff,int len)
{
	int RecvLen = 0;
	RecvLen = recv(Sock_Fd, buff, len, 0);

	return RecvLen;
}
int /*CclientDlg::*/SockTcpSend(SOCKET       Sock_Fd,char *buff,int len)
{
	int SndLen = 0;
	SndLen =send(Sock_Fd,buff,len,0);

	return SndLen;
}
SOCKET CclientDlg::SockTcpStart(data *d)
{
	SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sclient == INVALID_SOCKET)
    {
        printf("invalid socket !");
        return sclient;
    }
	sockaddr ServerA;
	ServerA = d->ServerAddr;
	//��������
	if (connect(sclient, &ServerA, sizeof(ServerA)) == SOCKET_ERROR)
    {
		closesocket(sclient);
		AfxMessageBox(_T("connect error !"));
        printf("connect error !");
        return INVALID_SOCKET;
    }
	return sclient;
}
int /*CclientDlg::*/SockUdpRecv(SOCKET       Sock_Fd,char *buff,data *d,int len)
{
	int RecvLen = 0;
	d->ServerAddrLen = sizeof(sockaddr);
	RecvLen = recvfrom(Sock_Fd, buff, len, 0,&(d->ServerAddr),&(d->ServerAddrLen));

	return RecvLen;
}
int /*CclientDlg::*/SockUdpSend(SOCKET       Sock_Fd,char *buff,data *d,int len)
{
	int SndLen = 0;

	d->ServerAddrLen = sizeof(sockaddr);
	SndLen =sendto(Sock_Fd,buff,len,0,&(d->ServerAddr),(d->ServerAddrLen));

	return SndLen;
}

SOCKET CclientDlg::SockUdpStart()
{
	SOCKET sclient=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(sclient == INVALID_SOCKET)
    {
        printf("invalid socket !");
       
    }
	return sclient;
}

void CclientDlg::OnBnClickedButtonConnect()
{
	SOCKET sclient = 0;
	data *d = new data;

	//��ȡ����ķ�����IP��ַ��˿�
	GetServerAddr(d);

	if (m_TcpCheck.GetCheck() == 1)
	{
		sclient = SockTcpStart(d);

		d->protocol = SOCK_TCP;
	}
	else if (m_UdpCheck.GetCheck() == 1)
	{
		sclient = SockUdpStart();
		d->protocol = SOCK_UDP;
	}
	else 
	{
		AfxMessageBox(_T("��ѡ������Э��"));
		return;
	}
	if (sclient == INVALID_SOCKET)
	{
		AfxMessageBox(_T("�׽�����Ч"));
		return;
	}
	
	d->socket = sclient;
	d->index = index;

	//��ǰIP�Ͷ˿���ʾ
	if (DisplayCurrConnectAddr(d)< 0)
	{
		AfxMessageBox(_T("�������Ѵ���"));
		return;
	}
	
	DWORD dwThreadID;
	//HWND hwnd = GetSafeHwnd();
	HANDLE hThread = CreateThread(NULL, 10*1024, (LPTHREAD_START_ROUTINE)CclientDlg::GetServerData, (LPVOID)d, 0, &dwThreadID);
	
    if(hThread == NULL)   
    {   
         TerminateThread(hThread, 0);   
         return;   
    }

	index++;
	
	//�رվ�����رվ��������ر��̣߳�������رվ�����ᵼ��ϵͳ�ľ��Խ��Խ�٣����ղ����õ������
	CloseHandle(hThread);
	Sleep(10);
}


DWORD WINAPI CclientDlg::GetServerData(LPVOID lpParam)
{
	threadcount++;//�߳�����1

	char buffer[MAXSIZE] = {0};
	fd_set fds;
	int RecvLen = 0;
	data d = *(data *)lpParam;
	
	SOCKET client = *((SOCKET *)(lpParam));

	CListBox* listbox = (CListBox *)AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_LIST_CLIENT);
#if 0
	CString name;
	listbox->GetText(d.index,name);//��ȡ����,�����ļ���д

	CString filename = CString(".txt");
	filename = name + filename;
	
	CFile file; //�����ļ�����   
   
	//CString filename=L"D://test.txt";   
   
	/*
	modeCreate,ָ�����캯������һ�����ļ���������ļ��Ѿ����ڣ���ô�����ĳ��Ƚض�Ϊ0   
   
	modeWrite,���ļ������ļ�������д�����    
   
	shareDenyNone,������������������ļ�
	*/   
	if(file.Open(filename,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))    
	{    
   
		file.SeekToBegin(); //�����ļ���ͷ    
	   
		//file.Write("/xff/xfe", 2); //Unicode�����txt�ļ���0xff��0xfe�����ֽ���Ϊ��ͷ��ǡ�
	}
	else
	{
		closesocket(client);
		file.Close();
		return 0;
	}
#endif
	while(1)
	{
		RecvLen = 0;
		memset(buffer, 0, MAXSIZE);
		FD_ZERO( & fds);
        FD_SET(client, &fds);
		if (select(client + 1, &fds, NULL, NULL, NULL) == SOCKET_ERROR)
		{
			if (errno == WSAEINTR) 
				continue;
			
			break;
		}
		if (FD_ISSET(client, &fds))
		{
			if (d.protocol == SOCK_TCP)
			{
				RecvLen =SockTcpRecv(client, buffer,MAXSIZE-1);
			}
			else if (d.protocol == SOCK_UDP)
			{
				RecvLen = SockUdpRecv(client, buffer,&d,MAXSIZE-1);
			}
			if(RecvLen <= 0)
			{
				break;
			}

			RecvLen = get_server_msg(buffer,RecvLen);
			if (RecvLen < 0)
			{
				break;
			}
			RecvCount++;

			//��������Ϣ������ʾ
			CString RecvData(buffer);
			RecvData += _T("\r\n");
			RecvLen = RecvData.GetLength();
			
			CEdit* ServerMsg = (CEdit *)AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_EDIT_MESSAGEs);
			CString ServerOpt;
			ServerMsg->GetWindowTextW(ServerOpt);
			CString Count;
			Count.Format(_T("�� %d �� :\r\n"),RecvCount);
			ServerOpt += (Count + RecvData);
			ServerMsg->SetWindowTextW(ServerOpt);

			//��ʱ
			CString StrTime;
			CEdit* TimeMsg = (CEdit *)AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_EDIT_INTERVAL_TIME);
		    TimeMsg->GetWindowTextW(StrTime);
		    int time = _ttoi(StrTime)*1000;//ʱ�䵥λת��Ϊ s
	
			if (SndCount == 0)
			{
				AfxMessageBox(_T("success"));
				RecvCount = 0;
				SndAutoFlag = -1;
			} 
			else 
			{
				Sleep(time);
				CWnd* pWnd;
				pWnd=((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
				HWND hwnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
				::PostMessage(hwnd,WM_COMMAND,MAKEWPARAM(IDC_BUTTON_SND, BN_CLICKED),NULL);
				SndAutoFlag = 1;
			}
			
			//�ı���ʾ
			//file.Write(RecvData,RecvLen);
        }
		
        Sleep(5);
	}
	//file.Close();
	if (client>0)
		closesocket(client);
	
	//�˳����赯��
	if (ExitCloseFlag == 0)
	{
		AfxMessageBox(_T("���ӶϿ�"));
	//}
		//�߳���ֻ��ͨ��AfxGetApp����ȡ���ڲ������ؼ�
		//��ȡ�б����ָ��
		int findindex = listbox->FindString(-1,d.ListName);
		if(findindex>=0)
		{
			delete &d;
			listbox->DeleteString(findindex);
		}
	}

	SndAutoFlag = -1;
	RecvCount = 0;
	//�߳���-1
	threadcount--;
	return 0;
}


void CclientDlg::OnLbnSelchangeListClient()
{
	int index = m_list.GetCurSel();
	if(index != LB_ERR)
	{
	#if 0
		CString name;
		m_list.GetText(index,name);//��ȡ����,�����ļ���д

		CString filename = CString(".txt");
		filename = name + filename;

		CStdioFile m_file(filename,CFile::modeRead|CFile::shareDenyNone);
		CString contents;
		m_file.ReadString(contents);
		m_message.SetWindowTextW(contents);
	#endif
		CString title;
		m_list.GetText(index,title);
		m_showname.SetWindowTextW(title);
		//m_file.Close();
	}
}


void CclientDlg::OnBnClickedButtonClose()
{
	int index = m_list.GetCurSel();
	ExitCloseFlag = 0;
	if(index!=LB_ERR)
	{
		data *d = (data *)m_list.GetItemData(index);
		closesocket(d->socket);
		delete d;
		m_list.DeleteString(index);

		//�����ǰ���ӵ�ַ��ʾ
		CString clear("");
		m_showname.SetWindowTextW(clear);
	}
}


void CclientDlg::OnBnClickedButtonSnd()
{
	char buffer[MAXSIZE] = {0};
	int len = 0;

	int SndLen = 0;
	
	CString msg;
	
	send_msg.GetWindowTextW(msg);
	int index = m_list.GetCurSel();

	if(index !=LB_ERR)
	{
		data *d = (data *)m_list.GetItemData(index);
#if 1		
		//���ַ�ת��char����
		int n = msg.GetLength(); 

		int len = WideCharToMultiByte(CP_ACP,0,msg,msg.GetLength(),NULL,0,NULL,NULL);
		char * cmsg = new char[len+1];  
		memset(cmsg,0,len+1);
		WideCharToMultiByte(CP_ACP,0,msg,msg.GetLength(),cmsg,len,NULL,NULL);
		//cmsg[len] = '/0';//������Ϊ�ַ�����ʱ��Ӧ��д���ַ�����β�����Ƿ����û�й���/0�����Բ�������
#endif
		//len = convert_string_to_ctrl_frame(cmsg,buffer,len+1);
		//if(ctrl_frame_convert_result(len) != CTRL_ANL_OK)
		len = get_snd_msg(cmsg,buffer,len+1);
		if (len <= 0)
		{
			return;
		}

		if (SndAutoFlag != 1)
		{
			m_message.SetWindowTextW(_T(""));

			//���ʹ���
			CString StrCount;
			m_SndCount.GetWindowTextW(StrCount);
			SndCount = _ttoi(StrCount);
		}
		
		if (d->protocol == SOCK_TCP)
		{
			SndLen = SockTcpSend(d->socket, buffer,len);
		}
		else if (d->protocol == SOCK_UDP)
		{
			SndLen = SockUdpSend(d->socket, buffer,d,len);
		}
		if(SndLen <= 0)
		{
			return;
		}
		
		//if (SndCount != 0)
		{
			SndCount --;
		}
	
		Sleep(500);
		delete[] cmsg;//ʹ�����ɾ���ռ�
	}
}


void CclientDlg::OnBnClickedButtonExit()
{
	ExitCloseFlag = 1;
	int ListCount = m_list.GetCount();

	for (int i = ListCount - 1;i >= 0;i--)
	//for(int i=0;i<ListCount;i++)
	{
		data *d = (data *)m_list.GetItemData(i);
		closesocket(d->socket);//ѭ���ر�socket
		m_list.DeleteString(i);
		//Sleep(500);
		delete d;
	}
	//�����ǰ���ӵ�ַ��ʾ
	CString clear("");
	m_showname.SetWindowTextW(clear);
	
	while(threadcount>0)//����socket�رպ��̲߳�һ���رգ����Եȴ��߳̽�����
	{
		Sleep(100);
	}
	CclientDlg::OnCancel();
}



void CclientDlg::OnBnClickedButtonClearServer()
{
	CString clear("");

	m_message.SetWindowTextW(clear);
}


void CclientDlg::OnBnClickedButtonClearClnt()
{
	send_msg.SetWindowTextW(_T(""));
}
